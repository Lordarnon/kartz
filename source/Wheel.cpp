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
#include "Wheel.h"
#include "Car.h"
#include "IwEnt.h"

iwfixed CWheel::s_Radius = 0;

CWheel::CWheel(CCar* theCar, bool isSteerable)
: m_IsSteerable(isSteerable)
, m_Car(theCar)
, m_Roll(0)
{
}

void CWheel::Update()
{
    CIwMat mat = CIwMat::g_Identity;

    // Rotate
    iwfixed spf = GetCar()->GetSpeedPerFrame();
    iwfixed r = GetRadius();

    // here we take advantage of the fact that 2*PI in iwangle is equivalent to IW_GEOM_ONE
    // Thus, if spf == wheel's circumference we rotate 1 revolution.
    iwangle a = -IW_FIXED_DIV(spf, IW_FIXED_MUL(r, IW_FIXED(PI*2)));  // radians rotated this frame
    m_Roll += a;
    mat.PostRotateX(m_Roll);

    // Steer
    if (m_IsSteerable)
    {
        static const int STEER_EXAGGERATE = 4;
        iwfixed steering = STEER_EXAGGERATE * GetCar()->GetSteering();
        mat.PostRotateY(steering);
    }

    GetEnt()->ExposeMatLocal()->CopyRot(mat);
}
