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

#include "ModelBlockCollisionMesh.h"



IW_MANAGED_IMPLEMENT(CModelBlockCollisionMesh)
IW_CLASS_FACTORY(CModelBlockCollisionMesh)

//-----------------------------------------------------------------------------
// CModelBlockCollisionMesh
//-----------------------------------------------------------------------------

CModelBlockCollisionMesh::CModelBlockCollisionMesh()
{
    IW_CALLSTACK("CModelBlockCollisionMesh::CModelBlockCollisionMesh")

    SetName("CModelBlockCollisionMesh");

    m_NumItems = 0;
}

CModelBlockCollisionMesh::~CModelBlockCollisionMesh()
{
    IW_CALLSTACK("CModelBlockCollisionMesh::~CModelBlockCollisionMesh")

}

#if defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )
void CModelBlockCollisionMesh::AddRemoveFace(const CIwFace* pFace, bool add, int flags)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::AddRemoveFace")

    IwAssertMsg(RCD, pFace->GetNumPoints() >= 3 && pFace->GetNumPoints() <= 4, ("Face has %d verts", pFace->GetNumPoints()));

    bool flipQuadBisection = false;
    if (pFace->GetNumPoints() == 4)
    {
        CIwFVec3 v0 = IwGetModelBuilder()->GetVert(pFace->GetVertID(0));
        CIwFVec3 v1 = IwGetModelBuilder()->GetVert(pFace->GetVertID(1));
        CIwFVec3 v2 = IwGetModelBuilder()->GetVert(pFace->GetVertID(2));
        CIwFVec3 v3 = IwGetModelBuilder()->GetVert(pFace->GetVertID(3));

        if ((v2-v0).GetLength() > (v3-v1).GetLength())
            flipQuadBisection = true;
    }

    for (int i=0; i<pFace->GetNumPoints()-2; i++)
    {
        CIndexTri tri;
        if (flipQuadBisection)
        {
            tri.m_Verts[0] = (uint16)pFace->GetVertID(0+i);
            tri.m_Verts[1] = (uint16)pFace->GetVertID(3);
            tri.m_Verts[2] = (uint16)pFace->GetVertID(1+i);
        }
        else
        {
            tri.m_Verts[0] = (uint16)pFace->GetVertID(0);
            tri.m_Verts[1] = (uint16)pFace->GetVertID(2+i);
            tri.m_Verts[2] = (uint16)pFace->GetVertID(1+i);
        }
        tri.m_Flags = 0;

        int f = m_Tris.find(tri);

        if (f == -1)
            f = m_Tris.push_back(tri);

        if (add)
            m_Tris[f].m_Flags |= flags;
        else
            m_Tris[f].m_Flags &= ~flags;
    }

    m_NumItems = (uint16)m_Tris.size();
}
void CModelBlockCollisionMesh::Add(CIwModelExtSelSetFace& faces, int flags)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::Add")

    for (CIwArray<uint16>::iterator itF = faces.m_FaceIDs.begin(); itF != faces.m_FaceIDs.end(); ++itF)
        AddRemoveFace(IwGetModelBuilder()->GetFace(*itF), true, flags);
}
void CModelBlockCollisionMesh::Remove(CIwModelExtSelSetFace& faces, int flags)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::Remove")

    for (CIwArray<uint16>::iterator itF = faces.m_FaceIDs.begin(); itF != faces.m_FaceIDs.end(); ++itF)
        AddRemoveFace(IwGetModelBuilder()->GetFace(*itF), false, flags);
}
void CModelBlockCollisionMesh::FindCoincidentVertices(CIwSVec3* vert, int qty)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::FindCoincidentVertices")

    for (uint32 i=0; i<m_Tris.size(); i++)
    {
        for (int j=0; j<3; j++)
        {
            uint16 & v = m_Tris[i].m_Verts[j];
            for (int k=0; k<v; k++)
            {
                if (vert[v] == vert[k])
                {
                    v=k;
                    break;
                }
            }
        }
    }
}
#endif

void CModelBlockCollisionMesh::GetTriangle(uint32 index, CSVec3Tri& out_Tri)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::GetTriangle")

    IwAssert(RCD, index < m_NumItems);

    CIndexTri& tri = GetTriangle(index);
    for (int32 v = 0; v < 3; v++)
    {
        out_Tri.m_Verts[v] = ((CIwSVec3*)m_Owner->GetVerts())[tri.m_Verts[v]];
    }
}


void CModelBlockCollisionMesh::Serialise()
{
    IW_CALLSTACK("CModelBlockCollisionMesh::Serialise")

    CIwModelBlock::Serialise();

    if (g_IwSerialiseContext.read == true)
    {
        m_Tris.resize_optimised( m_NumItems );
    }

    IwSerialiseUInt8(*(uint8*)m_Tris.data(), sizeof(CIndexTri)*m_NumItems);
}

uint32  CModelBlockCollisionMesh::Render(CIwModel* pModel, uint32 flags)
{
    IW_CALLSTACK("CModelBlockCollisionMesh::Render")

    return RENDER_OK;
}
