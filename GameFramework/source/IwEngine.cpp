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
#include "IwEngine.h"
#include "IwClassFactory.h"
#include "IwMenu.h"
#include "IwRenderManager.h"
#include "IwRuntime.h"

// For class factories
#include "IwCamera.h"
#include "IwGeomClass.h"
#include "IwGeomModel.h"
#include "IwPathLine.h"
#include "IwPathBezier.h"
#include "IwPathCircle.h"
#include "IwWorld.h"
#include "IwVolumeBBox.h"
#include "IwVolumeVCylinder.h"

//-----------------------------------------------------------------------------
static int32 s_Initialised = 0;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void    IwEngineInit()
{
    IW_CALLSTACK("IwEngineInit")

    if (s_Initialised++ > 0)
        return;

    // Register classes
    IW_CLASS_REGISTER(CIwFrame);
    IW_CLASS_REGISTER(CIwFrameLW);

    IW_CLASS_REGISTER(CIwWorld);
    IW_CLASS_REGISTER(CIwPathLine);
    IW_CLASS_REGISTER(CIwPathBezier);
    IW_CLASS_REGISTER(CIwPathCircle);
    IW_CLASS_REGISTER(CIwPathNurbs);
    IW_CLASS_REGISTER(CIwCamera);
    IW_CLASS_REGISTER(CIwGeomModel);

    IW_CLASS_REGISTER(CIwPoint);
    IW_CLASS_REGISTER(CIwVolumeBBox);
    IW_CLASS_REGISTER(CIwVolumeVCylinder);
}
//-----------------------------------------------------------------------------
void    IwEngineTerminate()
{
    IW_CALLSTACK("IwEngineTerminate")

    if (--s_Initialised != 0)
    {
        IwAssertMsg(GX, s_Initialised >= 0, ("IwEngineTerminate called more often than IwEngineInit"));
        return;
    }
}

//-----------------------------------------------------------------------------
// CIwMenuItemEngine
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwMenuItemEngine::Select()
{
    IW_CALLSTACK("CIwMenuItemEngine::Select")

    // Create debug menu
    CIwMenu* pMenu = new CIwMenu;
    pMenu->AddItem(new CIwMenuItemTitle("IwEngine"));

    IwGetRenderManager()->DebugAddMenuItems(pMenu);

    pMenu->SetItemID(1);
    IwGetMenuManager()->AddMenu(pMenu);
}
#endif
