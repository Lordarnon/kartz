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
#include "IwPoint.h"
#include "IwWorld.h"
#include "IwTextParserITX.h"

#ifdef IW_DEBUG
#include "IwRenderManager.h"
#include "IwDebugPrim.h"
#endif

//-----------------------------------------------------------------------------
// CIwPoint
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwPoint);
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwPoint)
bool CIwPoint::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwPoint::IsPointInside")

    return (p == *this);
}
//-----------------------------------------------------------------------------
bool CIwPoint::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwPoint::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "pos"))
    {
        pParser->ReadInt32Array(&x, 3);
    }
    else
    {
        return CIwShape::ParseAttribute(pParser, pAttrName);
    }

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//-----------------------------------------------------------------------------
void CIwPoint::Serialise()
{
    IW_CALLSTACK("CIwPoint::Serialise")

    CIwShape::Serialise();
    CIwVec3::Serialise();
}
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
CIwColour CIwPoint::GetDebugColourDefault()
{
    IW_CALLSTACK("CIwPoint::GetDebugColourDefault")

    return IwGxGetColFixed(IW_GX_COLOUR_RED);
}
//-----------------------------------------------------------------------------
void    CIwPoint::DebugRender()
{
    IW_CALLSTACK("CIwPoint::DebugRender")

    if (TestFlag(DEBUG_RENDER_F))
    {
        IwGxDebugPrimCross(*this, 10, m_DebugColour);
    }
    CIwShape::DebugRender();
}
//-----------------------------------------------------------------------------
void    CIwPoint::DebugRenderName()
{
    IW_CALLSTACK("CIwPoint::DebugRenderName")

    CIwSVec3 pointS(*this);
    IwGetRenderManager()->DebugAddText(DebugGetName(), pointS, CIwMenuItem::TITLE);
}
#endif  // IW_DEBUG
//-----------------------------------------------------------------------------
