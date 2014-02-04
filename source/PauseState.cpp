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
#include "PauseState.h"

#include "Game.h"
#include "IwGxPrint.h"
#include "IwResManager.h"
#include "IwMaterial.h"
#include "World.h"
#include "IwSound.h"


IW_MANAGED_IMPLEMENT(CPauseState);

CPauseState::CPauseState()
{
    IW_CALLSTACK("CPauseState::CPauseState");

    CIwSoundGroup* gamesfx = (CIwSoundGroup*)IwGetResManager()->GetResNamed("gamesfx", "CIwSoundGroup");
    if (gamesfx)
        gamesfx->Pause();

    WriteFlags(DELETE_WHEN_POPPED_F);
}


void CPauseState::Update()
{
    IW_CALLSTACK("CPauseState::Update");

    CBaseState::Update();

    if (IsSoftKeyPressed(BACK) )
    {
        // We want to go back to the frontend, so we need to delete the in game state
        // that's under us on the stack before we pop.
        CIwGameState* pInGameState = GetGame().GetUnderState();
        GetGame().RemoveState(pInGameState);
        delete pInGameState;
        GetGame().QueuePop();
    }
    else if (IsSoftKeyPressed(FORWARDS) )
    {
        GetGame().QueuePop();
        CIwSoundGroup* gamesfx = (CIwSoundGroup*)IwGetResManager()->GetResNamed("gamesfx", "CIwSoundGroup");
        if (gamesfx)
            gamesfx->Resume();
    }
}


void CPauseState::Render()
{
    IW_CALLSTACK("CPauseState::Render");

    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

    GetWorld().Render();

    IwGxSetScreenSpaceOrg(&CIwSVec2::g_Zero);
    IwGxLightingOff();

    CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
    memset(cols, 255, sizeof(CIwColour) * 4 );

    CIwMaterial* mat = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("pause", IW_GX_RESTYPE_MATERIAL);
    IwGxSetMaterial(mat);

    const int32 width   = IwGxGetScreenWidth();
    const int32 height  = IwGxGetScreenHeight();

    const int16 imageWidth  = 64;
    const int16 imageHeight = 32;

    CIwSVec2 XY( (int16)((width/2) - (imageWidth/2)) , (int16)((height/2) - (imageHeight/2)) ),
            dXY( (int16)imageWidth, (int16)imageHeight );
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
