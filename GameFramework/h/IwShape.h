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
#ifndef IW_SHAPE_H
#define IW_SHAPE_H

// Our includes
#include "IwGeomVec.h"
#include "IwManaged.h"
#include "IwDebugPrim.h"
#include "IwColour.h"

/**
 * @defgroup IwShape IwShape
 */

/**
 * @addtogroup IwShape
 * @brief Shape base class.
 * @{
 */
// Forward declarations

//-----------------------------------------------------------------------------
// CIwShape
//-----------------------------------------------------------------------------
/** Shape base class. Abstract. */
class CIwShape : public CIwManaged
{
public:
    IW_MANAGED_DECLARE(CIwShape);

    /** Flags */
    enum
    {
        // Private
        INITIALISED_F               = (1 << 0),     //!< Shape is initialised.
        CLOSED_F                    = (1 << 1),     //!< Shape is closed
        PRIVATE_FLAGS_MASK          = (0x3),        //!< Mask the flags that are non-modifiable

        // Public
        DEBUG_RENDER_F              = (1 << 2),     //!< Render shape debug geometry
        DEBUG_RENDER_NAME_F         = (1 << 3),     //!< Render shape debug name

        // Type-specific
        TYPE_F                      = (1 << 4),
        // IwVolume
        TYPE_CAMERA_F               = (1 << 5),     //!< Volume is a camera volume
        TYPE_TRIGGER_F              = (1 << 6),     //!< Volume is a trigger volume
        // IwPath
        NO_PROJECT_F                = (1 << 6),     //!< Do NOT project path points onto terrain.
        DEBUG_RENDER_POINT_IDS_F    = (1 << 7),     //!< Debug render path point IDs.
    };

    // Methods
    CIwShape();
    virtual ~CIwShape() {};

    /** Initialise shape. Pre-calculate any necessary dimensions etc. */
    virtual void    Init();

    /** Check if a world point is inside the CLOSED shape. */
    virtual bool    IsPointInside(const CIwVec3& p) = 0;

    /** Write flags */
    inline void WriteFlags(uint32 flags)
    {
        IwAssertMsgN(297,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags = (m_Flags & PRIVATE_FLAGS_MASK) | flags;
    }

    /** Set flags */
    inline void SetFlags(uint32 flags)
    {
        IwAssertMsgN(298,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags |= flags;
    }

    /** Clear flags */
    inline void ClearFlags(uint32 flags)
    {
        IwAssertMsgN(299,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags &= ~flags;
    }

    /** Test a flag */
    inline bool TestFlag(uint32 test) { return ((m_Flags & test) != 0); }

    /** Get flags */
    inline uint32 GetFlags() const {return m_Flags;}

    /** Return true only if shape has been initialised. */
    inline bool IsInitialised() {return (m_Flags & INITIALISED_F) != 0;}

    /** IO functions */
    virtual void    Serialise();
    virtual void    ParseClose(CIwTextParserITX* pParser);
    virtual bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    /** Resolve references to other objects once serialisation is complete. */
    virtual void    Resolve();

#ifdef IW_DEBUG
    CIwColour           m_DebugColour;              //!< Colour to draw this instance
    /** Default colour to draw objects of this type */
    virtual CIwColour   GetDebugColourDefault();
    /** Render shape debug geometry */
    virtual void        DebugRender();
    /** Add items to debug menu */
    virtual void        DebugAddMenuItems(CIwMenu* pMenu);
    /** Render shape name */
    virtual void        DebugRenderName() = 0;
#endif

    // Members
protected:
    uint32  m_Flags;    //!< flags, e.g. CLOSED_F

    // Static members
public:
    static  const char* g_FlagNames[];  //!< Flag identifiers as found in ITX files
};

#endif

// @}
