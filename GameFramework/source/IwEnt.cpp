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
 * Base class for entity with behaviours.
 */
#include "IwEnt.h"
#include "IwClassFactory.h"
#include "IwEvent.h"
#include "IwEngineEvent.h"
#include "IwBehaviour.h"
#include "IwDebugPrim.h"
#include "IwFrame.h"
#include "IwGeomQuat.h"
#include "IwRenderManager.h"
#include "IwResource.h"
#include "IwTextParserITX.h"

//-----------------------------------------------------------------------------
// CIwEnt
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwEnt)
CIwEnt::CIwEnt()
{
    IW_CALLSTACK("CIwEnt::CIwEnt")

    m_Flags     = UPDATE_F;
    m_Behaviour = NULL;
    m_Frame     = NULL;
}
//-----------------------------------------------------------------------------
CIwEnt::~CIwEnt()
{
    IW_CALLSTACK("CIwEnt::~CIwEnt")
    IwAssertMsgN(305,ENGINE, !(m_Flags & IN_WORLD_F), ("Illegal to delete a CIwEnt whilst it is still in a CIwWorld"));

    ClearBehaviour();

    if  (
        (m_Frame) &&
        (IsFlag(FRAME_OWNED_F))
        )
        delete m_Frame;
}

//-----------------------------------------------------------------------------
void    CIwEnt::Serialise()
{
    IW_CALLSTACK("CIwEnt::Serialise")

    // Superclass
    CIwManaged::Serialise();

    IwSerialiseUInt32(m_Flags);
    m_BSphere.Serialise();

    // Serialise frame?
    if  (
        (m_Flags & FRAME_OWNED_F) &&
        (m_Flags & SERIALISE_FRAME_F)
        )
    {
        if (g_IwSerialiseContext.read == true)
        {
            // Read classname and create class
            uint32 hash;
            IwSerialiseUInt32(hash);
            delete m_Frame;
            m_Frame = (CIwFrame*)IwClassFactoryCreate(hash);
            IwAssertMsgN(306,ENGINE, m_Frame, ("Failed to create frame from hash - must register factory for (e.g.) CIwFrame"));
        }
        else
        {
            // Write classname
            IwAssertMsgN(307,ENGINE, m_Frame, ("No frame to serialise"));
            uint32 hash = IwHashString(m_Frame->GetClassName());
            IwSerialiseUInt32(hash);
        }
        m_Frame->Serialise();
        m_Frame->Update();
    }

    // Serialise behaviour?
    if  (
        (m_Flags & BEHAVIOUR_OWNED_F) &&
        (m_Flags & SERIALISE_BEHAVIOUR_F)
        )
    {
        if (g_IwSerialiseContext.read == true)
        {
            // Read classname and create class
            uint32 hash;
            IwSerialiseUInt32(hash);
            CIwBehaviour* pBe = (CIwBehaviour*)IwClassFactoryCreate(hash);
            IwAssertMsgN(308,ENGINE, pBe != NULL, ("Failed to create class from hash"));
            SetBehaviour(pBe);
        }
        else
        {
            // Write classname
            IwAssertMsgN(309,ENGINE, m_Behaviour, ("No behaviour to serialise"));
            uint32 hash = IwHashString(m_Behaviour->GetClassName());
            IwSerialiseUInt32(hash);
        }
        m_Behaviour->Serialise();
    }
}
//-----------------------------------------------------------------------------
void    CIwEnt::Resolve()
{
    IW_CALLSTACK("CIwEnt::Resolve")

    // Resolve frame?
    if  (
        (m_Flags & FRAME_OWNED_F) &&
        (m_Flags & SERIALISE_FRAME_F) &&
        (m_Frame)
        )
        m_Frame->Resolve();

    // Resolve behaviour?
    if  (
        (m_Flags & BEHAVIOUR_OWNED_F) &&
        (m_Flags & SERIALISE_BEHAVIOUR_F) &&
        (m_Behaviour)
        )
    {
        m_Behaviour->Resolve();
        m_Behaviour->SetEnt(this);
    }
}
//-----------------------------------------------------------------------------
/** Set behaviour, mark as owned by Ent */
void    CIwEnt::SetBehaviour(CIwBehaviour* pBe)
{
    IW_CALLSTACK("CIwEnt::SetBehaviour")

    IwAssertMsgN(310,ENGINE, pBe, ("NULL behaviour: use ClearBehaviour?"));

    ClearBehaviour();
    pBe->SetEnt(this);
    m_Behaviour = pBe;
    m_Flags |= BEHAVIOUR_OWNED_F;
}
//-----------------------------------------------------------------------------
/** Set behaviour, mark as not owned by Ent */
void    CIwEnt::SetBehaviourShared(CIwBehaviour* pBe)
{
    IW_CALLSTACK("CIwEnt::SetBehaviourShared")

    IwAssertMsgN(311,ENGINE, pBe, ("NULL behaviour: use ClearBehaviour?"));

    ClearBehaviour();
    pBe->SetEnt(this);
    m_Behaviour = pBe;
    m_Flags &= ~BEHAVIOUR_OWNED_F;
}
//-----------------------------------------------------------------------------
void    CIwEnt::ClearBehaviour()
{
    IW_CALLSTACK("CIwEnt::ClearBehaviour")

    if (m_Behaviour)
    {
        m_Behaviour->SetEnt(NULL);

        if (m_Flags & BEHAVIOUR_OWNED_F)
        {
            delete m_Behaviour;
            m_Flags &= ~BEHAVIOUR_OWNED_F;
        }

        m_Behaviour = NULL;
    }
}
//-----------------------------------------------------------------------------
void    CIwEnt::Init()
{
    IW_CALLSTACK("CIwEnt::Init")

    // Behaviours
    if (m_Behaviour)
        m_Behaviour->Init();
}
//-----------------------------------------------------------------------------
/*void  CIwEnt::Clone(const CIwEnt& src)
{
    IW_CALLSTACK("CIwEnt::Clone")

    CIwManaged::Clone(*(CIwManaged*)&src);

    m_BSphere   = src.m_BSphere;
    m_Flags     = src.m_Flags;
    m_Frame     = src.m_Frame;
    m_Behaviour = src.m_Behaviour;
}*/
//-----------------------------------------------------------------------------
void    CIwEnt::Update()
{
    IW_CALLSTACK("CIwEnt::Update")

    // Behaviours
    if  (
        (m_Behaviour) &&
        (m_Behaviour->IsActive())
        )
    {
        IW_PROFILE(m_Behaviour->GetClassName());
        m_Behaviour->Update();
    }
}
//--------------------------------------------------------------------------------
bool    CIwEnt::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{

    IW_CALLSTACK("CIwEnt::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (!strcmp(pAttrName, "pos"))
    {
        IwAssertMsgN(312,ENGINE, m_Frame, ("pos requires m_Frame"));
        CIwMat* pMat = m_Frame->ExposeMatLocal();
        pParser->ReadInt32Array(&pMat->t.x, 3);
        m_Frame->InvalidateMatWorld();
    }
    else
    if (!strcmp(pAttrName, "rot"))
    {
        IwAssertMsgN(313,ENGINE, m_Frame, ("rot requires m_Frame"));
//      pParser->ReadRotation(m_Frame->ExposeMatLocal());
        float   f[4];
        pParser->ReadFloatArray(f, 4);
        CIwQuat q;
        q.s     = (int32)(f[0] * (1 << 30));
        q.x = (int32)(f[1] * (1 << 30));
        q.y = (int32)(f[2] * (1 << 30));
        q.z = (int32)(f[3] * (1 << 30));
        m_Frame->ExposeMatLocal()->CopyRot(q);
    }
    else
        return CIwManaged::ParseAttribute(pParser, pAttrName);

#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//--------------------------------------------------------------------------------
void    CIwEnt::ParseClose(CIwTextParserITX* pParser)
{
    IW_CALLSTACK("CIwEnt::ParseClose")

#ifdef IW_BUILD_RESOURCES

    IwAssertMsgN(314,ENGINE, false, ("Don't know what to do with CIwEnt base type"));

#endif // #ifdef IW_BUILD_RESOURCES
}
//-----------------------------------------------------------------------------
void    CIwEnt::OnDeath()
{
    IW_CALLSTACK("CIwEnt::OnDeath")

    if (m_Behaviour && (m_Flags & BEHAVIOUR_OWNED_F))
        m_Behaviour->OnDeath();
}
//-----------------------------------------------------------------------------
void    CIwEnt::SetFrameOwned(CIwFrame* pFrame)
{
    IW_CALLSTACK("CIwEnt::SetFrameOwned")

    IwAssertMsgN(315,ENGINE, pFrame, ("Null ptr"));
    if (m_Flags & FRAME_OWNED_F)
        delete m_Frame;

    m_Frame = pFrame;
    m_Flags |= FRAME_OWNED_F;
}
//-----------------------------------------------------------------------------
void    CIwEnt::SetFrameShared(CIwFrame* pFrame)
{
    IW_CALLSTACK("CIwEnt::SetFrameShared")

    if (m_Flags & FRAME_OWNED_F)
        delete m_Frame;

    IwAssertMsgN(316,ENGINE, pFrame, ("Null ptr"));
    m_Frame = pFrame;
    m_Flags &= ~FRAME_OWNED_F;
}
//-----------------------------------------------------------------------------
void    CIwEnt::UpdateList(CIwManagedList& list)
{
    IW_CALLSTACK("CIwEnt::UpdateList")

    for (uint32 i = 0; i < list.GetSize();)
    {
        CIwEnt* pEnt = (CIwEnt*)list[i];
        if  (
            (!pEnt->IsDying()) &&
            (pEnt->GetFlags() & UPDATE_F)
            )
            pEnt->Update();

        if (pEnt->IsDying())
        {
            // Override OnDeath() to remove from any convenience lists, etc.
            pEnt->OnDeath();

            // Physically delete the object
            pEnt->m_Flags &= ~IN_WORLD_F;
            delete pEnt;

            // Remove from this list
            i = list.EraseFast(i);

            // Do not increase i
            continue;
        }
        i++;
    }
}
//-----------------------------------------------------------------------------
void    CIwEnt::InitList(CIwManagedList& list)
{
    IW_CALLSTACK("CIwEnt::InitList")

    // Initing entities may cause more entities to be added to the iterated
    // list... causing the iterator to become invalidated. Using indices avoids
    // this problem
    const int num = list.GetSize();
    for (int i = 0; i < num; i++)
    {
        CIwEnt* pEnt = IwSafeCast<CIwEnt*>(list[i]);
        pEnt->Init();
    }
}
//-----------------------------------------------------------------------------
void    CIwEnt::DeleteList(CIwManagedList& list)
{
    IW_CALLSTACK("CIwEnt::DeleteList")

    for (uint32 i = 0; i < list.GetSize(); i++)
    {
        ((CIwEnt*)list[i])->m_Flags &= ~IN_WORLD_F;
        delete list[i];
    }
}

bool CIwEnt::HandleEvent(CIwEvent* pEvent)
{
    switch (pEvent->GetID())
    {
    case Event_EntSetPos:
        ExposeMatLocal()->t = ((CIwEventEntSetPos*)pEvent)->m_Pos;
        break;

    case Event_EntOffset:
        ExposeMatLocal()->t += ((CIwEventEntOffset*)pEvent)->m_Off;
        break;

    case Event_EntSetRotY:
        ExposeMatLocal()->SetRotY(((CIwEventEntSetRotY*)pEvent)->m_Rot, false);
        break;

    default:
        return CIwManaged::HandleEvent(pEvent);
    }

    return true;
}

#ifdef IW_DEBUG
//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
void    CIwEnt::DebugRender()
{
    IW_CALLSTACK("CIwEnt::DebugRender")

    if (m_Frame)
    {
#ifdef IW_DEBUG
        if (m_Flags & DEBUG_RENDER_NAME_F)
        {
            const CIwMat* pMat = m_Frame->GetMatWorld();
            IwGetRenderManager()->DebugAddText(DebugGetName(), CIwSVec3(pMat->t), CIwMenuItem::TITLE);
        }
#endif
        if (m_Flags & DEBUG_RENDER_POS_F)
        {
            const CIwMat* pMat = m_Frame->GetMatWorld();
            IwGxDebugPrimCross(pMat->t, 4, IwGxGetColFixed(IW_GX_COLOUR_GREEN));
            IwGxDebugPrimAxes(*pMat, 100);
        }

        if (m_Flags & DEBUG_RENDER_BSPHERE_F)
        {
            const CIwMat* pMat = m_Frame->GetMatWorld();
            CIwSphere s;
            s.t = pMat->TransformVec(m_BSphere.t);
            s.SetRadius(m_BSphere.r);
            IwGxDebugPrimSphere(s, IwGxGetColFixed(IW_GX_COLOUR_RED));
        }
    }

    // Call DebugRender() on all behaviours
    if (m_Behaviour)
        m_Behaviour->DebugRender();
}
//-----------------------------------------------------------------------------
void    CIwEnt::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwEnt::DebugAddMenuItems")

    // Add items to debug menu
    CIwManaged::DebugAddMenuItems(pMenu);

    // Behaviours
    if (m_Behaviour)
        pMenu->AddItem(new CIwMenuItemManaged(NULL, m_Behaviour));

    // Other
    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_NAME_F",      &m_Flags, DEBUG_RENDER_NAME_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_POS_F",       &m_Flags, DEBUG_RENDER_POS_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("DEBUG_RENDER_BSPHERE_F",   &m_Flags, DEBUG_RENDER_BSPHERE_F));
}
#endif // IW_DEBUG
