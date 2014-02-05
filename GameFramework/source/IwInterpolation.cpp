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
 * Classes and functions for general value interpolation.
 */

#include "IwInterpolation.h"
#include "IwMath.h"
#include "IwRuntime.h"

//-----------------------------------------------------------------------------
// Values, angles, interpolation
//-----------------------------------------------------------------------------
int32   IwInterpMinimiseAngleDiff(int32 curr, int32& dest)
{
    IW_CALLSTACK("IwInterpMinimiseAngleDiff")

    int32   dA = ABS(dest - curr);
    if (ABS(curr - (dest + 0x1000)) < dA)
        dest += 0x1000;
    else
    if (ABS(curr - (dest - 0x1000)) < dA)
        dest -= 0x1000;

    return ABS(dest - curr);
}
//-----------------------------------------------------------------------------
void    IwInterpBindAnglePhase(int32& curr)
{
    IW_CALLSTACK("IwInterpBindAnglePhase")

    if (curr > 0x800)
        curr -= 0x1000;
    else
    if (curr < -0x800)
        curr += 0x1000;
}
//-----------------------------------------------------------------------------
void    IwInterpBindAnglePhase(int32& curr, int32& dest)
{
    IW_CALLSTACK("IwInterpBindAnglePhase")

    if (curr > 0x800)
    {
        curr -= 0x1000;
        dest -= 0x1000;
    }
    else
    if (curr < -0x800)
    {
        curr += 0x1000;
        dest += 0x1000;
    }
}
