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
#include "EndGameState.h"

#include "IwGxPrint.h"
#include "Car.h"
#include "World.h"
#include "IwResManager.h"
#include "Game.h"


// minimum time (ms) I3D logo is displayed for
#define I3D_RACE_FINISH_TIMEOUT 3000



IW_MANAGED_IMPLEMENT(CEndGameState);

CEndGameState::CEndGameState()
{
    WriteFlags(DELETE_WHEN_NOT_TOP_F);

    m_ResultTimer.SetTimeMS( I3D_RACE_FINISH_TIMEOUT );
}

void CEndGameState::Update()
{
    IW_CALLSTACK("CEndGameState::Update");

    CBaseState::Update();

    PopOnBack();

    if (m_ResultTimer.Tick() )
    {
        GetGame().QueuePop();
    }

    m_ResultTimer.Update();
}


void CEndGameState::Render()
{
    IW_CALLSTACK("CEndGameState::Render");

    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

    GetWorld().Render();

    IwGxLightingOff();
    IwGxSetScreenSpaceOrg( &CIwSVec2::g_Zero );

    CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
    memset(cols, 255, sizeof(CIwColour) * 4 );

    if (GetWorld().GetUICar()->GetPosition() == 1 )
    {
        CIwMaterial* mat = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("youwin", IW_GX_RESTYPE_MATERIAL);
        IwGxSetMaterial(mat);
    }
    else
    {
        CIwMaterial* mat = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("youlose", IW_GX_RESTYPE_MATERIAL);
        IwGxSetMaterial(mat);
    }


    const uint32 imageWidth  = 128;
    const uint32 imageHeight = 32;

    CIwSVec2 XY( (int16)((IwGxGetScreenWidth()/2) - (imageWidth/2)), (int16)(IwGxGetScreenHeight()/4) ),
            dXY( (int16)imageWidth, (int16)imageHeight );

    IwGxDrawRectScreenSpace(&XY, &dXY, cols);
    IwGxLightingOn();

    IwGxFlush();
    IwGxSwapBuffers();

#ifdef IW_DEBUG
    // Reset metrics for next frame
    IwGxMetricsReset();
#endif
}

void CEndGameState::ExitTop()
{
}
