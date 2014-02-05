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
#ifndef IW_VOLUMEBBOX_H
#define IW_VOLUMEBBOX_H

// Our includes
#include "IwGeomBBox.h"
#include "IwVolume.h"

/**
 * @addtogroup IwVolume
 * @brief Axis aligned bounding box.
 * @{
 *
 *
 * Axis aligned bounding box
 */
class CIwVolumeBBox : public CIwVolume
{
public:
    IW_MANAGED_DECLARE(CIwVolumeBBox);

    /** Constructor */
    CIwVolumeBBox();
    /** Constructor from two opposite corner points */
    CIwVolumeBBox(const CIwVec3 &cornerA, const CIwVec3 &cornerB);

    /** Initialise volume: Any necessary pre-calculation and consistency checking */
    virtual void    Init();

    virtual void    Serialise();
    virtual bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

    /** Returns a point at the centre of the volume */
    virtual CIwVec3 Centre();
    /** Check if a world point is inside the volume */
    virtual bool    IsPointInside(const CIwVec3& p);
#ifdef IW_DEBUG
    void            DebugRender();
#endif

    CIwBBox         m_Bounds;   //!< Max and min x, y and z values
};

#endif //IW_VOLUMEBBOX_H

// @}
