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
#ifndef MODELBLOCKCOLLISIONMESH_H
#define MODELBLOCKCOLLISIONMESH_H

#include "IwModelBlock.h"
#include "IwModelExt.h"

/**
 * A custom model block containing raw triangle info
 */
class CModelBlockCollisionMesh : public CIwModelBlock
{
public:
    IW_MANAGED_DECLARE(CModelBlockCollisionMesh);

    CModelBlockCollisionMesh();
    ~CModelBlockCollisionMesh();

    enum
    {
        DRIVABLE_F = 1
    };

    struct CIndexTri
    {
        uint16 m_Verts[3];
        uint16 m_Flags;
#ifdef IW_DEBUG
        bool operator == (CIndexTri const & o) const
        {
            return m_Verts[0] == o.m_Verts[0] && m_Verts[1]==o.m_Verts[1] && m_Verts[2]==o.m_Verts[2];
        }
#endif
    };
    struct CSVec3Tri
    {
        CIwSVec3 m_Verts[3];
    };

    CIndexTri& GetTriangle(uint32 index)
    {
        IwAssert(RCD, m_Tris.size() == m_NumItems);
        return m_Tris[index];
    }

    //for convenience get a tri structure with actual positions
    void GetTriangle(uint32 index, CSVec3Tri& out_Tri);

//CIwModelBlock overrides

    virtual uint32 GetItemSize()
    {
        return sizeof(CIndexTri);
    };
    virtual void    Serialise();

    virtual uint32  Render(CIwModel* pModel, uint32 flags);

    #if defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )
        void Add(CIwModelExtSelSetFace& faces, int flags);
        void Remove(CIwModelExtSelSetFace& faces, int flags);
        void FindCoincidentVertices(CIwSVec3*, int qty);
    #endif

private:

    #if defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )
        void AddRemoveFace(const CIwFace* pFace, bool add, int flags);
    #endif

    CIwArray<CIndexTri> m_Tris;
};

#endif /* !MODELBLOCKCOLLISIONMESH_H */
