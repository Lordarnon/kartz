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
#ifndef IW_PATHLINE_H
#define IW_PATHLINE_H

// Our includes
#include "IwShapePath.h"

/**
 * @addtogroup IwPath
 * @brief IwPathLine: straight lines.
 * @{
 *
 *
 * Path: straight lines.
 */
class CIwPathLine : public CIwPath
{
public:
    IW_MANAGED_DECLARE(CIwPathLine);

    /** Segment structure */
    struct Segment
    {
        // Members
        int32   m_SegLength;    //!< length spanned by segment (depends on segment type) (calculated by Init())
        int32   m_PathLength;   //!< path length at start of segment (calculated by Init())

        /** Read/write binary data */
        inline void Serialise()
        {
            IwSerialiseInt32(m_SegLength);
            IwSerialiseInt32(m_PathLength);
        }
    };

    // Methods
    CIwPathLine();
    virtual ~CIwPathLine();

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
    virtual void    SetNumPoints(uint32 n);

    /** Accessors */
    uint32 GetNumPoints() const     { return m_NumPoints; }
    uint32 GetNumSegments() const   { return m_NumSegments; }

    /** Set a point */
    void    SetPoint(uint32 id, const CIwVec3& p);

    /** Get a pointer to a point. */
    inline CIwVec3* GetPoint(uint32 p)
    {
        IwAssertMsgN(295,ENGINE, p < m_NumPoints, ("Point ID too large"));
        return &m_Points[p];
    }

    /** Get the distance along the path which results in the shortest distance to a specified point */
//  virtual uint32  CalcDistClosestTo(CIwVec3* pPoint, uint32* pDiff, uint32* pSegID) = 0;

    void    Serialise();
    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

#ifdef IW_DEBUG
    virtual void    DebugRender();
    virtual void    DebugAddMenuItems(CIwMenu* pMenu);

    /** Render path segments as lines */
    void    DebugRenderSegmentLines();

    /** Render path point IDs */
    void    DebugRenderPointIDs();
#endif

    // Members
protected:
    CIwVec3*    m_Points;       //!< ptr to array of points
    uint32      m_NumPoints;    //!< number of points

    Segment*    m_Segments;     //!< path segment structures
    uint32      m_NumSegments;  //!< number of segments on the path
};

#endif

// @}
