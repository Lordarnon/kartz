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
 * Motion behaviour: follow path
 */

#include "IwMotionPath.h"
#include "IwEnt.h"
#include "IwGeomQuat.h"
#include "IwInterpolation.h"
#include "IwShapePath.h"
#include "IwWorld.h"

// Debug rendering
#ifdef IW_DEBUG_RENDER
#define DEBUG_RENDER
#endif

//-----------------------------------------------------------------------------
// CIwBeMotionPath
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwBeMotionPath)
CIwBeMotionPath::CIwBeMotionPath()
{
    IW_CALLSTACK("CIwBeMotionPath::CIwBeMotionPath")

    m_Path  = NULL;
    m_Dist  = 0;
    m_Speed = 0;
    m_Acc   = 0;
    m_Flags = UPDATE_ENT_POS_F | UPDATE_ENT_ROT_F;
}
//-----------------------------------------------------------------------------
void    CIwBeMotionPath::Update()
{
    IW_CALLSTACK("CIwBeMotionPath::Update")

    // If m_Path has been cleared - assume motion behaviour has been dropped
    if (!m_Path)
    {
        SetActive(false);
        return;
    }

    // Move between subpoints at specified speed
    m_Speed += m_Acc;
    m_Dist  += m_Speed >> 16;
    m_Path->CalcPointAtDist(m_Dist, &m_LastPos, &m_LastTangent, NULL);

    IwAssert(ENGINE, m_Ent);
    CIwMat* pMat = m_Ent->ExposeMatLocal();
    IwAssert(ENGINE, pMat);

    // Set ent pos
    if (m_Flags & UPDATE_ENT_POS_F)
        pMat->t = m_LastPos;

    // Set ent rotation
    if (m_Flags & UPDATE_ENT_ROT_F)
    {
        int32   ay  = IW_GEOM_ATAN2(m_LastTangent.x, m_LastTangent.z);
        int32   xz  = IW_GEOM_SQRT2(SQR(m_LastTangent.x) + SQR(m_LastTangent.z)) >> IW_GEOM_POINT;
        int32   ax  = -IW_GEOM_ATAN2(m_LastTangent.y, xz);
        IwInterpBindAnglePhase(ax);

        CIwQuat qx, qy;
        qy.SetAxisAngle(0, IW_GEOM_QONE, 0, ay << 18);
        qx.SetAxisAngle(IW_GEOM_QONE, 0, 0, ax << 18);
        pMat->CopyRot(qy * qx);
//      pMat->LookAt(m_LastPos, m_LastPos + m_LastTangent, CIwVec3::g_AxisY);
    }

    if (m_Dist >= (int32)m_Path->GetLength())
    {
        if (m_Flags & OFF_AT_END_F)
        {
            // Turn OFF update and clear path ptr
            SetActive(false);
            m_Path = NULL;

            // Turn ON main update behaviour
            m_Ent->GetBehaviour()->SetActive(true);
        }
/*      else
        if (m_Flags & NEXT_AT_END_F)
        {
            m_Path  = ((CPathSpline*)m_Path)->m_PathNext[0];
            m_Dist  = 0;
        }*/
    }
}
//-----------------------------------------------------------------------------
void    CIwBeMotionPath::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwBeMotionPath::DebugAddMenuItems")

#ifdef IW_DEBUG
CIwBeMotion::DebugAddMenuItems(pMenu);
    pMenu->AddItem(new CIwMenuItemEditInt32("Speed 1.15.16", &m_Speed, 1 << 12));
    pMenu->AddItem(new CIwMenuItemEditInt32("Acc 1.15:16", &m_Acc, 1 << 12));
#endif
}
