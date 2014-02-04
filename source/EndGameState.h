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
#ifndef ENDGAMESTATE_H
#define ENDGAMESTATE_H

#include "BaseState.h"

class CEndGameState : public CBaseState
{
public:
    IW_MANAGED_DECLARE(CEndGameState);

    CEndGameState();

    /** @copydoc CBaseState::Update() */
    virtual void Update();

    /** @copydoc CBaseState::Render() */
    virtual void Render();

    /** @copydoc CBaseState::ExitTop() */
    virtual void ExitTop();

private:
    CTimer m_ResultTimer; // result screen timer
};

#endif /* !ENDGAMESTATE_H */
