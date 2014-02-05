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
/*
 * Base class for main game and game states.
 */

#include "IwGame.h"
#include "IwGameState.h"
#include "IwGx.h"
#include "IwRuntime.h"
#include "s3eDevice.h"

#include "s3eTimer.h"
#define doTickCount s3eTimerGetMs

// Singletons
IW_SINGLETON_INSTANTIATE(Game);
IW_SINGLETON_INSTANTIATE(GameClock);

#define IW_ENGINE_CLOCK_FORCE_FRAME_RATE

static CIwGameState* s_CurrentlyRunningState = 0;

//--------------------------------------------------------------------------------
// CIwGameClock
//--------------------------------------------------------------------------------
CIwGameClock::CIwGameClock()
{
    IW_CALLSTACK("CIwGameClock::CIwGameClock");
    IW_SINGLETON_SET(GameClock);

    m_MillisecondsPerLogicTick  = 0; // this *must* be set explicitly with InitFramerate()
    m_MillisecondsPassed        = 0;
    m_Milliseconds              = 0;
    m_UpdateFrames              = 0;
    m_RenderFrames              = 0;
    m_RenderFramesLast          = 0;
    m_Paused                    = false;
    m_FrameUnlimited            = false;
}
//--------------------------------------------------------------------------------
CIwGameClock::~CIwGameClock()
{
    IW_CALLSTACK("CIwGameClock::~CIwGameClock");
    IW_SINGLETON_CLEAR(GameClock);
}
//--------------------------------------------------------------------------------
void    CIwGameClock::UpdateTimer()
{
    IW_CALLSTACK("CIwGameClock::UpdateTimer")

    // Update timers
    static int32 t = (int32) doTickCount();
    int32 t2 = (int32) doTickCount();

    // If we're not paused, add on any extra milliseconds since the last call
    // to UpdateTimer()
    if (!GetPaused())
    {
        m_Milliseconds          += t2 - t;
        m_MillisecondsPassed    += t2 - t;
    }

    // Update static variable for future comparisons
    t = t2;

    // This occurs when the clock wraps, and needs to be dealt with
    if (m_MillisecondsPassed < 0)
    {
        m_MillisecondsPassed = 0;
    }

    // Deal with spikes by bounding milliseconds passed
    if (m_MillisecondsPassed > m_MillisecondsPerLogicTick * 8)
    {
        m_MillisecondsPassed = m_MillisecondsPerLogicTick;
    }
}
//--------------------------------------------------------------------------------
void    CIwGameClock::Update()
{
    if (m_FrameUnlimited)
    {
        // Update exactly once per rendered frame
        IwGetGame()->UpdateControls();
        IwGetGame()->DebugUpdate();
        m_UpdateFrames++;
        IwGetGame()->UpdateTick();
        return;
    }

    IW_CALLSTACK("CIwGameClock::Update")

    // If we haven't set a framerate, bad things will happen, so verify...
    IwAssertMsg(ENGINE, m_MillisecondsPerLogicTick != 0,
        ("InitFramerate() has not been called"));

    if (!GetPaused())
    {
        static uint32 d = (int32) doTickCount();

        // Force frame rate down to be similar to on-device
        // (by constantly updating timer until we're ready for a new tick)
        do
        {

            UpdateTimer();
            s3eDeviceYield(0);
        }
        while ((int32)(doTickCount() - d) < m_MillisecondsPerLogicTick);

        d = (int32) doTickCount();
    }
    else
    {
        s3eDeviceYield(0);
        // Paused
        IwGetGame()->UpdateControls();
        IwGetGame()->Update();
    }

    // UpdateTimer now respects the paused setting, so it's always safe to call.
    UpdateTimer();

#ifdef IW_USE_PROFILE
    IW_PROFILE("Update")
#endif

    s3eDeviceYield(0);
    if (m_MillisecondsPassed >= m_MillisecondsPerLogicTick)
    {
        // We're ready to execute at least one frame
        while (m_MillisecondsPassed >= m_MillisecondsPerLogicTick)
        {
            s3eDeviceYield(0);
            m_MillisecondsPassed -= m_MillisecondsPerLogicTick;

            // Read controls
            IwGetGame()->UpdateControls();
            IwGetGame()->DebugUpdate();

            // Timer update (So we'll iterate this loop again if we need to)
            int32 oldMilliseconds = m_Milliseconds;

            UpdateTimer();

            m_UpdateFrames++;

            // Call game object update tick
            IwGetGame()->UpdateTick();

            if (m_Milliseconds - oldMilliseconds > m_MillisecondsPerLogicTick)
            {
                return; //just give up
            }
        }
    }
    else
    {
        // Not ready for another frame yet. Just update controls
        IwGetGame()->UpdateControls();
    }
}
//--------------------------------------------------------------------------------
void CIwGameClock::InitFramerate(int32 fps)
{
    IW_CALLSTACK("CIwGameClock::InitFramerate")

    IwAssertMsg(ENGINE, m_MillisecondsPerLogicTick == 0,
                ("InitFramerate() should only be called once"));
    ChangeFramerate(fps);
}

void CIwGameClock::ChangeFramerate(int32 fps)
{
    if (fps > 0)
    {
        int32 res = s3eTimerGetInt(S3E_TIMER_RESOLUTION);
        int32 millis = 1000 / fps;
        if (res != 1) //if counter resolution is not millisecond-accurate
        {
            int32 mod = millis % res;

            //go to nearest milli count
            if (mod > res/2)
                millis += mod;
            else
                millis -= mod;

            if (millis == 0) //if requested frame rate is above counter resolution
                millis = res; //do at least some physics!
        }

        m_MillisecondsPerLogicTick = millis;
    }
    else
    {
        m_MillisecondsPerLogicTick = 0;
    }

}


//-----------------------------------------------------------------------------
// CIwGame
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwGame)
CIwGame::CIwGame()
{
    IW_CALLSTACK("CIwGame::CIwGame")
    IW_SINGLETON_SET(Game);

    new CIwGameClock;
    m_ExitState = EXIT_NONE;
    m_QueuedState = NULL;
}
//-----------------------------------------------------------------------------
CIwGame::~CIwGame()
{
    IW_CALLSTACK("CIwGame::~CIwGame")

    delete IwGetGameClock();
    IW_SINGLETON_CLEAR(Game);
}
//-----------------------------------------------------------------------------
void    CIwGame::PushState(CIwGameState* pState)
{
    IW_CALLSTACK("CIwGame::PushState")
    IwAssertMsg(ENGINE, pState, ("Null ptr"));

    // Look for specified state on stack. If it's there, pop states until the specified on is on top.
    for (uint32 i=0; i<m_States.GetSize(); i++)
    {
        if (m_States[i] == pState)
        {
            while (m_States.GetSize() > i+1)
                PopState();
            return;
        }
    }

    if (m_States.GetSize())
    {
        // Get current top state
        CIwGameState* pTop = (CIwGameState*)m_States.GetTop();
        pTop->ExitTop();

        if (pState->GetFlags() & CIwGameState::DELETE_ALL_PREV_F)
        {
            // Pop and delete all other states (as instructed by new state)
            while (true)
            {
                m_States.Pop();
                IwAssert(ENGINE, pTop != s_CurrentlyRunningState);
                delete pTop;

                if (m_States.GetSize())
                {
                    pTop = (CIwGameState*)m_States.GetTop();
                    pTop->ExitTop();
                }
                else
                {
                    break;
                }
            }
        }
        else
        if (pTop->GetFlags() & CIwGameState::DELETE_WHEN_NOT_TOP_F)
        {
            // Pop and delete top state (old state is telling us to do this)
            m_States.Pop();
            IwAssert(ENGINE, pTop != s_CurrentlyRunningState);
            delete pTop;
        }
        else
        if (pState->GetFlags() & CIwGameState::DELETE_PREV_F)
        {
            // Pop and delete top state (new state is telling us to do this)
            m_States.Pop();
            IwAssert(ENGINE, pTop != s_CurrentlyRunningState);
            delete pTop;
        }
        else
        if (pState->GetFlags() & CIwGameState::REPLACE_PREV_F)
        {
            // Pop but do NOT delete top state
            m_States.Pop();
        }
        else
        {
            // Default behaviour: set top state to inactive
        }
    }

    // Push new state
    m_States.Push(pState);
    pState->EnterTop();
}
//-----------------------------------------------------------------------------
void    CIwGame::QueueState(CIwGameState* pState)
{
    IW_CALLSTACK("CIwGame::QueueState")

    // if this will override a state already queued, destroy it
    if (m_QueuedState && m_States.Find(pState) == -1)
    {
        IwAssert(ENGINE,false);
        delete m_QueuedState;
    }

    m_QueuedState = pState;
}
//-----------------------------------------------------------------------------
CIwGameState* CIwGame::PopState()
{
    IW_CALLSTACK("CIwGame::PopState")
    IwAssertMsg(ENGINE, m_States.GetSize(), ("No states on stack"));

    // Get current top state
    CIwGameState* pTop = (CIwGameState*)m_States.GetTop();
    pTop->ExitTop();
    m_States.Pop();

    if (pTop->GetFlags() & (CIwGameState::DELETE_WHEN_NOT_TOP_F | CIwGameState::DELETE_WHEN_POPPED_F))
    {
        // Delete state - assert is commented because states need to pop themselves; this correct?
        //IwAssert(ENGINE, pTop != s_CurrentlyRunningState);
        delete pTop;
        pTop = NULL;
    }

    // Enter new top state?
    if (m_States.GetSize())
        GetState()->EnterTop();

    return pTop;
}
//-----------------------------------------------------------------------------
void    CIwGame::UpdateStates()
{
    IW_CALLSTACK("CIwGame::UpdateStates")
    IwAssert(ENGINE, 0 == s_CurrentlyRunningState);

    // Process delayed state change
    if (m_QueuedState)
    {
        PushState(m_QueuedState);
        m_QueuedState = NULL;
    }

    for (uint i=0; i<m_States.GetSize(); i++)
    {
        IwAssert(ENGINE, 0 == m_QueuedState);   // We only expect the top-most state to set this usually!

        CIwGameState* pState = (CIwGameState*)m_States[i];
        if (pState->GetFlags() & CIwGameState::UPDATE_F)
        {
            s_CurrentlyRunningState = pState;
            pState->Update();
            s_CurrentlyRunningState = 0;
        }
    }
}
//-----------------------------------------------------------------------------
void    CIwGame::RenderStates()
{
    IW_CALLSTACK("CIwGame::RenderStates")
    IwAssert(ENGINE, 0 == s_CurrentlyRunningState);

    s3eDeviceYield(0);

    IW_MANAGED_LIST_ITERATE(m_States, itS)
    {
        CIwGameState* pState = (CIwGameState*)*itS;
        if (pState->GetFlags() & CIwGameState::RENDER_F)
            pState->Render();
    }
}
//-----------------------------------------------------------------------------
void    CIwGame::Update()
{
    IW_CALLSTACK("CIwGame::Update")

    // Check quit state
    if (s3eDeviceCheckQuitRequest())
        m_ExitState = EXIT_QUIT;

    // Process delayed state change (BEFORE we update the states, so that
    // update gets called before render...)
    if (m_QueuedState)
    {
        // Just in case we immediately queue up another state...
        CIwGameState* pNewState = m_QueuedState;
        m_QueuedState = NULL;
        PushState(pNewState);
    }

    // Update clock scheduler... calls UpdateTimer() and makes calls to UpdateTick() / UpdateControls()
    if (IwGetGameClock()->m_MillisecondsPerLogicTick)
        IwGetGameClock()->Update();
    else
    {
        UpdateControls();
        DebugUpdate();
        UpdateTick();
    }
}
