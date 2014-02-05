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
#ifndef BASESTATE_H
#define BASESTATE_H

class CIwSoundSpec;

#include "IwGameState.h"
#include "IwGame.h"

/*
 * base class for all game states
 */
class CBaseState : public CIwGameState
{
public:
    IW_MANAGED_DECLARE(CBaseState);

    CBaseState();

    virtual void Update();

    // Standard control for pop
    void PopOnBack();

    // Detect softkey presses, (including touch screen)...
    enum CSoftKey { BACK, FORWARDS };
    bool IsSoftKeyPressed(CSoftKey key);

    // Standard soft key render call
    void RenderSoftKeys(const char* forwards, const char* back);

protected:
    void GetSoftKeyRects(CIwRect& back, CIwRect& forwards);
    CIwSoundSpec* m_SelectSFX;
};


//--------------------------------------------------------------------------------
// CTimer
//--------------------------------------------------------------------------------
class CTimer
{
public:
    CTimer() : m_TimeOut(-1), m_Clock(0), m_ClockInterval(0) {}

    inline void SetTimeMS(int32 ms)     { m_TimeOut = GetClock() + ms; }
    inline void Reset()                 { m_TimeOut = -1; }

    // note: after timer expires, successive calls will always return true.
    inline bool Tick()                  { return GetClock() > m_TimeOut; }

    inline int32 GetClock() const       { return IW_FIXED_MUL_SAFE(m_Clock, IW_GEOM_ONE); }

    inline void Update()                {   m_ClockInterval  = IwGetGameClock()->m_MillisecondsPerLogicTick;
                                            m_Clock         += m_ClockInterval;                             }
private:
    //--------------------------------------------------------------------------------
    // Members
    //--------------------------------------------------------------------------------
    int32           m_Clock;                // ms since startup
    int32           m_ClockInterval;        // ms since last update
    int32           m_TimeOut;
};

#endif /* !BASESTATE_H */
