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
#include "InGameState.h"

#include "IwGxPrint.h"
#include "IwEnt.h"
#include "BestLine.h"
#include "Car.h"
#include "Game.h"
#include "World.h"
#include "PauseState.h"
#include "EndGameState.h"
#include "IwResManager.h"
#include "IwSound.h"

IW_MANAGED_IMPLEMENT(CInGameState);

//-------------------------------------------------------------------------------------------------

CInGameState::CInGameState()
:   m_LastTime( 0 ),
    m_pLapMaterial( NULL ),
    m_pPosMaterial( NULL ),
    m_LastDigit(-1)
{
    // Cache pointers to materials - IwResManager search functions can be expensive if there are many resources,
    // So it's good practise to keep pointers around for anything that's used in 'realtime' portions of the application.
    m_pLapMaterial = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("lap", IW_GX_RESTYPE_MATERIAL);
    m_pPosMaterial = (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed("pos", IW_GX_RESTYPE_MATERIAL);
    m_pCountdown = (CIwSoundSpec*)IwGetResManager()->GetResNamed("countdown", "CIwSoundSpec");
    m_pGo = (CIwSoundSpec*)IwGetResManager()->GetResNamed("go", "CIwSoundSpec");

    static const char* positionString[4] = { "pos1_4", "pos2_4", "pos3_4", "pos4_4" };

    uint32 currentMaterial = 0;

    // Retrieve position materials
    for (; currentMaterial < NUM_POSITIONS; ++currentMaterial)
    {
        m_pPositionMaterial[ currentMaterial ] =    (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed(
                                                        positionString[currentMaterial],
                                                        IW_GX_RESTYPE_MATERIAL
                                                    );
    }

    static const char* countDownMarker[] =
    {
        "countdownmarkergo",
        "countdownmarker1",
        "countdownmarker2",
        "countdownmarker3",
    };

    for (currentMaterial = 0; currentMaterial < NUM_COUNTDOWN_SIGNS; ++currentMaterial)
    {
        m_pCountdownMaterial[ currentMaterial ] =   (CIwMaterial *)IwGetResManager()->GetGroupNamed("ui")->GetResNamed(
                                                        countDownMarker[currentMaterial],
                                                        IW_GX_RESTYPE_MATERIAL
                                                    );
    }

#ifdef IW_DEBUG
    IwGetResManager()->DumpCatalogue("Resource.log");
#endif // IW_DEBUG
}

//-------------------------------------------------------------------------------------------------
void    CInGameState::EnterTop()
{
    CBaseState::EnterTop();

    //Only make the accelerometer active during gameplay to preserve batteries
    s3eAccelerometerStart();
}

//-------------------------------------------------------------------------------------------------
void    CInGameState::ExitTop()
{
    CBaseState::ExitTop();

    s3eAccelerometerStop();
}


//-------------------------------------------------------------------------------------------------

void CInGameState::Update()
{
    IW_CALLSTACK("CInGameState::Update");

    CBaseState::Update();

    if (s3eKeyboardGetState(s3eKey1) & S3E_KEY_STATE_PRESSED )
        GetWorld().ToggleInCarCamera();

    // pause
    if (IsSoftKeyPressed(BACK) || s3eDeviceCheckPauseRequest() )
    {
        CTempBucketSwitch b(MB_STATES);
        GetGame().QueueState( new CPauseState );
    }

    GetWorld().Update();

    CCar* car = GetWorld().GetUICar();
    int32 lap = 0;
    if (GetWorld().GetBestLine() )
    {
        lap = car->GetTracker().GetDistance(car->GetEnt()->GetPosLocal()) / GetWorld().GetBestLine()->GetLength();
    }

    if (lap > 3)
    {
        CIwSoundGroup* gamesfx = (CIwSoundGroup*)IwGetResManager()->GetResNamed("gamesfx", "CIwSoundGroup");
        if (gamesfx)
            gamesfx->Pause();
        SetFlags(DELETE_WHEN_NOT_TOP_F);
        CTempBucketSwitch b(MB_STATES);
        GetGame().QueueState(new CEndGameState);
    }
}

//-------------------------------------------------------------------------------------------------

void CInGameState::Render()
{
    IW_CALLSTACK("CInGameState::Render");

    //keep the backlight on during gameplay
    s3eDeviceBacklightOn();

    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

    GetWorld().Render();

    IwGxLightingOff();

    CCar* car = GetWorld().GetUICar();
    int32 lap = 0;
    if (GetWorld().GetBestLine() )
    {
        lap = car->GetTracker().GetDistance(car->GetEnt()->GetPosLocal()) / GetWorld().GetBestLine()->GetLength();
    }

    RenderSoftKeys(NULL, "Pause");

    if (GetWorld().GetCountDownTicks() )
    {
        int32 count =  GetWorld().GetCountDownTicks() / FPS;

        if (count != m_LastDigit)
        {
            m_LastDigit = count;
            if (count == 0)
                m_pGo->Play();
            else
                m_pCountdown->Play();
        }

        IwGxSetScreenSpaceOrg( &CIwSVec2::g_Zero );

        CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
        memset(cols, 255, sizeof(CIwColour) * 4 );

        IwGxSetMaterial( m_pCountdownMaterial[ count ] );

        const uint32 imageWidth[4] = { 64, 32, 32, 32 };
        const uint32 imageHeight    = 32;

        int32 width     = IwGxGetScreenWidth();
        int32 height    = IwGxGetScreenHeight();

        CIwSVec2 XY( (int16)((width/2) - (imageWidth[count]/2)) , (int16)((height/2) - (imageHeight/2)) ),
                dXY( (int16)imageWidth[count], (int16)imageHeight );

        IwGxDrawRectScreenSpace(&XY, &dXY, cols);
    }
    else
    {
        const uint32 imageSize = 32;

        CIwColour* cols = IW_GX_ALLOC(CIwColour, 4);
        memset(cols, 255, sizeof(CIwColour) * 4 );

        // Draw position image
        CIwSVec2 XY( (int16)(IwGxGetScreenWidth() - 70), 0 );
        CIwSVec2 dXY(imageSize, imageSize);

        IwGxSetMaterial( m_pPosMaterial );
        IwGxDrawRectScreenSpace(&XY, &dXY, cols);

        // Draw position number
        XY = CIwSVec2((int16)(IwGxGetScreenWidth() - 35), 0);
        dXY = CIwSVec2(imageSize, imageSize);

        IwGxSetMaterial( m_pPositionMaterial[ car->GetPosition()-1 ] );
        IwGxDrawRectScreenSpace(&XY, &dXY, cols);

        // Draw lap image
        XY = CIwSVec2(0, 0);
        dXY = CIwSVec2(imageSize, imageSize);

        IwGxSetMaterial( m_pLapMaterial );
        IwGxDrawRectScreenSpace(&XY, &dXY, cols);

        // Draw lap number
        XY = CIwSVec2(35, 0);
        dXY = CIwSVec2(imageSize, imageSize);

        IwGxSetMaterial(m_pPositionMaterial[lap]);
        IwGxDrawRectScreenSpace(&XY, &dXY, cols);
    }

    int64 now = s3eTimerGetMs();
    m_LastTime = now;

    IwGxLightingOn();

    {
        IW_PROFILE("GxFlush");
        IwGxFlush();
    }

    // Render the menu in a separate flush so GxMetrics reflect this frame when they are rendered
    IwGetMenuManager()->Render();
    {
        IW_PROFILE("GxFlush2");
        IwGxFlush();
    }

#ifdef IW_DEBUG
    // Reset metrics for next frame
    IwGxMetricsReset();
#endif

    {
        IW_PROFILE("GxSwapBuffers");
        IwGxSwapBuffers();
    }
}
