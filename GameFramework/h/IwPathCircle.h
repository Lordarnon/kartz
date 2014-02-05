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
#ifndef IW_PATHCIRCLE_H
#define IW_PATHCIRCLE_H

#include "IwShapePath.h"

/**
 * @addtogroup IwPath
 * @brief IwPathCircle: circle.
 * @{
 */

/**
 * Path: circle.
 */
class CIwPathCircle : public CIwPath
{
public:
    IW_MANAGED_DECLARE(CIwPathCircle);

    // Methods
    CIwPathCircle();

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
    /** Set the radius. */
    void    SetRadius(uint32 r);

    /** Get the radius. */
    inline uint32 GetRadius() {return m_Radius;}

    /** Set the origin. */
    inline void SetOrigin(const CIwVec3& p) {m_Origin = p;}

    /** Get the origin */
    inline const CIwVec3* GetOrigin() {return &m_Origin;}

    void    Serialise();
    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

#ifdef IW_DEBUG
    virtual void    DebugRender();
    virtual void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

    // Members
protected:
    CIwVec3     m_Origin;       //!< circle origin.
    uint32      m_Radius;       //!< circle radius.
};

#endif

// @}
