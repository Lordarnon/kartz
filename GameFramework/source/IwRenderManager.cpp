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

// Includes
#include "IwRenderManager.h"
#include "IwCamera.h"
#include "IwGxState.h"
#include "IwRenderSlots.h"
#include "IwDebug.h"

/** Singleton */
IW_SINGLETON_INSTANTIATE(RenderManager);

#ifdef IW_DEBUG
const char* CIwRenderManager::g_DebugFlagNames[] =
{
    "DEBUG_RENDER_MODEL_EDGES_F",
    "DEBUG_RENDER_MODEL_NO_FILL_F",
    NULL,
};
#endif

//-----------------------------------------------------------------------------
// CIwRenderManager
//-----------------------------------------------------------------------------
CIwRenderManager::CIwRenderManager()
{
    IW_CALLSTACK("CIwRenderManager::CIwRenderManager")
    IW_SINGLETON_SET(RenderManager);

    // Ptrs
    m_Camera    = NULL;
    m_World     = NULL;

    // Interpolation
    m_TimeFraction  = 0;

#ifdef IW_DEBUG
    m_DebugFlags    = 0;
    m_DebugTextNext = m_DebugText;
#endif
}
//-----------------------------------------------------------------------------
CIwRenderManager::~CIwRenderManager()
{
    IW_CALLSTACK("CIwRenderManager::~CIwRenderManager")

    DestroySlots();
    IW_SINGLETON_CLEAR(RenderManager);
}
//-----------------------------------------------------------------------------
void    CIwRenderManager::AddSlot(CIwRenderSlot* pSlot)
{
    IW_CALLSTACK("CIwRenderManager::AddSlot")

    m_Slots.Add(pSlot);
}
//-----------------------------------------------------------------------------
void    CIwRenderManager::Render()
{
    IW_CALLSTACK("CIwRenderManager::Render")

    // Render user slots
    RenderSlotList(m_Slots);
}
//-----------------------------------------------------------------------------
void    CIwRenderManager::RenderSlotList(const CIwManagedList& list)
{
    IW_CALLSTACK("CIwRenderManager::RenderSlotList")

    IW_MANAGED_LIST_ITERATE(list, itS)
    {
        CIwRenderSlot*  pSlot = (CIwRenderSlot*)*itS;
        if (pSlot->GetActive() == true)
        {
//          IW_PROFILE(pSlot->GetClassName());
//IwTrace(ENGINE, ("pSlot->Render"));
            pSlot->Render();
        }
    }
}

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwRenderManager::DebugRenderText()
{
    IW_CALLSTACK("CIwRenderManager::DebugRenderText")

    // USED TO BREAK DEVICE BUILD... OK NOW?
/*  IW_CALLSTACK("CIwRenderManager::DebugRenderText")

    CIwDebugText*   pDT = m_DebugText;
    while (pDT < m_DebugTextNext)
    {
        if ((int16)pDT->m_Pos.z == (int16)IW_RENDERMANAGER_DEBUG_TEXT_2D)
            CIwMenuManager::Get().m_TextCallback(pDT->m_Text.c_str(), pDT->m_Pos.x, pDT->m_Pos.y, (CIwMenuItem::ColourID)pDT->m_ColID);
        else
        {
            CIwVec3 vv = IwGetGxState()->m_MatViewWorld.TransformVec(pDT->m_Pos);
            if  (
                ((vv.z >> GX_VIEWSPACE_SHIFT) > m_Camera->m_NearClip) &&
                ((vv.z >> GX_VIEWSPACE_SHIFT) < m_Camera->m_FarClip)
                )
            {
                int16   sx, sy;
                IwViewToScreenXYSubPixel(sx, sy, vv, m_Camera->m_PerspMul);
                CIwMenuManager::Get().m_TextCallback(pDT->m_Text.c_str(), sx >> 3, sy >> 3, (CIwMenuItem::ColourID)pDT->m_ColID);
            }
        }
        pDT++;
    }*/
}
//-----------------------------------------------------------------------------
void    CIwRenderManager::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwRenderManager::DebugAddMenuItems")

    pMenu->AddItem(new CIwMenuItemManagedList("Slots", &m_Slots));
    pMenu->AddItem(new CIwMenuItemEditUInt8("LightColAmbient.r", &IwGetGxState()->m_LightColAmbient.r));
    pMenu->AddItem(new CIwMenuItemEditUInt8("LightColDiffuse.r", &IwGetGxState()->m_LightColDiffuse.r));
    pMenu->AddItem(new CIwMenuItemEditInt16("m_LightDirnDiffuse.x", &IwGetGxState()->m_LightDirnDiffuse.x));
    pMenu->AddItem(new CIwMenuItemEditInt16("m_LightDirnDiffuse.y", &IwGetGxState()->m_LightDirnDiffuse.y));
    pMenu->AddItem(new CIwMenuItemEditInt16("m_LightDirnDiffuse.z", &IwGetGxState()->m_LightDirnDiffuse.z));
}
#endif // IW_DEBUG
