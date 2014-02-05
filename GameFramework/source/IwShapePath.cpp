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
#include "IwShapePath.h"
#include "IwTextParserITX.h"
#include "IwWorld.h"

#ifdef IW_DEBUG
#include "IwRenderManager.h"
#endif

//-----------------------------------------------------------------------------
// CIwPath
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwPath)
CIwPath::CIwPath()
{
    IW_CALLSTACK("CIwPath::CIwPath")

    m_Length    = 0;
}
//-----------------------------------------------------------------------------
uint32  CIwPath::CalcSegIDAtDist(int32 dist)
{
    IW_CALLSTACK("CIwPath::CalcSegIDAtDist")

    IwAssertMsgN(364,ENGINE, IsInitialised(), ("Path not initialised"));

    uint32 segID;
    CalcPointAtDist(dist, NULL, NULL, &segID);
    return segID;
}
//-----------------------------------------------------------------------------
void    CIwPath::Serialise()
{
    IW_CALLSTACK("CIwPath::Serialise")

    CIwShape::Serialise();
    IwSerialiseUInt32(m_Length);
}


//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
CIwColour CIwPath::GetDebugColourDefault()
{
    IW_CALLSTACK("CIwPath::GetDebugColourDefault")

    return IwGxGetColFixed(IW_GX_COLOUR_MAGENTA);
}
//-----------------------------------------------------------------------------
void    CIwPath::DebugRenderName()
{
    IW_CALLSTACK("CIwPath::DebugRenderName")

    // TODO: FIX FOR LONG VEC!
    CIwVec3 nv;
    CalcPointAtDist(0, &nv);
    nv.x += 0x10;
    nv.z += 0x10;
    IwGetRenderManager()->DebugAddText(DebugGetName(), CIwSVec3(nv), CIwMenuItem::TITLE);
}
//-----------------------------------------------------------------------------
#endif  // IW_DEBUG
