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
#ifndef IW_GAME_H
#define IW_GAME_H

// Includes
#include "s3eTypes.h"
#include "IwManaged.h"
#include "IwManagedList.h"
#include "IwSingleton.h"

/**
 * @defgroup IwGame IwGame
 */

/**
 * @addtogroup IwGame
 * @brief Base class for main game and game states.
 * @{
 */

// Forward declarations
class CIwGame;
class CIwGameState;
class CIwMenu;

//--------------------------------------------------------------------------------
// CIwGameClock
//--------------------------------------------------------------------------------
/** Class for handling game framerate synchronisation */
class CIwGameClock
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    CIwGameClock();
    virtual ~CIwGameClock();

    /** Update timer */
    void    UpdateTimer();

    /** Update, and call game's UpdateStep() / UpdateControls() methods */
    void    Update();

    /** Set game paused status */
    virtual void    SetPaused(bool state)   {m_Paused = state;}

    /** Get game paused status */
    inline  bool    GetPaused()             {return m_Paused;}

    /** Set frame delay (once and once only) */
    void    InitFramerate(int32 fps);

    /** Alter frame delay (dangerous?!) */
    void    ChangeFramerate(int32 fps);

    // Members
    bool        m_Paused;               //!< true if game paused
    uint32      m_UpdateFrames;         //!< total number of update frames (calls to Update()) since app start
    uint32      m_RenderFrames;         //!< total number of render frames (calls to Render()) since app start
    uint32      m_RenderFramesLast;     //!< stored last value of above... update code can then check if a render has occurred
    int32       m_Milliseconds;         //!< total number of milliseconds elapsed since app start
    int32       m_MillisecondsPassed;   //!< number of milliseconds since last update
    int32       m_MillisecondsPerLogicTick;
    bool        m_FrameUnlimited;       //!< set this to enable uncapped frame rates
};
IW_SINGLETON_EXTERN(GameClock);

//-----------------------------------------------------------------------------
// CIwGame
//-----------------------------------------------------------------------------
/** CIwGame class */
class CIwGame : public CIwManaged
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    IW_MANAGED_DECLARE_NOCOPY(CIwGame);

    // Exit states
    enum ExitState
    {
        EXIT_NONE,
        EXIT_RESTART,
        EXIT_QUIT,
    };

    // Methods
    CIwGame();
    virtual ~CIwGame();

    /** Clear existing game state (and possibly delete it). */
/*  void    ClearState();

    /// Change game state with immediate effect.
    void    SetNewState(CIwGameState* pState, bool stateToDie);

    /// Request change of game state (will actually be set at next call to Update())
    inline  void    RequestNewState(CIwGameState* pState, bool stateToDie)
    {
        m_StateNext         = pState;
        m_StateNextToDie    = stateToDie;
    }

    /// Get the game state
    inline  CIwGameState*   GetState()      {return m_State;}

    /// Update game state changes
    void    UpdateStateChange();
*/
    /** Push state to stack. */
    void    PushState(CIwGameState* pState);

    /** Delayed state push */
    void    QueueState(CIwGameState* pState);

    /** Pop state from stack. */
    CIwGameState* PopState();

    /** Get the top state on the stack. */
    inline CIwGameState* GetState()
    {
        if (m_States.GetSize())
            return (CIwGameState*)m_States.GetTop();
        else
            return NULL;
    }

    inline CIwGameState* GetUnderState()
    {
        if (m_States.GetSize() >= 2)
            return (CIwGameState*)m_States[m_States.GetSize()-2];
        else
            return NULL;
    }
    inline CIwGameState* GetTopState()
    {
        if (m_States.GetSize() >= 1)
            return (CIwGameState*)m_States[0];
        else
            return NULL;
    }

    /** Delete all states on the stack. */
    inline void DeleteStates() {m_States.Delete();}

    /** Pop all states from the stack. */
    inline void PopStates()
    {
        while (m_States.GetSize())
            PopState();
    }

    /** Update all (active) states on the stack. */
    void    UpdateStates();

    /** Render all (active) states on the stack. */
    void    RenderStates();

    /** Remove state from the stack. */
    inline bool RemoveState(CIwGameState* pState) {return m_States.RemoveSlow((CIwManaged*)pState);}

    /** Run game until exit */
    virtual void    Run() {}

    /** Main update - calls UpdateTimer(), which calls UpdateTick() and UpdateControls() */
    virtual void    Update();

    /** Update game tick */
    virtual void    UpdateTick() {};

    /** Update controls */
    virtual void    UpdateControls() {};

    /** Debug: update */
    virtual void    DebugUpdate() {};

    // Members
public:
    ExitState       m_ExitState;        //!< e.g. EXIT_QUIT

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
protected:
//  CIwGameState*   m_State;            //!< Current game state
    CIwManagedList  m_States;           //!< Stack of game states
    CIwGameState*   m_QueuedState;      //!< Queued state change

//  bool            m_StateToDie;       //!< True if state should be deleted when changed
//  CIwGameState*   m_StateNext;        //!< Pending game state
//  bool            m_StateNextToDie;   //!< True if state should be deleted when changed

//  friend class CIwGameState;
};
IW_SINGLETON_EXTERN(Game);

#endif

// @}
