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
#ifndef IW_PATHBEZIER_H
#define IW_PATHBEZIER_H

// Our includes
#include "IwPathLine.h"



/**
 * @addtogroup IwPath
 * @brief IwPathBezier: beziers.
 * @{
 */

// Forward declarations

/**
 * Path: beziers.
 */
class CIwPathBezier : public CIwPathLine
{
public:
    IW_MANAGED_DECLARE(CIwPathBezier);

    /** Flags */
    enum
    {
        PRECALCULATE_BEZIER_VECS_F  = (TYPE_F << 0),    //!< store precalculated vectors to speed up bezier calcs
    };

    // Methods
    CIwPathBezier();
    virtual ~CIwPathBezier();

    //-----------------------------------------------------------------------------
    // CIwPath interface
    //-----------------------------------------------------------------------------
    /** Initialise path: calculate segment distances, etc. */
    virtual void    Init();

    /** Calculate a point at a specified distance along the path. */
    virtual void    CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent = NULL, uint32* pSegID = NULL);

    /** Check if a world point is inside the CLOSED path. */
    virtual bool    IsPointInside(const CIwVec3& p);

    //-----------------------------------------------------------------------------
    // New interface
    //-----------------------------------------------------------------------------
    /** Set number of points in path. Allocates space for all vectors and segments. */
    void    SetNumPoints(uint32 n);

    /** Get point on a bezier segment */
    void    GetBezierPos(CIwVec3* pPos, int32 prevID, int32 t);

    /** Get tangent to a bezier segment */
    void    GetBezierTangent(CIwSVec3* pPos, int32 prevID, int32 t);

    /** Get 2nd derivative to a bezier segment - note the result can be large so requires a CIwVec3 */
    void    GetBezierAcc(CIwVec3* pPos, int32 prevID, int32 t);

    /** Get bezier calculation vectors for all points */
    void    PrecalculateBezierVecs();

    /** Get bezier calculation vectors a, b, c */
    inline  void    GetBezierVecsABC(int32 prevID, CIwVec3* pA, CIwVec3* pB, CIwVec3* pC)
    {
        int32   nextID = (prevID + 1) % m_NumPoints;
        CIwVec3*    p0  = m_Points + prevID;            // (point 0)
        CIwVec3*    p3  = m_Points + nextID;            // (point 1)
        CIwVec3*    p1  = m_ControlsOut + prevID;       // (control point for point 0)
        CIwVec3*    p2  = m_ControlsIn + nextID;        // (control point for point 1)
        CIwVec3     P0  = CIwVec3(*p0);
        CIwVec3     P1  = CIwVec3(*p1);
        CIwVec3     P2  = CIwVec3(*p2);
        CIwVec3     P3  = CIwVec3(*p3);

        // c = 3 * (p1 - p0)
        *pC = (3 << IW_GEOM_POINT) * (P1 - P0);

        // b = 3 * (p2 - p1) - c
        *pB = (3 << IW_GEOM_POINT) * (P2 - P1) - *pC;

        // a = p3 - p0 - c - b;
        *pA = P3 - P0 - *pC - *pB;
    };

    void    Serialise();
    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

#ifdef IW_DEBUG
    virtual void    DebugRender();
#endif

    // Members
protected:
//  uint32      m_SubsPerSeg;   //!< number of subpoints per segment
    CIwVec3*    m_ControlsIn;   //!< ptr to array of "in" control points
    CIwVec3*    m_ControlsOut;  //!< ptr to array of "out" control points
    CIwVec3*    m_VecsABC;      //!< precalculated "a, b, c" vectors to speed up bezier calcs
};

#endif

// @}
