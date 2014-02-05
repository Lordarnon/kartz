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
#ifndef FRONTENDSTATE_H
#define FRONTENDSTATE_H

#include "BaseState.h"

//--------------------------------------------------------------------------------
// CFrontendState
//--------------------------------------------------------------------------------
class CFrontendState : public CBaseState
{
public:
    IW_MANAGED_DECLARE(CFrontendState);

    CFrontendState();

    virtual void    EnterTop();

    virtual void    ExitTop();

    /** @copydoc CBaseState::Update() */
    virtual void Update();

    /** @copydoc CBaseState::Render() */
    virtual void Render();

private:
    CIwSoundSpec* m_Music;

    void PlayMusic();

    void StopMusic();

    friend int32 HandleSuspend(void* sysData, void* userData);
    friend int32 HandleResume(void* sysData, void* userData);
};

#endif /* !FRONTENDSTATE_H */
