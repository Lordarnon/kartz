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

#include "IwShape.h"
#include "IwResource.h"
#include "IwTextParserITX.h"

// Static members
IW_MANAGED_IMPLEMENT(CIwShape)
const char* CIwShape::g_FlagNames[] =
{
    "INITIALISED_F",
    "CLOSED_F",
    "DEBUG_RENDER_F",
    "DEBUG_RENDER_NAME_F",
    "TYPE_F",
    "TYPE_CAMERA_F",
    "TYPE_TRIGGER_F",
    "NO_PROJECT_F",
    "DEBUG_RENDER_POINT_IDS_F",
    NULL,
};

//-----------------------------------------------------------------------------
// CIwShape
//-----------------------------------------------------------------------------
CIwShape::CIwShape()
{
    IW_CALLSTACK("CIwShape::CIwShape")

    m_Flags = 0;
}
void CIwShape::Init()
{
    IW_CALLSTACK("CIwShape::Init")

    m_Flags |= INITIALISED_F;
#ifdef IW_DEBUG
    m_DebugColour = GetDebugColourDefault();
#endif
}
//-----------------------------------------------------------------------------
void    CIwShape::Serialise()
{
    IW_CALLSTACK("CIwShape::Serialise")

    // Base class
    CIwManaged::Serialise();

    // Don't write the initialised flag
    if (g_IwSerialiseContext.read)
    {
        IwSerialiseUInt32(m_Flags);
    }
    else
    {
        uint32 flags = m_Flags & ~INITIALISED_F;
        IwSerialiseUInt32(flags);
    }
}
//-----------------------------------------------------------------------------
void    CIwShape::ParseClose(CIwTextParserITX* pParser)
{
#ifdef IW_BUILD_RESOURCES

    IW_CALLSTACK("CIwShape::ParseClose")

    Init();
    CIwManaged::ParseClose(pParser);

#endif // #ifdef IW_BUILD_RESOURCES
}
//-----------------------------------------------------------------------------
bool    CIwShape::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwShape::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "flags"))
    {
        char line[80];
        pParser->ReadString(line, 80);
        pParser->FilterStringSetFlags(line, g_FlagNames, m_Flags);
    }
    else
        return CIwManaged::ParseAttribute(pParser, pAttrName);

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//-----------------------------------------------------------------------------
void    CIwShape::Resolve()
{
    CIwManaged::Resolve();
    Init();
}
//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
CIwColour CIwShape::GetDebugColourDefault()
{
    IW_CALLSTACK("CIwShape::GetDebugColourDefault")

    return IwGxGetColFixed(IW_GX_COLOUR_BLUE);
}
//-----------------------------------------------------------------------------
void    CIwShape::DebugRender()
{
    IW_CALLSTACK("CIwShape::DebugRender")

    // Check shape was initialised
    IwAssertMsgN(363,ENGINE, TestFlag(INITIALISED_F), ("CIwShape not initialised"));

    if (m_Flags & DEBUG_RENDER_NAME_F)
        DebugRenderName();
}
//-----------------------------------------------------------------------------
void    CIwShape::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwShape::DebugAddMenuItems")

    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_F",       &m_Flags, DEBUG_RENDER_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_NAME_F",  &m_Flags, DEBUG_RENDER_NAME_F));
}
#endif  // IW_DEBUG
