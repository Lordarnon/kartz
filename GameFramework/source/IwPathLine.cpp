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
 * Path: straight lines.
 */

#include "IwPathLine.h"
#include "IwResource.h"
#include "IwTextParserITX.h"
#include <stdio.h>

#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#include "IwRenderManager.h"
#endif

IW_CLASS_FACTORY(CIwPathLine);

//-----------------------------------------------------------------------------
// CIwPathLine
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwPathLine)
CIwPathLine::CIwPathLine()
{
    IW_CALLSTACK("CIwPathLine::CIwPathLine")

    m_Points        = NULL;
    m_NumPoints     = 0;
    m_Segments      = NULL;
    m_NumSegments   = 0;
}
//-----------------------------------------------------------------------------
CIwPathLine::~CIwPathLine()
{
    IW_CALLSTACK("CIwPathLine::~CIwPathLine")

    delete[] m_Points;
    delete[] m_Segments;
}
//-----------------------------------------------------------------------------
void    CIwPathLine::SetNumPoints(uint32 n)
{
    IW_CALLSTACK("CIwPathLine::SetNumPoints")
    IwAssertMsgN(341,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));

    m_NumPoints     = n;
    m_NumSegments   = (m_Flags & CLOSED_F) ? m_NumPoints : (m_NumPoints - 1);

    // Allocate space for segments
    m_Segments  = new Segment[m_NumSegments];

    // Allocate space for points
    uint32  numVecs = m_NumPoints;
    IwAssert(ENGINE, !m_Points);
    if (n)
        m_Points = new CIwVec3[numVecs];
}
//-----------------------------------------------------------------------------
void    CIwPathLine::SetPoint(uint32 id, const CIwVec3& p)
{
    IW_CALLSTACK("CIwPathLine::SetPoint")

    IwAssertMsgN(342,ENGINE, id < m_NumPoints, ("Point ID too large"));
    IwAssertMsgN(343,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));
    m_Points[id] = p;
}
//-----------------------------------------------------------------------------
void    CIwPathLine::Init()
{
    IW_CALLSTACK("CIwPathLine::Init")

    // Only initialise once
    if (TestFlag(INITIALISED_F))
        return;

    IwAssertMsgN(344,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));
    IwAssertMsgN(345,ENGINE, m_Points, ("Path %s has no points", DebugGetName()));
    IwAssertMsgN(346,ENGINE, m_Segments, ("Path %s has no segments", DebugGetName()));

    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    Segment*    pSeg    = m_Segments;
    int32       d       = 0;
    CIwVec3 v0;

    // Calculate segment lengths and entire path length (if not done so already)
    if (m_Length == 0)
    {
        for (uint32 s = 0; s < m_NumSegments; s++)
        {
            // Store path length at start of segment
            pSeg->m_PathLength  = d;

            // Store segment length
            v0 = *pPoint1 - *pPoint0;
            pSeg->m_SegLength = v0.GetLength();

            d += pSeg->m_SegLength;
            pSeg++;
            pPoint0++;
            pPoint1++;
            if (pPoint1 >= &m_Points[m_NumPoints])
                pPoint1 = m_Points;
        }
        // Store entire path length
        m_Length = d;
    }
    // Superclass
    CIwPath::Init();
}
//-----------------------------------------------------------------------------
void    CIwPathLine::CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent, uint32* pSegID)
{
    IW_CALLSTACK("CIwPathLine::CalcPointAtDist")
    IwAssertMsgN(347,ENGINE, IsInitialised(), ("Path %s not initialised", DebugGetName()));

    // Note: this can be slow
    if (!(m_Flags & CLOSED_F))
    {
        // Open path
        if (d >= (int32)m_Length)
        {
//          d = m_Length;
            if (pPoint)
                *pPoint = *GetPoint(m_NumPoints - 1);
            if ((pTangent) && (m_NumPoints > 1))
            {
                *pTangent = *GetPoint(m_NumPoints - 1) - *GetPoint(m_NumPoints - 2);
                pTangent->Normalise();
            }
            if (pSegID)
                *pSegID = m_NumSegments - 1;
            return;
        }
        else
        if (d <= 0.0f)
        {
//          d = 0.0f;
            if (pPoint)
                *pPoint = *GetPoint(0);
            if ((pTangent) && (m_NumPoints > 1))
            {
                *pTangent = *GetPoint(1) - *GetPoint(0);
                pTangent->Normalise();
            }
            if (pSegID)
                *pSegID = 0;
            return;
        }
    }
    else
    {
        // Closed path... get d in range 0..m_Length
        if (d >= (int32)m_Length)
        {
            while (d >= (int32)m_Length)
                d -= m_Length;
        }
        else
        if (d < 0)
        {
            while (d < 0)
                d += m_Length;
        }
    }

    Segment*    pSeg    = m_Segments;
    uint32      s       = m_NumSegments;
    uint32      prevID  = 0;
    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    int32       f;
    while (s--)
    {
        if (d <= (pSeg->m_PathLength + pSeg->m_SegLength))
        {
            // Calculate position on this segment
            f   = ((d - pSeg->m_PathLength) << IW_GEOM_POINT) / pSeg->m_SegLength;

            if (pPoint)
                *pPoint = *pPoint0 + f * (*pPoint1 - *pPoint0);

            if (pTangent)
            {
                CIwVec3 dir = *pPoint1 - *pPoint0;
                dir.Normalise();
                *pTangent = dir;
            }
            if (pSegID)
                *pSegID = m_NumSegments - s;
            return;
        }
        pSeg++;
        pPoint0++;
        pPoint1++;
        prevID++;
        if (pPoint1 >= &m_Points[m_NumPoints])
            pPoint1 = m_Points;
    }
    IwAssertMsgN(348,ENGINE, false, ("d within path length but not within a segment"));
}
//-----------------------------------------------------------------------------
bool    CIwPathLine::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwPathLine::IsPointInside")
    IwAssertMsgN(349,ENGINE, IsInitialised(), ("Path %s not initialised", DebugGetName()));
    IwAssertMsgN(350,ENGINE, m_Flags & CLOSED_F, ("Path %s not closed", DebugGetName()));

    CIwVec3*    pPoint0 = &m_Points[0];
    CIwVec3*    pPoint1 = &m_Points[1];
    CIwVec3     v0, v1, v2;
    uint32      i       = m_NumPoints;
    int32       sign    = 0;

    while (i--)
    {
        if (i == 0)
            pPoint1 = &m_Points[0];

        v0  = *pPoint1 - *pPoint0;
        v1  = p - *pPoint0;

        // Note: check in XZ plane only
        v0.y = v1.y = 0;

        v2 = v0 ^ v1;
        if  (
            ((sign > 0) && (v2.y < 0)) ||
            ((sign < 0) && (v2.y > 0))
            )
            return false;

        sign = v2.y;
        pPoint0++;
        pPoint1++;
    }

    // Check Y
/*  int32 dy = m_Points[0].y - p.y;
    if  (
        (dy > 0) &&
        (dy < m_Height)
        )
        return true;
*/
    return false;
}
//-----------------------------------------------------------------------------
/*uint32    CIwPathLine::CalcDistClosestTo(CIwVec3* pPoint, uint32* pDiff, uint32* pSegID)
{
    IW_CALLSTACK("CIwPathLine::CalcDistClosestTo")
    IwAssertMsgN(351,ENGINE, IsInitialised(), ("Path %s not initialised", DebugGetName()));

    // For each segment - calculate min distance
    Segment*    pSeg    = m_Segments;
    uint32      s       = m_NumSegments;
    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    int32       min     = INT32_MAX;
    int32       f1;
    uint32      d   = 0;
    uint32      sd  = 0;
    CIwSVec3    v0, v1;

    for (s = 0; s < m_NumSegments; s++)
    {
        // Calculate min distance on this segment
        v0  = *pPoint - *pPoint0;
        v1  = *pPoint1 - *pPoint0;

        int64   num = ((int64)(v0.x*v1.x + v0.y*v1.y + v0.z*v1.z)) << 12;
        int64   den = (int64)v1.GetLengthSquaredUnshifted();
        int32   f0  = (int32)(num / den);

        v0  = *pPoint0;
        v1.Normalise();
        v0      = *pPoint0 + (v1 * f0);

        v1      = *pPoint - v0;
        f1      = v1.GetLength();

        if (f1 < min)
        {
            min = f1;
            d   = pSeg->m_PathLength + ((pSeg->m_SegLength * f0) >> 12);
            sd  = s;
        }
        pSeg++;
        pPoint0++;
        pPoint1++;
        if (pPoint1 >= &m_Points[m_NumPoints])
            pPoint1 = m_Points;
    }
    // Store min dist to point?
    if (pDiff)
        *pDiff = min;

    // Store seg ID of min dist to point?
    if (pSegID)
        *pSegID = sd;

    // Return distance along path
    return d;
}*/
//-----------------------------------------------------------------------------
void    CIwPathLine::Serialise()
{
    IW_CALLSTACK("CIwPathLine::Serialise")

    CIwPath::Serialise();

    IwSerialiseUInt32(m_NumPoints);
    IwSerialiseUInt32(m_NumSegments);

    // Allocate data?
    if (g_IwSerialiseContext.read == true)
    {
        m_Points    = new CIwVec3[m_NumPoints];
        m_Segments  = new Segment[m_NumSegments];
    }

    // Points
    IwSerialiseInt32(m_Points[0].x, m_NumPoints * 3);

    // Segments
    for (uint32 s = 0; s < m_NumSegments; s++)
        m_Segments[s].Serialise();
}
//-----------------------------------------------------------------------------
bool    CIwPathLine::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwPathLine::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    static CIwVec3* pPoint = NULL;
    static int32 s_nextSegLength = 0;
    if (!strcmp(pAttrName, "numPoints"))
    {
        uint32 n;
        pParser->ReadUInt32(&n);
        IwAssertMsgN(352,ENGINE, n > 0, ("Zero points"));
        SetNumPoints(n);
        pPoint = m_Points;
        s_nextSegLength = 0;
    }
    else
    if (!strcmp(pAttrName, "p"))
    {
        // Read point in game coords
        pParser->ReadInt32Array(&pPoint->x, 3);
        pPoint++;
    }
    else //old way of doig seglengths
    if (!strcmp(pAttrName, "segLengths"))
    {
        // Read segment lengths
        uint32* lens = new uint32[m_NumSegments];
        pParser->ReadUInt32Array(lens, m_NumSegments);

        Segment*    pSeg    = m_Segments;
        int32       pathLen = 0;
        for (uint32 s = 0; s < m_NumSegments; s++)
        {
            pSeg->m_PathLength  = pathLen;
            pSeg->m_SegLength   = lens[s];
            pathLen             += pSeg->m_SegLength;
            pSeg++;
        }
        // Set total path length
        m_Length = pathLen;

        delete[] lens;
    }
    else //new way of doing seglengths
    if (!strcmp(pAttrName, "segLength"))
    {
        IwAssertMsgN(353,ENGINE, s_nextSegLength < (int32)m_NumSegments, ("Line %s has too many segLengths: got %d, expected %u", DebugGetName(), s_nextSegLength+1, m_NumSegments));
        pParser->ReadInt32(&m_Segments[s_nextSegLength++].m_SegLength);

        if (s_nextSegLength == (int32)m_NumSegments )
        {
            Segment*    pSeg    = m_Segments;
            int32       pathLen = 0;
            for (uint32 s = 0; s < m_NumSegments; s++)
            {
                pSeg->m_PathLength  = pathLen;
                pathLen             += pSeg->m_SegLength;
                pSeg++;
            }
            // Set total path length
            m_Length = pathLen;
        }
    }
    else
        return CIwPath::ParseAttribute(pParser, pAttrName);

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwPathLine::DebugRenderSegmentLines()
{
    IW_CALLSTACK("CIwPathLine::DebugRenderSegmentLines")

    IwGxDebugPrimLineStrip(m_Points, m_NumPoints, m_DebugColour, (m_Flags & CLOSED_F) != 0);
}
//-----------------------------------------------------------------------------
void    CIwPathLine::DebugRenderPointIDs()
{
    IW_CALLSTACK("CIwPathLine::DebugRenderPointIDs")

    for (uint32 p = 0; p < m_NumPoints; p++)
    {
        char line[4];
        sprintf(line, "%u", p);
        // TODO: FIX FOR LONG VEC!
        IwGetRenderManager()->DebugAddText(line, CIwSVec3(m_Points[p]), CIwMenuItem::TITLE);
    }
}
//-----------------------------------------------------------------------------
void    CIwPathLine::DebugRender()
{
    IW_CALLSTACK("CIwPathLine::DebugRender")

    if (m_Points)
    {
        if (m_Flags & DEBUG_RENDER_F)           DebugRenderSegmentLines();
        if (m_Flags & DEBUG_RENDER_POINT_IDS_F) DebugRenderPointIDs();
    }
    // Superclass
    CIwPath::DebugRender();
}
//-----------------------------------------------------------------------------
void    CIwPathLine::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwPathLine::DebugAddMenuItems")

    CIwPath::DebugAddMenuItems(pMenu);
    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_POINT_IDS_F", &m_Flags, DEBUG_RENDER_POINT_IDS_F));
}
#endif  // IW_DEBUG
