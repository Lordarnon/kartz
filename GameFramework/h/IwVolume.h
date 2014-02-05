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
#ifndef IW_VOLUME_H
#define IW_VOLUME_H

// Our includes
#include "IwShape.h"
#include "IwGeomVec.h"

/**
 * @defgroup IwVolume IwVolume
 *
 * @addtogroup IwVolume
 * @brief Volume base class.
 * @{
 *
 * Volume base class. Abstract.
 */
class CIwVolume : public CIwShape
{
public:
    IW_MANAGED_DECLARE(CIwVolume);

    // Methods
    /** Constructor */
    CIwVolume() {};
    /** Returns a point at the centre of the volume */
    virtual CIwVec3 Centre() = 0;

#ifdef IW_DEBUG
    /** Default colour to draw objects of this type */
    virtual CIwColour   GetDebugColourDefault();
    /** Render volume name */
    virtual void    DebugRenderName();
#endif
};

#endif

// @}
