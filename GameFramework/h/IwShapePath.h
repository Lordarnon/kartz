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
#ifndef IW_SHAPE_PATH_H
#define IW_SHAPE_PATH_H

// Our includes
#include "IwShape.h"

/**
 * @defgroup IwPath
 *
 * @addtogroup IwPath
 * @brief Path base class.
 * @{
 *
 * Path base class.
 */
class CIwPath : public CIwShape
{
public:
    IW_MANAGED_DECLARE(CIwPath);

    // Methods
    CIwPath();
    virtual ~CIwPath() {};

    /** Calculate a point at a specified distance along the path. */
    virtual void    CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent = NULL, uint32* pSegID = NULL) = 0;

    /** Calculate the segment ID corresponding to a distance on the path. */
    uint32  CalcSegIDAtDist(int32 dist);

    /** Get length. */
    inline uint32 GetLength() {return m_Length;}

    /** IO functions */
    virtual void    Serialise();

#ifdef IW_DEBUG
    /** Default colour to draw objects of this type */
    virtual CIwColour   GetDebugColourDefault();
    /** Render path name */
    void    DebugRenderName();
#endif

    // Members
protected:
    uint32  m_Length;   //!< path length (calculated by Init())
};

#endif

// @}
