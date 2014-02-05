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
/*
 *Camera behaviour: free-cam
 */

#include "IwMotionCamFree.h"
#include "IwCamera.h"
#include "s3eKeyboard.h"
#include "IwGeomQuat.h"

/** Half cam speed */
#define HALF_SPEED

//-----------------------------------------------------------------------------
// CIwBeMotionCamFree
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwBeMotionCamFree)
CIwBeMotionCamFree::CIwBeMotionCamFree()
{
    IW_CALLSTACK("CIwBeMotionCamFree::CIwBeMotionCamFree")

    m_AngVel.x  = 0;
    m_AngVel.y  = 0;
    m_Vel.y     = 0;
    m_Vel.z     = 0;

    m_MaxAngVel = 60;
    m_MaxSpeed  = 250;

    m_UseWorldY = false;
}
//-----------------------------------------------------------------------------
void    CIwBeMotionCamFree::Init()
{
    IW_CALLSTACK("CIwBeMotionCamFree::Init")

    IwAssert(ENGINE, GetEnt());
    CIwCamera*  pCam    = (CIwCamera*)GetEnt();
    pCam->ExposeMatLocal();
}
//-----------------------------------------------------------------------------
void    CIwBeMotionCamFree::Update()
{
    IW_CALLSTACK("CIwBeMotionCamFree::Update")

    CIwCamera*  pCam = (CIwCamera*)GetEnt();
    CIwMat*     pMat = pCam->ExposeMatLocal();

#if defined I3D_OS_WINDOWS || defined I3D_OS_S3E
    const s3eKey    kLeft           = s3eKeyLeft;
    const s3eKey    kRight          = s3eKeyRight;
    const s3eKey    kUp             = s3eKeyUp;
    const s3eKey    kDown           = s3eKeyDown;
    const s3eKey    kZRotUp         = s3eKeyNumPadPlus;
    const s3eKey    kZRotDown       = s3eKeyNumPadMinus;

    const s3eKey    kFore           = s3eKeyNumPad4;
    const s3eKey    kBack           = s3eKeyNumPad1;
    const s3eKey    kRise           = s3eKeyNumPad2;
    const s3eKey    kFall           = s3eKeyNumPad5;

    const s3eKey    kStrafeLeft     = s3eKeyNumPad7;
    const s3eKey    kStrafeRight    = s3eKeyNumPad8;

    const s3eKey    kFast           = s3eKeyShift;
#else
    const s3eKey    kLeft           = s3eKey4;
    const s3eKey    kRight          = s3eKey6;
    const s3eKey    kUp             = s3eKey2;
    const s3eKey    kDown           = s3eKey8;
    const s3eKey    kZRotUp         = s3eKeyLSK;
    const s3eKey    kZRotDown       = s3eKeyRSK;

    const s3eKey    kFore           = s3eKey1;
    const s3eKey    kBack           = s3eKey7;
    const s3eKey    kRise           = s3eKey3;
    const s3eKey    kFall           = s3eKey9;

    const s3eKey    kStrafeLeft     = s3eKey0;
    const s3eKey    kStrafeRight    = s3eKeyHash;

    const s3eKey    kFast           = s3eKeyStar;
#endif

    bool    shift       = (s3eKeyboardGetState(kFast) & S3E_KEY_STATE_DOWN) != 0;
    int32   maxAngVel   = shift ? m_MaxAngVel : m_MaxAngVel/2;
    int32   maxSpeed    = shift ? m_MaxSpeed : m_MaxSpeed/4;
    int32   angAcc      = maxAngVel >> 3;
    int32   acc         = maxSpeed >> 1;
#ifdef HALF_SPEED
    maxAngVel >>= 1;
    maxSpeed >>= 1;
#endif

    // Rotate Y
    if (s3eKeyboardGetState(kLeft) & S3E_KEY_STATE_DOWN)
        m_AngVel.y  = MAX(-maxAngVel, m_AngVel.y + angAcc);
    else
    if (s3eKeyboardGetState(kRight) & S3E_KEY_STATE_DOWN)
        m_AngVel.y  = MIN(maxAngVel, m_AngVel.y - angAcc);
    else
        m_AngVel.y  = 0;

    // Rotate X
    if (s3eKeyboardGetState(kUp) & S3E_KEY_STATE_DOWN)
        m_AngVel.x  = MAX(-maxAngVel, m_AngVel.x - angAcc);
    else
    if (s3eKeyboardGetState(kDown) & S3E_KEY_STATE_DOWN)
        m_AngVel.x  = MIN(maxAngVel, m_AngVel.x + angAcc);
    else
        m_AngVel.x  = 0;

    m_AngVel.z  = 0;
    if ((s3eKeyboardGetState(s3eKeyV) & S3E_KEY_STATE_DOWN))
    {
        // Rotate Z
        if (s3eKeyboardGetState(kZRotUp) & S3E_KEY_STATE_DOWN)
            m_AngVel.z  = MAX(-maxAngVel, m_AngVel.z - angAcc);
        else
        if (s3eKeyboardGetState(kZRotDown) & S3E_KEY_STATE_DOWN)
            m_AngVel.z  = MIN(maxAngVel, m_AngVel.z + angAcc);
        else
            m_AngVel.z  = 0;
    }

    if (m_AngVel.x || m_AngVel.y || m_AngVel.z)
    {
        // Back up translation of matrix, and zero it so that the
        // rotations don't change the current position of the camera
        const CIwVec3 origTranslation = pMat->t;
        pMat->t = CIwVec3::g_Zero;

        // Apply required rotation
        CIwMat  mat;

        if (m_AngVel.x)
        {
            mat.SetAxisAngle(pMat->VecX(), m_AngVel.x);
            *pMat *= mat;
        }
        if (m_AngVel.y)
        {
            if (m_UseWorldY)
                mat.SetRotY(-m_AngVel.y);
            else
                mat.SetAxisAngle(pMat->VecY(), m_AngVel.y);
            *pMat *= mat;
        }
        if (m_AngVel.z)
        {
            mat.SetAxisAngle(pMat->VecZ(), m_AngVel.z);
            *pMat *= mat;
        }

        // Normalise matrix and copy back the original position
        pMat->Normalise();
        pMat->t = origTranslation;
    }

    // Move forward/back
    if (s3eKeyboardGetState(kFore) & S3E_KEY_STATE_DOWN)
        m_Vel.z = MIN(maxSpeed, m_Vel.z + acc);
    else
    if (s3eKeyboardGetState(kBack) & S3E_KEY_STATE_DOWN)
        m_Vel.z = MAX(-maxSpeed, m_Vel.z - acc);
    else
        m_Vel.z = 0;

    pMat->t += pMat->VecZ() * m_Vel.z;

    // Move up/down
    if (s3eKeyboardGetState(kRise) & S3E_KEY_STATE_DOWN)
        m_Vel.y = MIN(maxSpeed, m_Vel.y + acc);
    else
    if (s3eKeyboardGetState(kFall) & S3E_KEY_STATE_DOWN)
        m_Vel.y = MAX(-maxSpeed, m_Vel.y - acc);
    else
        m_Vel.y = 0;

    pMat->t += pMat->VecY() * m_Vel.y;

    // Strafe left/right
    if (s3eKeyboardGetState(kStrafeRight) & S3E_KEY_STATE_DOWN)
        m_Vel.x = MIN(maxSpeed, m_Vel.x + acc);
    else
    if (s3eKeyboardGetState(kStrafeLeft) & S3E_KEY_STATE_DOWN)
        m_Vel.x = MAX(-maxSpeed, m_Vel.x - acc);
    else
        m_Vel.x = 0;

    pMat->t += pMat->VecX() * m_Vel.x;

    // Change perspmul
    if (s3eKeyboardGetState(s3eKeyV) & S3E_KEY_STATE_DOWN)
    {
        if (s3eKeyboardGetState(s3eKeyNumPadPlus) & S3E_KEY_STATE_DOWN)     pCam->m_PerspMul += 0x8;
        if (s3eKeyboardGetState(s3eKeyNumPadMinus) & S3E_KEY_STATE_DOWN)    pCam->m_PerspMul = MAX(0x8, pCam->m_PerspMul - 0x8);
    }
}


void    CIwBeMotionCamFree::SetUseWorldY()
{
    m_UseWorldY = true;
}
