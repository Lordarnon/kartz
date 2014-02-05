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
#ifndef INGAMESTATE_H
#define INGAMESTATE_H

#include "BaseState.h"

class CInGameState : public CBaseState
{
public:
    IW_MANAGED_DECLARE(CInGameState);

    CInGameState();

    /** @copydoc CBaseState::EnterTop() */
    virtual void    EnterTop();

    /** @copydoc CBaseState::ExitTop() */
    virtual void    ExitTop();

    /** @copydoc CBaseState::Update() */
    virtual void Update();

    /** @copydoc CBaseState::Render() */
    virtual void Render();

private:

    enum
    {
        NUM_POSITIONS       = 4,
        NUM_COUNTDOWN_SIGNS = 4,
    };

    int64 m_LastTime;

    CIwMaterial*    m_pPosMaterial;
    CIwMaterial*    m_pLapMaterial;
    CIwMaterial*    m_pPositionMaterial[NUM_POSITIONS];
    CIwMaterial*    m_pCountdownMaterial[NUM_COUNTDOWN_SIGNS];

    int m_LastDigit;
    CIwSoundSpec*   m_pCountdown;
    CIwSoundSpec*   m_pGo;
};

#endif /* !INGAMESTATE_H */
