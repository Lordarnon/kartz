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
 * Path base class.
 */
#include "IwVolume.h"
#include "IwTextParserITX.h"
#include "IwWorld.h"

#ifdef IW_DEBUG
#include "IwRenderManager.h"
#include "IwDebugPrim.h"
#endif

IW_MANAGED_IMPLEMENT(CIwVolume);

//-----------------------------------------------------------------------------
// CIwVolume
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
CIwColour CIwVolume::GetDebugColourDefault()
{
    IW_CALLSTACK("CIwVolume::GetDebugColourDefault")

    return IwGxGetColFixed(IW_GX_COLOUR_YELLOW);
}
//-----------------------------------------------------------------------------
void    CIwVolume::DebugRenderName()
{
    IW_CALLSTACK("CIwVolume::DebugRenderName")

    CIwSVec3 centreS(Centre());
    IwGetRenderManager()->DebugAddText(DebugGetName(), centreS, CIwMenuItem::TITLE);
}
#endif  // IW_DEBUG
