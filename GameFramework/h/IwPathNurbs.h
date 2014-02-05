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
#ifndef IW_PATHNURBS_H
#define IW_PATHNURBS_H

// Our includes
#include "IwPathLine.h"

/**
 * @addtogroup IwPath
 * @brief IwPathNurbs: beziers.
 * @{
 */

// Forward declarations
class CIwPolynomial;

/**
 * Path: beziers.
 */
class CIwPathNurbs : public CIwPathLine
{
public:
    IW_MANAGED_DECLARE(CIwPathNurbs);

    // Methods
    CIwPathNurbs();
    virtual ~CIwPathNurbs();

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

    /** Get point on a nurbs segment */
    void    GetNurbsPos(CIwVec3* pPos, int32 prevID, int32 t);
    void    GetNurbsPos2(CIwVec3* pPos, int32 prevID, int32 t);

    /** Get tangent to a nurbs segment */
    void    GetNurbsTangent(CIwSVec3* pPos, int32 prevID, int32 t);

    /** Compute Polynomials for each segment of curve */
    void    ComputeSegPolynomials();

    void    Serialise();
    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

#ifdef IW_DEBUG
    virtual void    DebugRender();
#endif

    // Members
protected:
    uint32 m_Order; //!< Order of the curve
    CIwPolynomial* m_SegPolynomials[3]; //!< Numerator Polynomials of the curve segments
    CIwPolynomial* m_SegSumPolynomials; //!< Denominator Polynomials of the curve segments
};

#endif
