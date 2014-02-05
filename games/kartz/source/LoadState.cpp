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
#include "LoadState.h"

#include "IwGxPrint.h"

#include "Game.h"
#include "InGameState.h"
#include "World.h"

IW_MANAGED_IMPLEMENT(CLoadState);

bool CLoadState::m_Loaded = false;
CLoadState* g_pLoadState = NULL;

CLoadState::CLoadState()
{
    IW_CALLSTACK("CLoadState::CLoadState");

    // The load state wants to disappear immediately when it pushes the in game state,
    // so the stack goes from *Frontend->LoadState* to *Frontend->InGameState*
    WriteFlags(DELETE_WHEN_NOT_TOP_F);
}


void SerialiseCallback()
{
    g_pLoadState->Render();
    s3eDeviceYield(0);
}

void CLoadState::Update()
{
    IW_CALLSTACK("CLoadState::Update");

    CBaseState::Update();

    PopOnBack();

    Render();

    // Setup loading callbacks. We need this to periodically render the screen during load
    g_pLoadState = this;
    IwSerialiseSetCallbackPeriod(10000);
    IwSerialiseSetCallback(SerialiseCallback);

    if (!m_Loaded)
    {
        CTempBucketSwitch b(MB_TRACK);

        CreateWorld();

        m_Loaded = true;
    }
    else
    {
        GetWorld().Reset();
    }

    {
        CTempBucketSwitch b(MB_STATES);
        GetGame().QueueState(new CInGameState);
    }

    // Clear the callback - it should never be called once this state has been destroyed
    IwSerialiseSetCallback(NULL);

    // Ignore any key presses during loading.
    s3eKeyboardClearState();

    // Ignore any pause requests during loading
    s3eDeviceCheckPauseRequest();
}


void CLoadState::Render()
{
    IW_CALLSTACK("CLoadState::Render");

    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

    int32 width = IwGxGetScreenWidth();
    int32 height = IwGxGetScreenHeight();
    IwGxPrintString(width/2-20, height/2-5, "Loading...");

    IwGxFlush();
    IwGxSwapBuffers();
#ifdef IW_DEBUG
    // Reset metrics for next frame
    IwGxMetricsReset();
#endif
}
