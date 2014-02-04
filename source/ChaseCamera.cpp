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
#include "ChaseCamera.h"

#include "IwEnt.h"
#include "World.h"

#define TRACK_DISTANCE (IW_FIXED(0.00))
#define TRACK_SPEED (IW_FIXED(0.4))

CChaseCamera::CChaseCamera(CIwEnt* target)
: m_Target(target)
{

}

void CChaseCamera::Update()
{
    // The chase camera chases the target from an arm's length
    CIwVec3 targetPos = m_Target->GetPosWorld();
    CIwVec3 myPos = m_Ent->GetPosLocal();

    // Offset the position of the 'arm' along the target's Y and Z axes
    CIwVec3 armPos = m_Target->GetFrame()->GetMatLocal()->RowY() * IW_FIXED(3);
    armPos += m_Target->GetFrame()->GetMatLocal()->RowZ() * IW_FIXED(-4);

    armPos *= IW_FIXED_MUL(IW_FIXED(15.0), WORLD_SCALE);

    // If we're in the countdown hang back further for a more cinematic view of the racers...
    if (GetWorld().GetCountDownTicks() )
    {
        armPos *= IW_FIXED(2);
    }

    armPos += targetPos;

    // We actually want to look over the car, at the horizon
    targetPos += m_Target->GetFrame()->GetMatLocal()->RowY() * IW_FIXED_MUL(IW_FIXED(40.0), WORLD_SCALE);


    // Move a fraction of the way to the arm's position - this makes the camera 'elastic'
    CIwVec3 toTarget = armPos - myPos;
    myPos += toTarget * TRACK_SPEED;

    // Build and set the matrix
    CIwMat mat;
    mat.LookAt(myPos, targetPos, CIwVec3(0, -IW_GEOM_ONE, 0));
    mat.t = myPos;

    m_Ent->GetFrame()->SetMatLocal(mat);
}
