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
#ifndef IW_GAMESTATE_H
#define IW_GAMESTATE_H

// Includes
#include "s3eTypes.h"
#include "IwManaged.h"


/**
 * @defgroup IwGameState IwGameState
 */

/**
 * @addtogroup IwGameState
 * @brief Base class for a game state.
 * @{
 */

// Forward declarations
class CIwMenu;

//-----------------------------------------------------------------------------
// CIwGameState
//-----------------------------------------------------------------------------
/** Base class for game state */
class CIwGameState : public CIwManaged
{
public:
    IW_MANAGED_DECLARE(CIwGameState);

    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Flags */
    enum
    {
        UPDATE_F                = (1 << 0), //!< state should be updated
        RENDER_F                = (1 << 1), //!< state should be rendered
        REPLACE_PREV_F          = (1 << 2), //!< when pushed onto stack, will pop any existing top state
        DELETE_PREV_F           = (1 << 3), //!< when pushed onto stack, will delete any existing top state
        DELETE_ALL_PREV_F       = (1 << 4), //!< when pushed onto stack, will delete all existing states
        DELETE_WHEN_NOT_TOP_F   = (1 << 5), //!< delete this state when it ceases to be the top state
        DELETE_WHEN_POPPED_F    = (1 << 6), //!< delete this state when it is popped from the stack

        PRIVATE_FLAGS_MASK      = 0x0,
    };

    // Methods
    CIwGameState();
    virtual ~CIwGameState();

    /** Write flags */
    inline void WriteFlags(uint32 flags)
    {
        IwAssertMsgN(291,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags = (m_Flags & PRIVATE_FLAGS_MASK) | flags;
    }

    /** Set flags */
    inline void SetFlags(uint32 flags)
    {
        IwAssertMsgN(292,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags |= flags;
    }

    /** Clear flags */
    inline void ClearFlags(uint32 flags)
    {
        IwAssertMsgN(293,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags &= ~flags;
    }

    /** Get flags */
    inline uint32 GetFlags() const {return m_Flags;}

    /** Called as soon as state becomes the top entry on the stack. */
    virtual void    EnterTop()
    {
        m_Flags |= UPDATE_F | RENDER_F;
    };

    /** Called as soon as state leaves the top of the stack - even if it has become inactive. */
    virtual void    ExitTop()
    {
        m_Flags &= ~(UPDATE_F | RENDER_F);
    };

    /** Called if state is on the stack, and is active. */
    virtual void    Update() {};

    /** Called if state is on the stack, and is active. */
    virtual void    Render() {};

#ifdef IW_DEBUG
    virtual void    DebugRender() {};
    virtual void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

    // Members
    uint32      m_Flags;    //!< flags
};

#endif

// @}
