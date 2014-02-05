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
 * Path: beziers.
 */

#include "IwPathBezier.h"
#include "IwResource.h"
#include "IwTextParserITX.h"

#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#endif

IW_CLASS_FACTORY(CIwPathBezier);

//-----------------------------------------------------------------------------
// CIwPathBezier
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwPathBezier)
CIwPathBezier::CIwPathBezier()
{
    IW_CALLSTACK("CIwPathBezier::CIwPathBezier")

    m_ControlsIn    = NULL;
    m_ControlsOut   = NULL;
    m_VecsABC       = NULL;
}
//-----------------------------------------------------------------------------
CIwPathBezier::~CIwPathBezier()
{
    IW_CALLSTACK("CIwPathBezier::~CIwPathBezier")

    delete[] m_VecsABC;
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::SetNumPoints(uint32 n)
{
    IW_CALLSTACK("CIwPathBezier::SetNumPoints")
    IwAssertMsgN(328,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));

    m_NumPoints     = n;
    m_NumSegments   = (m_Flags & CLOSED_F) ? m_NumPoints : (m_NumPoints - 1);

    // Allocate space for segments
    m_Segments  = new Segment[m_NumSegments];

    // Allocate space for points, control points, and pre-calculated vectors
    int32   numVecs = m_NumPoints;
    if (n)
    {
        //...allow space for tangents (control points)
        numVecs += m_NumPoints * 2;

        //...allow space for precalculated vecs?
        if (m_Flags & PRECALCULATE_BEZIER_VECS_F)
        {
            delete[] m_VecsABC;
            m_VecsABC   = new CIwVec3[m_NumPoints * 3];
        }

        delete[] m_Points;
        m_Points        = new CIwVec3[numVecs];
        m_ControlsIn    = m_Points + m_NumPoints;
        m_ControlsOut   = m_ControlsIn + m_NumPoints;
    }
    else
        m_Points = NULL;
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::Init()
{
    IW_CALLSTACK("CIwPathBezier::Init")
    IwAssertMsgN(329,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));
    IwAssertMsgN(330,ENGINE, m_Points, ("Path %s has no points", DebugGetName()));
    IwAssertMsgN(331,ENGINE, m_Segments, ("Path %s has no segments", DebugGetName()));

    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    Segment*    pSeg    = m_Segments;
    uint32      d       = 0;
    CIwVec3     v0;

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

    // Calculate bezier helper vecs?
    if (m_Flags & PRECALCULATE_BEZIER_VECS_F)
        PrecalculateBezierVecs();

    // Base class
    CIwPath::Init();
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::PrecalculateBezierVecs()
{
    IW_CALLSTACK("CIwPathBezier::PrecalculateBezierVecs")
    IwAssertMsgN(332,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));
    IwAssertMsgN(333,ENGINE, m_VecsABC, ("Path %s has no buffer to precalculate helpers", DebugGetName()));

    CIwVec3* pVec = m_VecsABC;
    for (uint32 p = 0; p < m_NumPoints; p++)
    {
        GetBezierVecsABC(p, &pVec[0], &pVec[1], &pVec[2]);
        pVec += 3;
    }
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent, uint32* pSegID)
{
    IW_CALLSTACK("CIwPathBezier::CalcPointAtDist")
    IwAssertMsgN(334,ENGINE, IsInitialised(), ("Path %s is not initialised", DebugGetName()));

    // Note: this can be slow
    Segment*    pSeg    = m_Segments;
    int32       s       = m_NumSegments;
    int32       prevID  = 0;
    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    int32       f;

    if (!(m_Flags & CLOSED_F))
    {
        // Open path
        if (d >= (int32)m_Length)
        {
            if (pPoint)     GetBezierPos(pPoint, m_NumPoints - 2, IW_GEOM_ONE);
            if (pTangent)   GetBezierTangent(pTangent, m_NumPoints - 2, IW_GEOM_ONE);
            if (pSegID)     *pSegID = m_NumSegments - 1;
            return;
        }
        else
        if (d <= 0)
        {
            if (pPoint)     GetBezierPos(pPoint, 0, 0);
            if (pTangent)   GetBezierTangent(pTangent, 0, 0);
            if (pSegID)     *pSegID = 0;
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

    while (s--)
    {
        if (d <= (pSeg->m_PathLength + pSeg->m_SegLength))
        {
            // Calculate position on this segment
            f   = ((d - pSeg->m_PathLength) << IW_GEOM_POINT) / pSeg->m_SegLength;
            if (pPoint)     GetBezierPos(pPoint, prevID, f);
            if (pTangent)   GetBezierTangent(pTangent, prevID, f);
            if (pSegID)     *pSegID = m_NumSegments - s;
            return;
        }
        pSeg++;
        pPoint0++;
        pPoint1++;
        prevID++;
        if (pPoint1 >= &m_Points[m_NumPoints])
            pPoint1 = m_Points;
    }
    IwAssertMsgN(335,ENGINE, false, ("d within path length but not within a segment"));
}
//-----------------------------------------------------------------------------
bool    CIwPathBezier::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwPathBezier::IsPointInside")

    // NOTE: WE CHECK THE SHAPE ENCLOSED BY THE LINES JOINING THE CONTROL POINTS
    return CIwPathLine::IsPointInside(p);
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::GetBezierPos(CIwVec3* pPos, int32 prevID, int32 t)
{
    IW_CALLSTACK("CIwPathBezier::GetBezierPos")

    if (!pPos)
        return;

    IwAssertMsgN(336,ENGINE, (t >=0) && (t <= 0x1000), ("Bezier param out of range"));

    // TODO: maintain more accuracy here
    int32   t2  = (t * t) >> IW_GEOM_POINT;
    int32   t3  = (t2 * t) >> IW_GEOM_POINT;

    CIwVec3* p0 = m_Points + prevID;

    // pPos = (a * t3) + (b * t2) + (c * t) + p0
    if (m_VecsABC)
    {
        CIwVec3*    pA  = m_VecsABC + prevID * 3;
        CIwVec3*    pB  = pA + 1;
        CIwVec3*    pC  = pB + 1;
        *pPos = *p0 + (*pA * t3) + (*pB * t2) + (*pC * t);
    }
    else
    {
        CIwVec3 a, b, c;
        GetBezierVecsABC(prevID, &a, &b, &c);
        *pPos = *p0 + (a * t3) + (b * t2) + (c * t);
    }
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::GetBezierTangent(CIwSVec3* pVec, int32 prevID, int32 t)
{
    IW_CALLSTACK("CIwPathBezier::GetBezierTangent")

    if (!pVec)
        return;

    int32   t2  = (t * t) >> IW_GEOM_POINT;

    // pVec = (3 * a * t2) + (2 * b * t) + c
    if (m_VecsABC)
    {
        CIwVec3*    pA  = m_VecsABC + prevID * 3;
        CIwVec3*    pB  = pA + 1;
        CIwVec3*    pC  = pB + 1;
        CIwVec3     vt  = (*pA * (t2 * 3)) + (*pB * (t * 2)) + *pC;
        vt.Normalise();
        *pVec   = CIwSVec3(vt);
    }
    else
    {
        CIwVec3 a, b, c;
        GetBezierVecsABC(prevID, &a, &b, &c);
        CIwVec3 vt  = (a * (t2 * 3)) + (b * (t * 2)) + c;
        vt.Normalise();
        *pVec   = CIwSVec3(vt);
    }
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::GetBezierAcc(CIwVec3* pVec, int32 prevID, int32 t)
{
    IW_CALLSTACK("CIwPathBezier::GetBezierAcc")

    if (!pVec)
        return;

    // pVec = (6 * a * t) + (2 * b)
    if (m_VecsABC)
    {
        CIwVec3*    pA  = m_VecsABC + prevID * 3;
        CIwVec3*    pB  = pA + 1;
        *pVec   = (*pA * (t * 6)) + (*pB << 1);
    }
    else
    {
        CIwVec3 a, b, c;
        GetBezierVecsABC(prevID, &a, &b, &c);
        *pVec   = (a * (t * 6)) + (b << 1);
    }
}
//-----------------------------------------------------------------------------
void    CIwPathBezier::Serialise()
{
    IW_CALLSTACK("CIwPathBezier::Serialise")

    // Base class
    CIwPath::Serialise();

    IwSerialiseUInt32(m_NumPoints);
    IwSerialiseUInt32(m_NumSegments);

    // Allocate data?
    if (g_IwSerialiseContext.read == true)
    {
        m_Points    = new CIwVec3[m_NumPoints * 3]; // points and control points
        m_Segments  = new Segment[m_NumSegments];

        if (m_Flags & PRECALCULATE_BEZIER_VECS_F)
            m_VecsABC = new CIwVec3[m_NumPoints * 3];
        else
            m_VecsABC = NULL;
    }

    // Points
    IwSerialiseInt32(m_Points[0].x, m_NumPoints * 3 * 3);       // points and control points

    // Segments
    for (uint32 n = 0; n < m_NumSegments; n++)
        m_Segments[n].Serialise();

    m_ControlsIn    = m_Points + m_NumPoints;
    m_ControlsOut   = m_ControlsIn + m_NumPoints;

    if (m_Flags & PRECALCULATE_BEZIER_VECS_F)
        IwSerialiseInt32(m_VecsABC[0].x, m_NumPoints * 3 * 3);
}
//-----------------------------------------------------------------------------
bool    CIwPathBezier::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwPathBezier::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    static CIwVec3* pPoint = NULL;
    static CIwVec3* pPointI = NULL;
    static CIwVec3* pPointO = NULL;
    if (!strcmp(pAttrName, "numPoints"))
    {
        uint32 n;
        pParser->ReadUInt32(&n);
        IwAssertMsgN(337,ENGINE, n > 0, ("Zero points"));
        SetNumPoints(n);
        pPoint  = m_Points;
        pPointI = m_ControlsIn;
        pPointO = m_ControlsOut;
    }
    else
    if (!strcmp(pAttrName, "p"))
    {
        // Read point
        pParser->ReadInt32Array(&pPoint->x, 3);
        pPoint++;
    }
    else
    if (!strcmp(pAttrName, "pi"))
    {
        // Read control point (in)
        pParser->ReadInt32Array(&pPointI->x, 3);
        pPointI++;
    }
    else
    if (!strcmp(pAttrName, "po"))
    {
        // Read control point (out)
        pParser->ReadInt32Array(&pPointO->x, 3);
        pPointO++;
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
void    CIwPathBezier::DebugRender()
{
    IW_CALLSTACK("CIwPathBezier::DebugRender")

#define PATH_DEBUG_LINES_PER_SEG    10

    if (m_Flags & DEBUG_RENDER_F)
    {
        // Render segments as curves (multiple lines)
        int32       numP    = PATH_DEBUG_LINES_PER_SEG * m_NumSegments;
        CIwVec3*    pts     = new CIwVec3[numP];
        CIwVec3*    pVec    = pts;
        for (uint32 s = 0; s < m_NumSegments; s++)
        {
            for (uint32 n = 0; n < PATH_DEBUG_LINES_PER_SEG; n++)
            {
                GetBezierPos(pVec, s, (n << IW_GEOM_POINT) / PATH_DEBUG_LINES_PER_SEG);
                pVec++;
            }
        }
        IwGxDebugPrimLineStrip(pts, numP, m_DebugColour, (m_Flags & CLOSED_F) != 0);
        delete[] pts;
    }

    // Base class
    CIwPath::DebugRender();
    if (m_Flags & DEBUG_RENDER_POINT_IDS_F)
        DebugRenderPointIDs();
}
#endif  // IW_DEBUG
