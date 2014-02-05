/*
 * This file is part of the Marmalade SDK Code Samples.
 *
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This source code is intended only as a supplement to the Marmalade SDK.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
#ifndef IW_FRAME_H
#define IW_FRAME_H

// Includes
#include "IwArray.h"
#include "IwGeomMat.h"
#include "IwManaged.h"
#include "IwManagedList.h"
#include "s3eTypes.h"

/**
 * @defgroup IwFrame IwFrame
 */

/**
 * @addtogroup IwFrame
 * @brief Coordinate system base class - derived classes add extra matrices.
 * @{
 */

// Forward declarations
class CIwAnimBone;
class CIwAnimXForm;

//-----------------------------------------------------------------------------
// CIwFrame
//-----------------------------------------------------------------------------
/** Coordinate system, with local matrix only, no parent/children. */
class CIwFrame : public CIwManaged
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE(CIwFrame);

    /** Flags for all frame types */
    enum
    {
        // Bits 0..7 are flags that anyone can modify

        // Bits 8..31 are system flags that only core code can modify
        LOCAL_ROT_IDENTITY_F    = (1 << 8),     //!< m_MatLocal.m is identity
        WORLD_ROT_IDENTITY_F    = (1 << 9),     //!< m_MatWorld.m is identity
        WORLD_MAT_VALID_F       = (1 << 10),    //!< m_MatWorld   is valid (correctly calced from parent)
        PRIVATE_FLAGS_MASK      = 0xfff0,
    };

    // Methods
    CIwFrame();
    ~CIwFrame();

    /** Read/write binary file. */
    virtual void    Serialise();

    /** Resolve references to other objects once serialisation is complete. */
    virtual void    Resolve() {};

    /** Initialise frame to be identity, with no parent/children. */
    virtual void    Init();

    /** Get the local matrix, read only */
    inline  const CIwMat* GetMatLocal() const {return &m_MatLocal;}

    /** Get the world matrix, read only */
    virtual const CIwMat* GetMatWorld() {return &m_MatLocal;}

    /** Get write access to local matrix */
    inline CIwMat* ExposeMatLocal()
    {
        InvalidateMatWorld();
        return &m_MatLocal;
    }

    /** Only used by derived classes */
    virtual void    InvalidateMatWorld() {};

    /** Returns whether the world matrix of this frame has identity rotation */
    bool    IsRotWorldIdentity()        {return ((m_Flags & WORLD_ROT_IDENTITY_F) != 0);}

    /** Does the world matrix need updating */
    inline  bool    IsMatWorldValid()   {return ((m_Flags & WORLD_MAT_VALID_F) != 0);}

    /** Set the local matrix. */
    virtual void    SetMatLocal(const CIwMat& mat)
    {
        m_MatLocal  = mat;
        m_Flags     &= ~(LOCAL_ROT_IDENTITY_F);
    }

    /** Set rotation component only of local matrix */
    virtual void    SetRotLocal(CIwMat& mat)
    {
        m_MatLocal.CopyRot(mat);
        m_Flags &= ~(LOCAL_ROT_IDENTITY_F);
    }

    /** Set rotation component only of local matrix to identity */
    virtual void    SetRotLocalIdent()
    {
        m_MatLocal.CopyRot(CIwMat::g_Identity);
        m_Flags |= (LOCAL_ROT_IDENTITY_F);
    }

    /** Set translation component only of local matrix */
    virtual void    SetTransLocal(CIwVec3& vec)
    {
        m_MatLocal.t = vec;
    }

    /** Update the frame */
    virtual void    Update() {};

    /** Update the world matrix if necessary */
    inline void UpdateWorld()
    {
        if (!IsMatWorldValid())
            Update();
    }

    /** Write flags */
    inline void WriteFlags(uint32 flags)
    {
        IwAssertMsgN(288,GRAPHICS, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags = (m_Flags & PRIVATE_FLAGS_MASK) | flags;
    }

    /** Set flags */
    inline void SetFlags(uint32 flags)
    {
        IwAssertMsgN(289,GRAPHICS, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags |= flags;
    }

    /** Clear flags */
    inline void ClearFlags(uint32 flags)
    {
        IwAssertMsgN(290,GRAPHICS, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags &= ~flags;
    }

    /** Get flags */
    inline uint32 GetFlags() const {return m_Flags;}

    /** Copy a CIwFrame. This includes the flags. */
    inline void Copy(const CIwFrame& src)
    {
        m_Flags = src.m_Flags;
        m_MatLocal = src.m_MatLocal;
    }

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
protected:
    uint32      m_Flags;        //!< flags
    CIwMat      m_MatLocal;     //!< local-space (model-space) matrix
};

//-----------------------------------------------------------------------------
// CIwFrameLW
//-----------------------------------------------------------------------------
/** Coordinate system, with local and world matrices, and parent/children. */
class CIwFrameLW : public CIwFrame
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE(CIwFrameLW);

    // Methods
    CIwFrameLW();
    ~CIwFrameLW();

    //-------------------------------------------------------------------------
    // Derived API
    //-------------------------------------------------------------------------
    /** Read/write binary file. */
    virtual void    Serialise();

    /** Resolve references to other objects once serialisation is complete. */
    virtual void    Resolve();

    /** Initialise frame to be identity, with no parent/children. */
    virtual void    Init();

    /** Get the world matrix. */
    virtual const CIwMat* GetMatWorld();

    /** Set the local matrix. */
    virtual void    SetMatLocal(const CIwMat& mat)
    {
        m_MatLocal = mat;
        m_Flags &= ~(LOCAL_ROT_IDENTITY_F);
        InvalidateMatWorld();
        UpdateWorldFlags();
    }

    /** Set rotation component only of local matrix */
    virtual void    SetRotLocal(CIwMat& mat)
    {
        m_MatLocal.CopyRot(mat);
        m_Flags &= ~(LOCAL_ROT_IDENTITY_F);
        InvalidateMatWorld();
        UpdateWorldFlags();
    }

    /** Set rotation component only of local matrix to identity */
    virtual void    SetRotLocalIdent()
    {
        m_MatLocal.CopyRot(CIwMat::g_Identity);
        m_Flags |= (LOCAL_ROT_IDENTITY_F);
        InvalidateMatWorld();
        UpdateWorldFlags();
    }

    /** Set translation component only of local matrix */
    virtual void    SetTransLocal(CIwVec3& vec)
    {
        m_MatLocal.t = vec;
        InvalidateMatWorld();
    }

    /** Clear the WORLD_MAT_VALID_F flag for this frame and all its children */
    virtual void    InvalidateMatWorld();

    /** Update the frame and its hierarchy. */
    virtual void    Update();

    //-------------------------------------------------------------------------
    // New API
    //-------------------------------------------------------------------------
    /** Destroy the entire hierarchy beneath this frame. */
    void    DestroyChildren();

    /** Add a child frame. Maintains hierarchy consistency */
    void    AddChild(CIwFrameLW* pChild);

    /** Remove a child frame, returning true only if it existed in the hierarchy. Patches the hierarchy. Does NOT destroy the removed frame. */
    bool    RemoveChild(CIwFrameLW* pChild);

    /** Get the frame parent. */
    inline  CIwFrameLW* GetParent() {return m_Parent;}

    /** Update all flags by checking status of local translation and rotation components. */
    inline  void    UpdateFlags()
    {
        CheckLocalFlags();
        UpdateWorldFlags();
    }

    /** Update local flags by checking status of local translation and rotation components. */
    void    CheckLocalFlags();

    /** Update world flags from local flags and parent local flags. */
    void    UpdateWorldFlags();

    /** Get the number of children */
    inline uint32 GetNumChildren() {return m_Children.GetSize();}

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
protected:
    CIwMat          m_MatWorld;     //!< world-space (global-space) matrix
    CIwFrameLW*     m_Parent;       //!< parent frame, or NULL
    CIwManagedList  m_Children;     //!< any children

    friend void _UpdateBoneMatHierarchyFromXForms(CIwAnimBone* pBone, CIwAnimXForm* pXForms);
};

#endif

// @}
