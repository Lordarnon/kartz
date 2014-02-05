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
#ifndef IW_RENDERSLOTS_H
#define IW_RENDERSLOTS_H

// Includes
#include "s3eTypes.h"
#include "IwManaged.h"

/**
 * @defgroup IwRenderSlots IwRenderSlots
 */

/**
 * @addtogroup IwRenderSlots
 * @brief Default renderslots to add to rendermanager.
 * @{
 */

// Forward declarations

//-----------------------------------------------------------------------------
// CIwRenderSlot
//-----------------------------------------------------------------------------
/** Base class for render slot */
class CIwRenderSlot : public CIwManaged
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE(CIwRenderSlot);

    // Methods
    CIwRenderSlot() : m_Active(true) {};
    virtual ~CIwRenderSlot() {};

    /** Serialise (empty) */
    virtual void    Serialise() {};

    /** Resolve (empty) */
    virtual void    Resolve() {};

    /** Render */
    virtual void    Render() = 0;

    /** Set active status */
    inline  void    SetActive(bool active)  {m_Active = active;}

    /** Get active status */
    inline  bool    GetActive()             {return m_Active;}

#ifdef IW_DEBUG
    /** Add debug options to menu */
    void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
    bool    m_Active;   //!< slot is active
};

//-----------------------------------------------------------------------------
// CIwRenderSlotClear
//-----------------------------------------------------------------------------
/** Render slot: clear backbuffer */
class CIwRenderSlotClear : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotClear);
    CIwRenderSlotClear() {SetName("CIwRenderSlotClear");}
    void    Render();
};

//-----------------------------------------------------------------------------
// CIwRenderSlotBegin
//-----------------------------------------------------------------------------
/** Render slot: begin rendering */
class CIwRenderSlotBegin : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotBegin);
    CIwRenderSlotBegin() {SetName("CIwRenderSlotBegin");}
    void    Render();
};

//-----------------------------------------------------------------------------
// CIwRenderSlotEnd
//-----------------------------------------------------------------------------
/** Render slot: end rendering */
class CIwRenderSlotEnd : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotEnd);
    CIwRenderSlotEnd() {SetName("CIwRenderSlotEnd");}
    void    Render();
};

//-----------------------------------------------------------------------------
// CIwRenderSlotGeoms
//-----------------------------------------------------------------------------
/** Render slot: all CGeoms, and any gamestate-specific 3D rendering */
class CIwRenderSlotGeoms : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotGeoms);
    CIwRenderSlotGeoms() {SetName("CIwRenderSlotGeoms");}
    void    Render();
};

//-----------------------------------------------------------------------------
// CIwRenderSlotFlush
//-----------------------------------------------------------------------------
/** Render slot: flushed cached geometry */
class CIwRenderSlotFlush : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotFlush);
    CIwRenderSlotFlush() {SetName("CIwRenderSlotFlush");}
    void    Render();
};

//-----------------------------------------------------------------------------
// CIwRenderSlotGameStates
//-----------------------------------------------------------------------------
/** Render slot: game states */
class CIwRenderSlotGameStates : public CIwRenderSlot
{
public:
    IW_MANAGED_DECLARE(CIwRenderSlotGameStates);
    CIwRenderSlotGameStates() {SetName("CIwRenderSlotGameStates");}
    void    Render();
};



#endif

// @}
