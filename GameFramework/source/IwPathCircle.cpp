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
 * Path: circle.
 */
#include "IwPathCircle.h"
#include "IwResource.h"
#include "IwTextParserITX.h"

#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#endif

IW_CLASS_FACTORY(CIwPathCircle);

#define FIXED_2PI   0x6487

//-----------------------------------------------------------------------------
// CIwPathCircle
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwPathCircle)
CIwPathCircle::CIwPathCircle()
{
    IW_CALLSTACK("CIwPathCircle::CIwPathCircle")

    m_Origin    = CIwVec3::g_Zero;
    m_Radius    = 0;
}
//-----------------------------------------------------------------------------
bool    CIwPathCircle::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwPathCircle::IsPointInside")
    IwAssertMsgN(338,ENGINE, IsInitialised(), ("Path %s is not initialised", DebugGetName()));

    // Safety for points way off!
    int32   dx  = p.x - m_Origin.x;
    if ((dx > INT16_MAX) || (dx < INT16_MIN))
        return false;

    int32   dz  = p.z - m_Origin.z;
    if ((dz > INT16_MAX) || (dz < INT16_MIN))
        return false;

/*  if ((SQR(dx) + SQR(dz)) < SQR(m_Radius))
    {
        // Check Y
        int32 dy = m_Origin.y - p.y;
        if  (
            (dy > 0) &&
            (dy < m_Height)
            )
            return true;
    }*/
    return false;
}
//-----------------------------------------------------------------------------
void    CIwPathCircle::Init()
{
    IW_CALLSTACK("CIwPathCircle::Init")
    IwAssertMsgN(339,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));

    m_Length    = IW_FIXED_MUL(FIXED_2PI, m_Radius);
    m_Flags     |= CLOSED_F;

    // Superclass
    CIwPath::Init();
}
//-----------------------------------------------------------------------------
void    CIwPathCircle::CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent, uint32* pSegID)
{
    IW_CALLSTACK("CIwPathCircle::GetPointAtDistance")
    IwAssertMsgN(340,ENGINE, IsInitialised(), ("Path %s is not initialised", DebugGetName()));

    // Get angle
    int32 theta = (d << 16) / ((m_Radius * FIXED_2PI) >> 8);
    if (pPoint)
    {
        pPoint->x   = m_Origin.x + IW_FIXED_MUL(m_Radius, IW_GEOM_COS(theta));
        pPoint->y   = m_Origin.y;
        pPoint->z   = m_Origin.z + IW_FIXED_MUL(m_Radius, IW_GEOM_SIN(theta));
    }
    if (pTangent)
    {
        pTangent->x = (int16)IW_FIXED_MUL(m_Radius, -IW_GEOM_SIN(theta));
        pTangent->y = 0;
        pTangent->z = (int16)IW_FIXED_MUL(m_Radius, IW_GEOM_COS(theta));
    }
}
//-----------------------------------------------------------------------------
void    CIwPathCircle::Serialise()
{
    IW_CALLSTACK("CIwPathCircle::Serialise")

    CIwPath::Serialise();
    m_Origin.Serialise();
    IwSerialiseUInt32(m_Radius);
}
//-----------------------------------------------------------------------------
bool    CIwPathCircle::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwPathCircle::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "origin"))
        pParser->ReadInt32Array(&m_Origin.x, 3);
    else
    if (!strcmp(pAttrName, "radius"))
        pParser->ReadUInt32(&m_Radius);
    else
        return CIwPath::ParseAttribute(pParser, pAttrName);

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwPathCircle::DebugRender()
{
    IW_CALLSTACK("CIwPathCircle::DebugRender")

    // Render circumference
    if (m_Flags & DEBUG_RENDER_F)
    {
        const uint32    numPoints = 20;
        CIwVec3 points[numPoints];

        for (uint32 p = 0; p < numPoints; p++)
            CalcPointAtDist((m_Length * p) / numPoints, &points[p]);

        IwGxDebugPrimLineStrip(points, numPoints, m_DebugColour, true);
    }

    // Render name
    if (m_Flags & DEBUG_RENDER_NAME_F)
        DebugRenderName();
}
//-----------------------------------------------------------------------------
void    CIwPathCircle::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwPathCircle::DebugAddMenuItems")

    CIwPath::DebugAddMenuItems(pMenu);
    pMenu->AddItem(new CIwMenuItemEditUInt32("Radius", &m_Radius, 0));
}
#endif  // IW_DEBUG
