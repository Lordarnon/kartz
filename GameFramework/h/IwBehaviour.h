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

/**
 * @defgroup IwBehaviour IwBehaviour
 */

/**
 * @addtogroup IwBehaviour
 * @brief Entity behaviour base classes
 * @{
 */

#ifndef IW_BEHAVIOUR_H
#define IW_BEHAVIOUR_H

// Includes
#include "IwGeomVec.h"
#include "IwManaged.h"

// Forward declarations
class CIwEnt;
class CIwMenu;

/** Return true if behaviour is a subclass of the specified classname. */
#define IS_BEHAVIOUR(b, c) (b)->IsSubClass(##c_F)

//-----------------------------------------------------------------------------
// CIwBehaviour
//-----------------------------------------------------------------------------
/** Entity behaviour base class */
class CIwBehaviour : public CIwManaged
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Flags and types */
    enum
    {
        INITIALISED_F       = (1 << 0),     //!< Object has been initialised
        ACTIVE_F            = (1 << 1),     //!< update this behaviour.

        // Subclass flags
        CIwBeMotion_F       = (1 << 2),     //!< behaviour is derived from CIwBeMotion
        CIwBeRender_F       = (1 << 3),     //!< behaviour is derived from CIwBeRender
        CIwBeCollision_F    = (1 << 4),     //!< behaviour is derived from CIwBeCollision
        USER_CLASS_F        = (1 << 5),     //!< user classes

        CLASS_FLAG_MASK     = ~(INITIALISED_F | ACTIVE_F)   //!< Mask of class flags
    };

    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE_NOCOPY(CIwBehaviour);

    // Methods
    CIwBehaviour();
    virtual ~CIwBehaviour() {};
    virtual void Serialise();
    virtual void Resolve() {};

    /** Set parent entity. This is usually called from within CIwEnt::AttachBehaviour, not explicitly by the user. */
    INLINE_MEMBER   void    SetEnt(CIwEnt* pEnt)    {m_Ent = pEnt;}

    /** Get parent entity. */
    INLINE_MEMBER   CIwEnt* GetEnt() const {return m_Ent;}

    /** Can be called by user to initialise the behaviour */
    virtual void    Init() { m_BaseFlags |= INITIALISED_F; }

    /** Update tick */
    virtual void    Update() {};

    /** Set behaviour as active/inactive (i.e. whether or not it will be updated). */
    virtual void    SetActive(bool active)
    {
        if (active == true)
            m_BaseFlags |= ACTIVE_F;
        else
            m_BaseFlags &= ~ACTIVE_F;
    }

    /** Return true only if the behaviour is active. */
    inline bool IsActive() const {return (m_BaseFlags & ACTIVE_F) != 0;}

    /** Return true only if the behaviour is intialised. */
    inline bool IsInitialised() const {return (m_BaseFlags & INITIALISED_F) != 0;}

    /** Return true if behaviour is a subclass of the specified type. */
    inline bool IsSubClass(uint32 flag) const {return (m_BaseFlags & flag) != 0;}

    /** Return all behaviour class flags */
    inline uint32 GetClassFlags() const {return m_BaseFlags & CLASS_FLAG_MASK;}

    /** When the parent ent's OnDeath is called... */
    virtual void    OnDeath() {}

#ifdef IW_DEBUG
    /** Debug rendering */
    void    DebugRender();

    /** Debug method to add items to menu */
    void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

protected:
    void    SetClassFlags(uint32 flags);

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
protected:
    uint32      m_BaseFlags;    //!< run-time type info and other flags
    CIwEnt*     m_Ent;          //!< ptr to parent entity
};

//-----------------------------------------------------------------------------
// CIwBeRender
//-----------------------------------------------------------------------------
/** Entity render behaviour base class */
class CIwBeRender : public CIwBehaviour
{
public:
    IW_MANAGED_DECLARE(CIwBeRender);
    CIwBeRender() {SetClassFlags(CIwBeRender_F);}
};

//-----------------------------------------------------------------------------
// CIwBeMotion
//-----------------------------------------------------------------------------
/** Entity motion behaviour base class */
class CIwBeMotion : public CIwBehaviour
{
public:
    IW_MANAGED_DECLARE(CIwBeMotion);
    CIwBeMotion() {SetClassFlags(CIwBeMotion_F);}

    // Members
    int32       m_Speed;        //!< Speed (calculated from m_Vel)
    CIwVec3     m_Vel;          //!< Velocity
    CIwVec3     m_Acc;          //!< Acceleration
    CIwVec3     m_AngVel;       //!< Angular velocity about local axes
};

//-----------------------------------------------------------------------------
// CIwBeCollision
//-----------------------------------------------------------------------------
/** Entity collision behaviour base class */
class CIwBeCollision : public CIwBehaviour
{
public:
    IW_MANAGED_DECLARE(CIwBeCollision);
    CIwBeCollision() {SetClassFlags(CIwBeCollision_F);}
};

#endif

// @}
