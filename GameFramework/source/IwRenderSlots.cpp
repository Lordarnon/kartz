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
 * Default renderslots to add to rendermanager.
 */

#include "IwRenderSlots.h"
#include "IwCamera.h"
#include "IwFrame.h"
#include "IwGame.h"
#include "IwGameState.h"
#include "IwGeomClass.h"
#include "IwGraphics.h"
#include "IwGx.h"
#include "IwGxError.h"
#include "IwGxTransformSW.h"
#include "IwRenderManager.h"
#include "s3eGL.h"
#include "IwDebug.h"
#include "IwWorld.h"

/*#ifdef IW_DEBUG
#include "IwAnimMetrics.h"
#include "IwGraphicsMetrics.h"
#include "IwGxMetrics.h"
#endif
*/
#ifdef IW_GX_SUPPORT_SW_RASTERISATION
#include "IwGxRenderSW.h"
#include "IwRenderer.h"
#endif

IW_MANAGED_IMPLEMENT(CIwRenderSlot)
IW_MANAGED_IMPLEMENT(CIwRenderSlotClear)
IW_MANAGED_IMPLEMENT(CIwRenderSlotBegin)
IW_MANAGED_IMPLEMENT(CIwRenderSlotEnd)
IW_MANAGED_IMPLEMENT(CIwRenderSlotGeoms)
IW_MANAGED_IMPLEMENT(CIwRenderSlotFlush)
IW_MANAGED_IMPLEMENT(CIwRenderSlotGameStates)

//-----------------------------------------------------------------------------
// CIwRenderSlot
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwRenderSlot::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwRenderSlot::DebugAddMenuItems")

    pMenu->AddItem(new CIwMenuItemEditBool("Active", &m_Active));
}
#endif

//-----------------------------------------------------------------------------
// CIwRenderSlotClear
//-----------------------------------------------------------------------------
void    CIwRenderSlotClear::Render()
{
    IW_CALLSTACK("CIwRenderSlotClear::Render")

    // Clear backbuffer
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
}

//-----------------------------------------------------------------------------
// CIwRenderSlotBegin
//-----------------------------------------------------------------------------
void    CIwRenderSlotBegin::Render()
{
    IW_CALLSTACK("CIwRenderSlotBegin::Render")

    // Set view matrix from current world camera
    CIwCamera* pCam = IwGetRenderManager()->GetCamera();
    if  (
        (pCam) &&
        (pCam->GetFrame())
        )
        IwGxSetViewMatrix(IwGetRenderManager()->GetCamera()->GetMatLocal());

    // Perspective
    if (pCam)
    {
        IwGxSetPerspMul(pCam->m_PerspMul);
        IwGxSetFarNear(pCam->m_FarZ, pCam->m_NearZ);
    }

#ifdef IW_DEBUG
//  IwGetRenderManager()->m_DebugTextNext = IwGetRenderManager()->m_DebugText;
#endif
}

//-----------------------------------------------------------------------------
// CIwRenderSlotEnd
//-----------------------------------------------------------------------------
void    CIwRenderSlotEnd::Render()
{
    IW_CALLSTACK("CIwRenderSlotEnd::Render")

    // Render debug text
#ifdef IW_DEBUG
//IwTrace(ENGINE, ("DebugText"));
    bool fog = (IwGxGetFlags() & IW_GX_FOG_F) != 0;
    if (fog == true)
        IwGxFogOff();

    CIwCamera* pCam = IwGetRenderManager()->GetCamera();
    if (pCam)
    {
        CIwDebugText* pDT = IwGetRenderManager()->m_DebugText;
        while (pDT < IwGetRenderManager()->m_DebugTextNext)
        {
            if ((int16)pDT->m_Pos.z == (int16)IW_RENDERMANAGER_DEBUG_TEXT_2D)
                IwGetMenuManager()->m_TextCallback(pDT->m_Text.c_str(), pDT->m_Pos.x, pDT->m_Pos.y, (CIwMenuItem::ColourID)pDT->m_ColID);
            else
            {
                // Get screenspace coords for lines
                CIwVec3 v0          = IwGetGxState()->m_MatClipViewWorld.TransformVecSafe(CIwVec3(pDT->m_Pos));
                uint32  clipFlags   = IwGxClipVec(v0);
                if (!clipFlags)
                {
                    int16   sx, sy;
                    IwGxViewToScreenXYSubPixel(sx, sy, v0);
                    IwGetMenuManager()->m_TextCallback(pDT->m_Text.c_str(), sx >> 3, sy >> 3, (CIwMenuItem::ColourID)pDT->m_ColID);
                }
            }
            pDT++;
        }
    }
    // Clear debug text
    IwGetRenderManager()->m_DebugTextNext = IwGetRenderManager()->m_DebugText;

    // Menu manager
    if (IwGetMenuManager())
        IwGetMenuManager()->Render();

    if (fog == true)
        IwGxFogOn();
#endif

    // Do appropriate flip-screen, for SW or HW
    IwGxSwapBuffers();

    // NOTE: USER MUST RESET METRICS
}

//-----------------------------------------------------------------------------
// CIwRenderSlotGeoms
//-----------------------------------------------------------------------------
void    CIwRenderSlotGeoms::Render()
{
    IW_CALLSTACK("CIwRenderSlotGeoms::Render")

    // Render all entities

    // Do debug rendering
#ifdef IW_DEBUG
    if (IwGetRenderManager()->GetWorld())
        IwGetRenderManager()->GetWorld()->DebugRender();

    if  (
        (IwGetGame()) &&
        (IwGetGame()->GetState())
        )
        IwGetGame()->GetState()->DebugRender();
#endif
}

//-----------------------------------------------------------------------------
// CIwRenderSlotFlush
//-----------------------------------------------------------------------------
void    CIwRenderSlotFlush::Render()
{
    IW_CALLSTACK("CIwRenderSlotFlush::Render")

    // Draw all cached geometry
    IwGxFlush();
}

//-----------------------------------------------------------------------------
// CIwRenderSlotGameStates
//-----------------------------------------------------------------------------
void    CIwRenderSlotGameStates::Render()
{
    IW_CALLSTACK("CIwRenderSlotGameStates::Render")

    IwGetGame()->RenderStates();
}
