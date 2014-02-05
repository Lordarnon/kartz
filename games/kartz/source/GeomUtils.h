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
#ifndef GEOMUTILS_H
#define GEOMUTILS_H

#include <stdlib.h>

class CGeomUtils
{
public:
    static iwangle GetHeading(const CIwVec3& vec)
    {
        return IwGeomAtan2(vec.x, -vec.z);
    }

    static iwangle Normalise(iwangle angle)
    {
        while (angle >= IW_ANGLE_2PI)
            angle -= IW_ANGLE_2PI;
        while (angle < 0)
            angle += IW_ANGLE_2PI;

        return angle;
    }

    static iwangle GetShortestTurn(iwangle from, iwangle to)
    {
        iwangle normal = Normalise(to) - Normalise(from);
        iwangle opposite = (IW_ANGLE_2PI - abs(normal));
        if (abs(normal) <= abs(opposite))
        {
            IwAssert(RCD, Normalise(from+normal) == Normalise(to));
            return normal;
        }

        if (normal > 0)
            opposite = -opposite;

        IwAssert(RCD, Normalise(from+opposite) == Normalise(to));
        return opposite;
    }
};

#endif /* !GEOMUTILS_H */
