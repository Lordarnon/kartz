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
#include "BestLineTracker.h"
#include "BestLine.h"
#include "World.h"


/** This is the distance ahead to which the next target position is calculated. */
#define LOOK_AHEAD (IW_FIXED_MUL(IW_FIXED(500.0), WORLD_SCALE))


iwfixed CBestLineTracker::m_DistanceStart = 0;

//-----------------------------------------------------------------------------

CBestLineTracker::CBestLineTracker()
:   m_Inited(false),
    m_LastPoint(CIwVec3::g_Zero),
    m_LastDir(CIwVec3::g_Zero),
    m_Point(CIwVec3::g_Zero),
    m_Dir(CIwVec3::g_Zero)
{
}

//-----------------------------------------------------------------------------

void CBestLineTracker::Init(const CIwVec3& position)
{
    SetPoint(0);
    const iwfixed targetDistance = IW_FIXED_MUL(IW_FIXED(500.0), WORLD_SCALE);

    CIwVec3 toPoint;

    while (true)
    {
        toPoint = position - m_Point;
        iwfixed distance = toPoint.GetLength();
        if (distance < targetDistance)
            break;

        SetPoint(m_Distance + LOOK_AHEAD);
        if (m_Distance > GetWorld().GetBestLine()->GetLength() )
            break;
    }

    m_DistanceStart = m_Distance;
    m_Inited        = true;         // This instance is now initialised.
}

//-----------------------------------------------------------------------------

void CBestLineTracker::Update(const CIwVec3& position)
{
    // If there is no best line, just accept the current position as the target.
    if (!GetWorld().GetBestLine() )
    {
        m_Point = position;
    }
    // Initialise if not already done so.
    else if (!m_Inited)
    {
        Init(position);
    }
    // Else perform a standard update
    else
    {
        while (GetDistanceToPoint(position) > -LOOK_AHEAD/4)
        {
            //if we moved past the point, get the next one...
            SetPoint(m_Distance + LOOK_AHEAD);
        }
    }
}

//-----------------------------------------------------------------------------

iwfixed CBestLineTracker::GetDistance(const CIwVec3& position) const
{
    if (!m_Inited)
        return 0;

    return (position - m_Point).Dot(m_LastDir) + m_Distance - m_DistanceStart;
}

//-----------------------------------------------------------------------------

#ifdef IW_DEBUG
void CBestLineTracker::DebugRender()
{
//  if (m_Inited)
//  {
//      IwGxDebugPrimLine(m_Point, m_Point+m_Dir, g_IwGxColours[IW_GX_COLOUR_BLUE] );
//  }
}
#endif

//-----------------------------------------------------------------------------

void CBestLineTracker::SetPoint(iwfixed distance)
{
    m_LastPoint = m_Point;
    m_LastDir   = m_Dir;

    CBestLine* BestLine = GetWorld().GetBestLine();

    m_Distance = distance;

    // Calculate the distance round this lap.
    const iwfixed distanceRoundLap = m_Distance % BestLine->GetLength();

    CIwSVec3 norm;
    BestLine->GetPoint( distanceRoundLap, m_Point, norm );

    //compensate for badly formed curves: ignore suspicious normals...
    if (!m_Inited || m_Dir.Dot(CIwVec3(norm)) > 0 )
        m_Dir = norm;
}

//-----------------------------------------------------------------------------

iwfixed CBestLineTracker::GetDistanceToPoint(const CIwVec3& position) const
{
    CIwVec3 relative = position - m_Point;
    return m_Dir.Dot(relative);
}

//-----------------------------------------------------------------------------
