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
#include "IwVolumeVCylinder.h"
#include "IwGeomCore.h"
#include "IwTextParserITX.h"
#include "IwResource.h"

#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#endif


//-----------------------------------------------------------------------------
// CIwVolumeVCylinder
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwVolumeVCylinder);
IW_MANAGED_IMPLEMENT(CIwVolumeVCylinder)
CIwVolumeVCylinder::CIwVolumeVCylinder()
{
    IW_CALLSTACK("CIwVolumeVCylinder::CIwVolumeVCylinder")

    SetName("CIwVolumeVCylinder");
}
//-----------------------------------------------------------------------------
void CIwVolumeVCylinder::Init()
{
    IW_CALLSTACK("CIwVolumeVCylinder::Init")

    // Assert consistency
    IwAssert(ENGINE, m_Length > 0);
    IwAssert(ENGINE, m_Radius > 0);
    // Calcluate squared radius
    m_RadiusSqF = IW_FIXED(m_Radius * m_Radius);

    CIwVolume::Init();
}
//-----------------------------------------------------------------------------
CIwVec3 CIwVolumeVCylinder::Centre()
{
    IW_CALLSTACK("CIwVolumeVCylinder::Centre")

    return m_Centre;
}
//-----------------------------------------------------------------------------
bool CIwVolumeVCylinder::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwVolumeVCylinder::IsPointInside")

    // Test y position against length
    if (ABS(m_Centre.y - p.y) > (m_Length / 2))
        return false;

    // Test xz position against radius
    iwfixed dx = m_Centre.x - p.x;
    iwfixed dz = m_Centre.z - p.z;
    //return ((mulsafe(dx, dx) + mulsafe(dz, dz)) <= m_RadiusSqF);
    return (((dx * dx) + (dz * dz)) <= (m_Radius * m_Radius));
}
//-----------------------------------------------------------------------------
void CIwVolumeVCylinder::Serialise()
{
    IW_CALLSTACK("CIwVolumeVCylinder::Serialise")

    // Base class
    CIwVolume::Serialise();
    // Members
    m_Centre.Serialise();
    IwSerialiseInt32(m_Length, 1);
    IwSerialiseInt32(m_Radius, 1);
}
//-----------------------------------------------------------------------------
bool CIwVolumeVCylinder::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwVolumeVCylinder::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "pos"))
        pParser->ReadInt32Array(&m_Centre.x, 3);
    else if (!strcmp(pAttrName, "height"))
        pParser->ReadInt32(&m_Length);
    else if (!strcmp(pAttrName, "radius"))
        pParser->ReadInt32(&m_Radius);
    else
        return CIwVolume::ParseAttribute(pParser, pAttrName);

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void CIwVolumeVCylinder::DebugRender()
{
    IW_CALLSTACK("CIwVolumeVCylinder::DebugRender")

    if (m_Flags & DEBUG_RENDER_F)
    {
        // Calculate positions of centres of ends
        CIwVec3 endPts[2] = { m_Centre, m_Centre };
        endPts[0].y -= m_Length / 2;
        endPts[1].y += m_Length / 2;

        // Major axis
        IwGxDebugPrimLine(endPts[0], endPts[1], m_DebugColour);

        // Cross to indicate centre
        IwGxDebugPrimCross(m_Centre, 4, IwGxGetColFixed(IW_GX_COLOUR_RED));

        // Circles at either end
        IwGxDebugPrimCircle(CIwMat(CIwMat::g_Identity, endPts[0]), m_Radius, 1, m_DebugColour, false);
        IwGxDebugPrimCircle(CIwMat(CIwMat::g_Identity, endPts[1]), m_Radius, 1, m_DebugColour, false);
    }

    // Render parent
    CIwVolume::DebugRender();
}
#endif
