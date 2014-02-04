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
#include "RacingDemo.h"
#include "FrontendState.h"
#include "LoadState.h"
#include "Game.h"
#include "IwGxPrint.h"
#include "IwResManager.h"
#include "IwMaterial.h"
#include "IwSound.h"
#include "World.h"
#include "s3eBackgroundMusic.h"
#include "s3eDevice.h"

// minimum time (ms) I3D logo is displayed for
#define I3D_LOGO_TIMEOUT    1500

int32 HandleResume(void* sysData, void* userData)
{
    // To pass Microsoft submission requirenments app have to satisfy rules
    // for background music management.
    // On Windows Phone 8 application should prompt to pause background music.
    if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_WP8 &&
        s3eBackgroundMusicAvailable() &&
        s3eBackgroundMusicGetInt(S3E_BACKGROUNDMUSIC_PLAYBACK_STATE) != S3E_BACKGROUNDMUSIC_PLAYBACK_PLAYING)
    {
        CFrontendState* s = static_cast<CFrontendState*>(userData);
        s->PlayMusic();
    }

    return 0;
}

int32 HandleSuspend(void* sysData, void* userData)
{
    // To pass Microsoft submission requirenments app have to satisfy rules
    // for background music management.
    // On Windows Phone 8 application should prompt to pause background music.
    if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_WP8 &&
        s3eBackgroundMusicAvailable())
    {
        CFrontendState* s = static_cast<CFrontendState*>(userData);
        s->StopMusic();
    }

    return 0;
}

IW_MANAGED_IMPLEMENT(CFrontendState);

CFrontendState::CFrontendState() :
    m_Music(0)
{
    IW_CALLSTACK("CFrontendState::CFrontendState");

    WriteFlags(DELETE_WHEN_POPPED_F);
}

void CFrontendState::EnterTop()
{
    IW_CALLSTACK("CFrontendState::EnterTop");

    CBaseState::EnterTop();

    // To pass Microsoft submission requirenments app have to satisfy rules
    // for background music management.
    // On Windows Phone 8 application should prompt to pause background music.
    // Handle suspend/resume on Windows Phone 8 to pause/resume
    // music depending on background music playback state
    if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_WP8)
    {
        s3eDeviceRegister(S3E_DEVICE_PAUSE, HandleSuspend, this);
        s3eDeviceRegister(S3E_DEVICE_UNPAUSE, HandleResume, this);
    }

    if (s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WP8 ||
        (s3eBackgroundMusicAvailable() &&
        s3eBackgroundMusicGetInt(S3E_BACKGROUNDMUSIC_PLAYBACK_STATE) != S3E_BACKGROUNDMUSIC_PLAYBACK_PLAYING))
    {
        PlayMusic();
    }
}

void CFrontendState::ExitTop()
{
    CBaseState::ExitTop();

    // To pass Microsoft submission requirenments app have to satisfy rules
    // for background music management.
    // On Windows Phone 8 application should prompt to pause background music.
    if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_WP8)
    {
        s3eDeviceUnRegister(S3E_DEVICE_PAUSE, HandleSuspend);
        s3eDeviceUnRegister(S3E_DEVICE_UNPAUSE, HandleResume);
    }
}

void CFrontendState::PlayMusic()
{
    if (!m_Music)
        m_Music = (CIwSoundSpec*)IwGetResManager()->GetResNamed("kartingmusic", "CIwSoundSpec");
    if (m_Music)
        m_Music->Play();
}

void CFrontendState::StopMusic()
{
    if (m_Music)
    {
        IwGetSoundManager()->StopSoundSpec(m_Music);
    }
}

void CFrontendState::Update()
{
    IW_CALLSTACK("CFrontendState::Update");

    CBaseState::Update();

    PopOnBack();
    if (IsSoftKeyPressed(FORWARDS) )
    {
        CTempBucketSwitch b(MB_STATES);
        GetGame().PushState(new CLoadState);
    }

    // Ignore any pause requests during the frontend
    s3eDeviceCheckPauseRequest();
}


void CFrontendState::Render()
{
    IW_CALLSTACK("CFrontendState::Render");

    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
    IwGxSetScreenSpaceOrg(&CIwSVec2::g_Zero);
    IwGxLightingOff();

    CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
    memset(cols, 255, sizeof(CIwColour) * 4 );

    CIwMaterial* mat = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("startsplash", IW_GX_RESTYPE_MATERIAL);
    IwGxSetMaterial(mat);

    CIwSVec2 XY(0, 0), dXY( (int16)IwGxGetScreenWidth(), (int16)IwGxGetScreenHeight() );
    IwGxDrawRectScreenSpace(&XY, &dXY, cols);

    RenderSoftKeys("start", "quit");

    IwGxLightingOn();
    IwGxFlush();
    IwGxSwapBuffers();
#ifdef IW_DEBUG
    // Reset metrics for next frame
    IwGxMetricsReset();
#endif
}
