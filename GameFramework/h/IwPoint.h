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
#ifndef IW_POINT_H
#define IW_POINT_H

// Our includes
#include "IwShape.h"
#include "IwGeomVec.h"

/**
 * @defgroup IwPoint IwPoint
 */

/**
 * @addtogroup IwPoint
 * @brief Volume base class. Abstract.
 * @{
 */

// Forward declarations

//-----------------------------------------------------------------------------
// CIwPoint
//-----------------------------------------------------------------------------
/** Volume base class. Abstract. */
class CIwPoint : public CIwShape, public CIwVec3
{
public:
    IW_MANAGED_DECLARE(CIwPoint);

    // Methods
    /** Constructors */
    CIwPoint() {};
    CIwPoint(iwfixed x, iwfixed y, iwfixed z) : CIwVec3(x, y, z) {};

    /** This doesn't necessarily make sense for a point. Returns true if points are equal. */
    virtual bool    IsPointInside(const CIwVec3& p);

    /** IO functions */
    virtual bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
//  virtual void    ParseClose(CIwTextParserITX* pParser);
    virtual void    Serialise();

#ifdef IW_DEBUG
    /** Default colour to draw objects of this type */
    virtual CIwColour   GetDebugColourDefault();
    virtual void    DebugRender();
    /** Debug rendering */
    virtual void    DebugRenderName();
#endif
};

#endif //IW_POINT_H


// @}
