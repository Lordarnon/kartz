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
#include "IwVolumeBBox.h"
#include "IwResource.h"
#include "IwTextParserITX.h"
#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#endif

//-----------------------------------------------------------------------------
// CIwVolumeBBox
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwVolumeBBox);

IW_MANAGED_IMPLEMENT(CIwVolumeBBox)
CIwVolumeBBox::CIwVolumeBBox()
{
    IW_CALLSTACK("CIwVolumeBBox::CIwVolumeBBox")

    SetName("CIwVolumeBBox");
}
//-----------------------------------------------------------------------------
CIwVolumeBBox::CIwVolumeBBox(const CIwVec3 &a, const CIwVec3 &b)
{
    IW_CALLSTACK("CIwVolumeBBox::CIwVolumeBBox")

    // a and b are two opposing corner points so we need to sort them into
    // min and max values
    m_Bounds.m_Min.x = MIN(a.x, b.x);
    m_Bounds.m_Min.y = MIN(a.y, b.y);
    m_Bounds.m_Min.z = MIN(a.z, b.z);
    m_Bounds.m_Max.x = MAX(a.x, b.x);
    m_Bounds.m_Max.y = MAX(a.y, b.y);
    m_Bounds.m_Max.z = MAX(a.z, b.z);
}

//-----------------------------------------------------------------------------
void CIwVolumeBBox::Init()
{
    IW_CALLSTACK("CIwVolumeBBox::Init")

    CIwVolume::Init();
    IwAssertMsgN(365,ENGINE,   ((m_Bounds.m_Min.x < m_Bounds.m_Max.x)                       && (m_Bounds.m_Min.y < m_Bounds.m_Max.y)                        && (m_Bounds.m_Min.z < m_Bounds.m_Max.z)), ("Badly formed CIwVolumeBBox"));
}
//-----------------------------------------------------------------------------
void CIwVolumeBBox::Serialise()
{
    IW_CALLSTACK("CIwVolumeBBox::Serialise")

    CIwVolume::Serialise();
    m_Bounds.Serialise();
}
//-----------------------------------------------------------------------------
bool CIwVolumeBBox::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwVolumeBBox::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "min"))
    {
        pParser->ReadInt32Array(&m_Bounds.m_Min.x, 3);
    }
    else if (!strcmp(pAttrName, "max"))
    {
        pParser->ReadInt32Array(&m_Bounds.m_Max.x, 3);
    }
    else
    {
        return CIwVolume::ParseAttribute(pParser, pAttrName);
    }

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//-----------------------------------------------------------------------------
CIwVec3 CIwVolumeBBox::Centre()
{
    IW_CALLSTACK("CIwVolumeBBox::Centre")

    return m_Bounds.GetCentre();
}
//-----------------------------------------------------------------------------
bool CIwVolumeBBox::IsPointInside(const CIwVec3 &pt)
{
    IW_CALLSTACK("CIwVolumeBBox::IsPointInside")

    return m_Bounds.ContainsVec(pt, NULL);
}
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void CIwVolumeBBox::DebugRender()
{
    IW_CALLSTACK("CIwVolumeBBox::DebugRender")

    if (m_Flags & DEBUG_RENDER_F)
    {
        // Render a wireframe cuboid
        IwGxDebugPrimBBox(m_Bounds, m_DebugColour);
    }
    // Render parent
    CIwVolume::DebugRender();
}
#endif
//-----------------------------------------------------------------------------
