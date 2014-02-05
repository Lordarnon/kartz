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
#include "InCarCamera.h"

#include "IwEnt.h"
#include "World.h"

#define TRACK_DISTANCE (IW_FIXED(0.00))
#define TRACK_SPEED (IW_FIXED(0.4))

CInCarCamera::CInCarCamera(CIwEnt* target)
: m_Target(target)
{

}

void CInCarCamera::Update()
{
    //the in car camera is attached directly to the target at an appropriate offset...
    CIwVec3 targetPos = m_Target->GetPosWorld();

    CIwMat mat;
    mat = *m_Target->GetFrame()->GetMatLocal();

    targetPos += mat.RowZ() * IW_FIXED_MUL(IW_FIXED(1.0), WORLD_SCALE);
    targetPos += mat.RowY() * IW_FIXED_MUL(IW_FIXED(30.0), WORLD_SCALE);

    //+y is down the screen for the camera, so rotate this matrix 180 degrees around z
    mat.PreRotateZ(IW_FIXED_MUL(IW_ANGLE_2PI, IW_FIXED(0.5)));
    mat.t = targetPos;

    m_Ent->GetFrame()->SetMatLocal(mat);
}
