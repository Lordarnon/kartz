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
#include "RacingDemo.h"
#include "GeomModel.h"

#include "IwResManager.h"
#include "IwTextParserITX.h"
#include "IwMaterial.h"
#include "World.h"
#include "IwModel.h"


#define SHADOW_ZDEPTHOFS    -200
#define SHADOW_DIM          300

IW_MANAGED_IMPLEMENT(CShadow)

//--------------------------------------------------------------------------------
// CShadow
//--------------------------------------------------------------------------------
CShadow::CShadow()
:   m_pGeometry( NULL ),
    m_Intensity(255)
{
    m_Mat = (CIwMaterial*)IwGetResManager()->GetResNamed("shadow", IW_GX_RESTYPE_MATERIAL, IW_RES_SEARCH_ALL_F);

    // offset shadow material so that shadow appears in front of the ground
    m_Mat->SetZDepthOfs(SHADOW_ZDEPTHOFS);
}

//--------------------------------------------------------------------------------

void CShadow::Init()
{
    m_Intensity = 255;

    CIwGeomModel::Init();
}

//--------------------------------------------------------------------------------

void CShadow::Render()
{
    IW_PROFILE( "Shadow->Render" )

    if (m_Intensity == 0)
        return;

    // render shadow based on parent's foot bones, projected onto x-z plane
    CIwSVec3* verts = IW_GX_ALLOC(CIwSVec3, 4);

    // create local coord system on floor
    CIwVec3 axisX   = m_pGeometry->GetFrame()->GetMatWorld()->ColumnX();
            axisX.y = 0;
            axisX.Normalise();

    // shadow rendered around base of the object
    verts[0] = SHADOW_DIM * (-CIwVec3::g_AxisZ + CIwVec3::g_AxisX);
    verts[1] = SHADOW_DIM * (-CIwVec3::g_AxisZ - CIwVec3::g_AxisX);
    verts[2] = SHADOW_DIM * ( CIwVec3::g_AxisZ - CIwVec3::g_AxisX);
    verts[3] = SHADOW_DIM * ( CIwVec3::g_AxisZ + CIwVec3::g_AxisX);

    CIwSVec2* uvs = IW_GX_ALLOC(CIwSVec2, 4);
    uvs[0] = CIwSVec2(0 << 12, 0 << 12);
    uvs[1] = CIwSVec2(1 << 12, 0 << 12);
    uvs[2] = CIwSVec2(1 << 12, 1 << 12);
    uvs[3] = CIwSVec2(0 << 12, 1 << 12);

    CIwColour *cols = IW_GX_ALLOC(CIwColour, 4);
    memset(cols, m_Intensity, sizeof(CIwColour) * 4);

    IwGxSetMaterial(m_Mat);
    IwGxSetVertStreamModelSpace(verts, 4);
    IwGxSetUVStream(uvs);
    IwGxSetColStream(cols);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

IW_MANAGED_IMPLEMENT(CGeomModel);
IW_CLASS_FACTORY(CGeomModel);

//--------------------------------------------------------------------------------

CGeomModel::CGeomModel( CShadow* pShadow )
:   m_pShadow(pShadow)
{
    SetFrameOwned(new CIwFrame);
    m_Flags |= SERIALISE_FRAME_F;

    m_BSphere.SetRadius(0);

    if (m_pShadow)
    {
        m_pShadow->SetGeometry(this);
    }
}

//--------------------------------------------------------------------------------

CGeomModel::~CGeomModel()
{
    delete m_pShadow;
}

//--------------------------------------------------------------------------------

bool CGeomModel::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
#ifdef IW_BUILD_RESOURCES

    if (strcmp(pAttrName, "model") == 0 )
    {
        char buffer[256];
        pParser->ReadString(buffer, 256);
        m_Model = (CIwModel*)IwGetResManager()->GetResNamed(buffer, "CIwModel");
        return true;
    }

#endif // #ifdef IW_BUILD_RESOURCES
    return CIwGeomModel::ParseAttribute(pParser, pAttrName);
}

//--------------------------------------------------------------------------------

void CGeomModel::ParseClose(CIwTextParserITX* pParser)
{
#ifdef IW_BUILD_RESOURCES
    GetWorld().AddGeom(this);
#endif // #ifdef IW_BUILD_RESOURCES
}

//--------------------------------------------------------------------------------

void CGeomModel::Render()
{
    if (m_BSphere.GetRadius() == 0)
    {
        m_BSphere = *m_Model->GetBSphere();
    }

    SetModelViewTransform();
    CIwGeomModel::Render();

    if (m_pShadow)
    {
        m_pShadow->Render();
    }
}

//--------------------------------------------------------------------------------
