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
#include "BaseState.h"
#include "Game.h"
#include "IwSound.h"
#include "IwResManager.h"
#include "IwMaterial.h"

IW_MANAGED_IMPLEMENT(CBaseState);

CBaseState::CBaseState()
{
    IW_CALLSTACK("CBaseState::CBaseState");

    WriteFlags(DELETE_WHEN_POPPED_F);

    m_SelectSFX = (CIwSoundSpec*)IwGetResManager()->GetResNamed("ui_select", "CIwSoundSpec", IW_RES_PERMIT_NULL_F);
}


void CBaseState::Update()
{
    IW_CALLSTACK("CBaseState::Update");

    if (GetGame().m_ExitState == CIwGame::EXIT_QUIT )
    {
        GetGame().QueuePop();
    }
}


void CBaseState::PopOnBack()
{
    IW_CALLSTACK("CBaseState::PopOnBack");

    if (IsSoftKeyPressed(BACK) )
    {
        GetGame().QueuePop();
    }
}


bool CBaseState::IsSoftKeyPressed(CSoftKey key)
{
    bool pressed = false;
    // check key states
    if (key == BACK && s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_PRESSED )
        pressed = true;
    if (key == FORWARDS && s3eKeyboardGetState(s3eKeyAbsASK) & S3E_KEY_STATE_PRESSED )
        pressed = true;

    if (!pressed && s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED )
    {
        int32 x = s3ePointerGetX();
        int32 y = s3ePointerGetY();

        CIwRect forwardsRect, backRect;
        GetSoftKeyRects(forwardsRect, backRect);

        if (key == BACK
            && x >= backRect.x && x <= backRect.x + backRect.w
            && y >= backRect.y && y <= backRect.y + backRect.h )
            pressed = true;

        if (key == FORWARDS
            && x >= forwardsRect.x && x <= forwardsRect.x + forwardsRect.w
            && y >= forwardsRect.y && y <= forwardsRect.y + forwardsRect.h)
            pressed = true;
    }

    if (pressed)
        m_SelectSFX->Play();

    return pressed;
}


static void DrawSoftKey(const char* name, CIwRect& rect)
{
    if (!name)
        return;

    IwGxLightingOff();
    CIwMaterial* mat = (CIwMaterial*)IwGetResManager()->GetGroupNamed("ui")->GetResNamed(name, IW_GX_RESTYPE_MATERIAL);
    IwGxSetMaterial(mat);

    //We set the material to be unmodulated. This makes the softkey icons appear full bright.
    mat->SetModulateMode(CIwMaterial::MODULATE_NONE);

    CIwSVec2 pos(rect.x, rect.y);
    CIwSVec2 size(rect.w, rect.h);
    IwGxDrawRectScreenSpace(&pos, &size);
}

void CBaseState::RenderSoftKeys(const char* forwards, const char* back)
{
    CIwRect forwardsRect, backRect;
    GetSoftKeyRects(forwardsRect, backRect);

    DrawSoftKey(back, backRect);
    DrawSoftKey(forwards, forwardsRect);
}


void CBaseState::GetSoftKeyRects(CIwRect& forwards, CIwRect& back)
{
    const int16 imageWidth  = 64;
    const int16 imageHeight = 32;

    forwards.x = 0;
    back.x = (int16)IwGxGetScreenWidth() - imageWidth;

    if (s3eDeviceGetInt( S3E_DEVICE_LEFT_SOFTKEY_IS_BACK) )
    {
        int16 temp  = back.x;
        back.x  = forwards.x;
        forwards.x = temp;
    }

    back.y = forwards.y = (int16)IwGxGetScreenHeight() - imageHeight;

    back.w = forwards.w = imageWidth;
    back.h = forwards.h = imageHeight;
}
