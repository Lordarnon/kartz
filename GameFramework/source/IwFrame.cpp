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
 * Coordinate system, with local and world space matrices.
 */

// Includes
#include "IwFrame.h"

//-----------------------------------------------------------------------------
// CIwFrame
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwFrame);
IW_MANAGED_IMPLEMENT(CIwFrame)
IW_MANAGED_IMPLEMENT(CIwFrameLW)
CIwFrame::CIwFrame()
{
    IW_CALLSTACK("CIwFrame::CIwFrame")
    m_Flags  = 0;
}
//-----------------------------------------------------------------------------
CIwFrame::~CIwFrame()
{
    IW_CALLSTACK("CIwFrame::~CIwFrame")
}
//-----------------------------------------------------------------------------
void    CIwFrame::Init()
{
    IW_CALLSTACK("CIwFrame::Init")

    m_MatLocal  = CIwMat::g_Identity;
    m_Flags     = 0;
}
//-----------------------------------------------------------------------------
void    CIwFrame::Serialise()
{
    IW_CALLSTACK("CIwFrame::Serialise")

    // Superclass
    CIwManaged::Serialise();

    IwSerialiseUInt32(m_Flags);
    m_MatLocal.Serialise();
}

//-----------------------------------------------------------------------------
// CIwFrameLW
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwFrameLW);
CIwFrameLW::CIwFrameLW()
{
    IW_CALLSTACK("CIwFrameLW::CIwFrameLW")
    m_Parent = NULL;
}
//-----------------------------------------------------------------------------
CIwFrameLW::~CIwFrameLW()
{
    IW_CALLSTACK("CIwFrameLW::~CIwFrameLW")

    // Detach all children
    IW_MANAGED_LIST_ITERATE(m_Children, itC)
    {
        CIwFrameLW* pChild = (CIwFrameLW*)*itC;
        pChild->m_Parent = NULL;
    }

    // Detach ourself from parent
    if (m_Parent)
        m_Parent->RemoveChild(this);
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::Serialise()
{
    IW_CALLSTACK("CIwFrameLW::Serialise")

    // Superclass
    CIwFrame::Serialise();

//  m_MatWorld.Serialise(); // DO WE NEED THIS?
    m_Flags &= ~WORLD_MAT_VALID_F;

    // TODO
/*  m_Parent ?
    m_Children.SerialiseHeader();
    ArrayIt itC = m_Children.begin();
    ArrayIt etC = m_Children.end();
    for (; itC != etC; ++itC)
        IwSerialiseManagedObjectPtr(*itC);
*/
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::Resolve()
{
    IW_CALLSTACK("CIwFrameLW::Resolve")

    // TODO?
/*  ArrayIt itC = m_Children.begin();
    ArrayIt etC = m_Children.end();
    for (; itC != etC; ++itC)
    {
//      IwResolveManagedObjectPtr(*itC);
    }*/
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::Init()
{
    IW_CALLSTACK("CIwFrameLW::Init")

    CIwFrame::Init();
    m_MatWorld  = CIwMat::g_Identity;
    m_Parent    = NULL;
    DestroyChildren();
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::DestroyChildren()
{
    IW_CALLSTACK("CIwFrameLW::DestroyChildren")

    m_Children.Delete();
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::AddChild(CIwFrameLW* pChild)
{
    IW_CALLSTACK("CIwFrameLW::AddChild")

    IwAssert(GRAPHICS, pChild);
    IwAssert(GRAPHICS, pChild->m_Parent == NULL);
    pChild->m_Parent = this;
    IwAssertMsgN(317,GRAPHICS, !m_Children.Contains(pChild), ("Adding child twice to frame"));
    m_Children.Add(pChild);
    pChild->InvalidateMatWorld();
    pChild->UpdateWorldFlags();
}
//-----------------------------------------------------------------------------
void    _FindChildPtr(CIwFrameLW pFrame, CIwFrameLW** ppChild)
{
    IW_CALLSTACK("_FindChildPtr")

    // TODO
}
//-----------------------------------------------------------------------------
bool    CIwFrameLW::RemoveChild(CIwFrameLW* pChild)
{
    IW_CALLSTACK("CIwFrameLW::RemoveChild")

    IwAssert(GRAPHICS, pChild);
    IwAssert(GRAPHICS, pChild != this);

    // NOTE: RemoveFast does NOT preserve order of children
    bool result = m_Children.RemoveFast(pChild);
    if (result)
        pChild->m_Parent = NULL;

    return result;
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::Update()
{
    IW_CALLSTACK("CIwFrameLW::Update")

    // We update our world matrix from local matrix and parents world matrix (if necessary)
    if (IsMatWorldValid())
        return;

/*  if (m_Parent)
    {
        m_Parent->Update();
        if (m_Parent->m_Flags & WORLD_ROT_IDENTITY_F) {
            IwAssert(GRAPHICS, m_Parent->m_MatWorld.IsRotIdentity());
            m_MatWorld.CopyRot(m_MatLocal);                         //could get rid of this if we wanted
            m_MatWorld.t = m_Parent->m_MatWorld.t + m_MatLocal.t;   //quicker to do the addition then to check flags
        }
        else {
            if (m_Flags & LOCAL_ROT_IDENTITY_F) {
                IwAssert(GRAPHICS, m_MatLocal.IsRotIdentity());
                m_MatWorld.CopyRot(m_Parent->m_MatWorld);
                m_MatWorld.t = m_Parent->m_MatWorld.TransformVec(m_MatLocal.t); //no point checking for identity trans I don't think
            }
            else
                m_MatWorld = m_MatLocal * m_Parent->m_MatWorld;
        }
    }
    else
        m_MatWorld = m_MatLocal;
*/

    // TIM: THIS SEEMS THE WRONG WAY ROUND TO ME. SHOULDN'T WE BE WORKING OUR WAY DOWN THE TREE, NOT BACK UP?
    // E.G. AS FOLLOWS...
    // TODO: RESPECT FLAGS
    CIwFrame* pParent = m_Parent;
    if (!pParent)
    {
        m_MatWorld.CopyRot(m_MatLocal);
        m_MatWorld.t = m_MatLocal.t;
    }
    else
    {
        pParent->UpdateWorld();
        const CIwMat* pMatP = pParent->GetMatWorld();
        m_MatWorld.CopyRot(m_MatLocal * *pMatP);
//      m_MatWorld.CopyRot(*pMatP * m_MatLocal);
        m_MatWorld.t = pMatP->TransformVec(m_MatLocal.t);
    }

    // World matrix is now valid
    m_Flags |= WORLD_MAT_VALID_F;

    // Recurse children
    IW_MANAGED_LIST_ITERATE(m_Children, itC)
    {
        CIwFrameLW* pChild = (CIwFrameLW*)*itC;
        pChild->Update();
    }
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::CheckLocalFlags()
{
    IW_CALLSTACK("CIwFrameLW::CheckLocalFlags")

    if  (m_MatLocal.IsRotIdentity())
        m_Flags |= LOCAL_ROT_IDENTITY_F;
    else
        m_Flags &= ~LOCAL_ROT_IDENTITY_F;
}
//-----------------------------------------------------------------------------
void    CIwFrameLW::UpdateWorldFlags()
{
    IW_CALLSTACK("CIwFrameLW::UpdateWorldFlags")

    //propogate rotation identity information down the tree
    uint32 oldFlags = m_Flags;
    if (m_Parent)
    {
        if  (
            (m_Flags & LOCAL_ROT_IDENTITY_F) &&
            (m_Parent->m_Flags & WORLD_ROT_IDENTITY_F)
            )
            m_Flags |= WORLD_ROT_IDENTITY_F;
        else
            m_Flags &= ~WORLD_ROT_IDENTITY_F;
    }
    else
    {
        if (m_Flags & LOCAL_ROT_IDENTITY_F)
            m_Flags |= WORLD_ROT_IDENTITY_F;
        else
            m_Flags &= ~WORLD_ROT_IDENTITY_F;
    }
    if (oldFlags != m_Flags)
    {
        IW_MANAGED_LIST_ITERATE(m_Children, itC)
        {
            CIwFrameLW* pChild = (CIwFrameLW*)*itC;
            pChild->UpdateWorldFlags();
        }
    }
}
//-----------------------------------------------------------------------------
void CIwFrameLW::InvalidateMatWorld()
{
    IW_CALLSTACK("CIwFrameLW::InvalidateMatWorld")

    if (!(m_Flags & WORLD_MAT_VALID_F)) //already invalid
        return;

    m_Flags &= ~WORLD_MAT_VALID_F;
    IW_MANAGED_LIST_ITERATE(m_Children, itC)
    {
        CIwFrameLW* pChild = (CIwFrameLW*)*itC;
        pChild->InvalidateMatWorld(); //invalidate for all children that think they have a valid matrix
    }
}
//-----------------------------------------------------------------------------
const CIwMat* CIwFrameLW::GetMatWorld()
{
    IW_CALLSTACK("CIwFrameLW::GetMatWorld")

    Update();

    return &m_MatWorld;
}
