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
#include "IwGeomModel.h"
#include "IwGraphics.h"
#include "IwModel.h"
#include "IwModelBlockGen.h"
#include "IwResManager.h"

//-----------------------------------------------------------------------------
// CIwGeomModel
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwGeomModel);
IW_MANAGED_IMPLEMENT(CIwGeomModel)
CIwGeomModel::CIwGeomModel()
{
    IW_CALLSTACK("CIwGeomModel::CIwGeomModel")

    m_Model = NULL;

    m_VertCols = NULL;
}
//-----------------------------------------------------------------------------
CIwGeomModel::~CIwGeomModel()
{
    IW_CALLSTACK("CIwGeomModel::~CIwGeomModel")

    if (m_VertCols)
    {
        delete m_VertCols;
    }
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::Render()
{
    IW_CALLSTACK("CIwGeomModel::Render")

    if (m_Model)
    {
        SetVertCols();
        m_Model->Render();
        ClearVertCols();
    }
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::Serialise()
{
    IW_CALLSTACK("CIwGeomModel::Serialise")

    CIwGeom::Serialise();
    IwSerialiseManagedHash(&m_Model);

    // Serialise optional VertCols block
    bool bHaveColBlock = (m_VertCols != NULL);
    IwSerialiseBool(bHaveColBlock);
    if (bHaveColBlock)
    {
        if (!m_VertCols) m_VertCols = new CIwModelBlockCols;
        m_VertCols->Serialise();
    }
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::Resolve()
{
    IW_CALLSTACK("CIwGeomModel::Resolve")

    CIwGeom::Resolve();
    IwResolveManagedHash(&m_Model, IW_GRAPHICS_RESTYPE_MODEL);
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::SetVertCols()
{
    IW_CALLSTACK("CIwGeomModel::SetVertCols")

    if (m_VertCols)
    {
        IwGraphicsSetFlags(IW_GRAPHICS_MODEL_NO_COL_STREAM_CLEAR_F);

        if (m_VertCols->m_RenderFn)
            m_VertCols->m_RenderFn(m_Model, m_VertCols, 0);
        else
            m_VertCols->Render(m_Model, 0);
    }
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::ClearVertCols()
{
    IW_CALLSTACK("CIwGeomModel::ClearVertCols")

    if (m_VertCols)
    {
        IwGraphicsClearFlags(IW_GRAPHICS_MODEL_NO_COL_STREAM_CLEAR_F);
        IwGetGxState()->m_Cols = 0;
        IwGetGxState()->m_NumCols = 0;
    }
}
//-----------------------------------------------------------------------------
void    CIwGeomModel::ParseCloseChild(CIwTextParserITX* pParser, CIwManaged* pChild)
{
    IW_CALLSTACK("CIwGeomModel::ParseCloseChild")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pChild->GetClassName(),"CIwModelBlockCols"))
    {
        IwAssertMsgN(322,ENGINE, !m_VertCols, ("m_VertCols already set"));
        m_VertCols = (CIwModelBlock*)pChild;
        IwAssertMsgN(323,ENGINE, !m_Model || m_VertCols->m_NumItems == m_Model->GetNumCols(), ("Mismatched colours in '%s': expected %d, found %d", DebugGetName(), m_Model->GetNumCols(), m_VertCols->m_NumItems));
    }
    else
    {
        IwAssertMsgN(324,ENGINE,0,("Invalid Child Class: %s",pChild->GetClassName()));
    }

#endif // #ifdef IW_BUILD_RESOURCES
}
//-----------------------------------------------------------------------------
CIwColour   CIwGeomModel::GetColourFromID(uint32 colID)
{
    if (!m_VertCols)
    {
        CIwColour col;
        col = 0xffffffff;
        return col;
    }

    return ((CIwModelBlockCols*)m_VertCols)->m_Cols[colID];
}
