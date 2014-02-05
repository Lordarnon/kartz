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
#include "Game.h"

#include "IwGraphics.h"
#include "IwGxMotionBlur.h"
#include "IwResManager.h"
#include "IwResHandlerGEO.h"
#include "IwResHandlerGROUP.h"
#include "IwResHandlerWORLD.h"
#include "IwTextParserITX.h"
#include "IwGxPrint.h"
#include "IwModelBuilder.h"
#include "IwEngine.h"
#include "IwRenderManager.h"
#include "IwProfileMenu.h"
#include "IwFile.h"
#include "IwSound.h"

#include "MemoryManager.h"
#include "FrontendState.h"
#include "World.h"
#include "derbh.h"

CGame* CGame::m_Instance = NULL;
int g_UseDerbh = 0;


//--------------------------------------------------------------------------------

class CIwMenuItemCheatCamera : public CIwMenuItem
{
public:
    CIwMenuItemCheatCamera(CWorld* pWorld) : CIwMenuItem("", "SetFreeFlightCamera", 0, CIwMenuItem::MENU)
    {
        m_pWorld = pWorld;
    }

    void Select() { m_pWorld->ToggleFlightCamera(); }

private:
    CWorld*     m_pWorld;
};

//--------------------------------------------------------------------------------

class CIwMenuItemCheats : public CIwMenuItem
{
public:
    CIwMenuItemCheats() : CIwMenuItem("", "Cheats", 0, CIwMenuItem::MENU) {};
    void Select();
};

void    CIwMenuItemCheats::Select()
{
    IW_CALLSTACK("CIwMenuItemCheats::Select")

    // Create debug menu
    CIwMenu* pMenu = new CIwMenu;
    pMenu->AddItem( new CIwMenuItemTitle("Cheats") );
    pMenu->AddItem( new CIwMenuItemCheatCamera( &GetWorld() ) );
    pMenu->SetItemID( 1 );

    IwGetMenuManager()->AddMenu(pMenu);
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------

#ifdef IW_USE_PROFILE

    class CIwMenuItemProfile : public CIwMenuItem
    {
    public:
        CIwMenuItemProfile() : CIwMenuItem("", "SysProfile", 0, CIwMenuItem::MENU) {};
        void Select();
    };

    void CIwMenuItemProfile::Select()
    {
        IW_CALLSTACK("CIwMenuItemProfile::Select")

        // Create debug menu
        new CIwProfileMenu();

        CIwProfileManager::Get().SetActive(true);
    }

#endif // IW_USE_PROFILE

//--------------------------------------------------------------------------------

// main menu function callback that defines the structure of the main debug menu
CIwMenu* DebugCreateMainMenu()
{
    IW_CALLSTACK("DebugCreateMainMenu")

    CIwMenu* pMenu = new CIwMenu;

    #ifdef IW_DEBUG
        pMenu->AddItem( new CIwMenuItemResManager );
        pMenu->AddItem( new CIwMenuItemGx );
        pMenu->AddItem( new CIwMenuItemGraphics );
    #endif

        pMenu->AddItem( new CIwMenuItemCheats );

    #ifdef IW_USE_PROFILE
        pMenu->AddItem(new CIwMenuItemProfile);
    #endif

    return pMenu;
}

//--------------------------------------------------------------------------------

CGame::CGame()
:   m_QueuedPop(false)
{
    IW_CALLSTACK("CGame::CGame");

    IwAssert(RCD, !m_Instance);
    m_Instance = this;

    CTempBucketSwitch b(MB_GX);

    IwSoundInit();

    IwGxInit();

    //IwGraphicsInit() also initialises ResManager
    IwGraphicsInit();


    IwGxSetColClear(128, 128, 255, 255);

#ifdef IW_BUILD_RESOURCES
    IwGetResManager()->AddHandler(new CIwResHandlerWORLD);
    IwGetResManager()->AddHandler(new CIwResHandlerWAV);
#endif

    IwGxMotionBlurInit();
    IwGxMotionBlurSetType(IW_GX_MOTION_BLUR_5050);

    //gx print allocs a small amount of memory the first time it's called (in GL)
    IwGxPrintString(0, 0, "", false);

    IwTrace(RCD, ("Gx memory used %d", MemoryManagerGetUsed(MB_GX)));

    IwAssert(RCD, WORLD_SCALE != 0);

    IwGetGameClock()->InitFramerate(FPS);

    new CIwMenuManager;

    // Rendering and main menu construction callbacks
    IwGetMenuManager()->SetTextCallback(IwGxPrintMenuCallback);
    IwGetMenuManager()->SetMainMenuFn(DebugCreateMainMenu);

#ifdef IW_USE_PROFILE
    new CIwProfileManager();
    CIwProfileManager::Get().SetActive(true);
#endif

    s3eConfigGetInt("Game", "LoadFromDerbh", &g_UseDerbh);

    // If we don't have any local source files we should always try derbh
    if (!s3eFileCheckExists("ui/ui.group") )
        g_UseDerbh = 1;

    // If we don't have any local binary files we should always try derbh
    if (!g_UseDerbh && IwGetResManager()->GetMode() == CIwResManager::MODE_LOAD )
    {

        //IwFileOpenPrefixed is aware of the current build style
        s3eFile* pTest = IwFileOpenPrefixed("ui/ui.group.bin", "rb");
        if (pTest)
        {
            s3eFileClose(pTest);
        }
        else
            g_UseDerbh = 1;
    }

    if (g_UseDerbh)
    {
        IwAssertMsg(RCD, IwGetResManager()->GetMode() != CIwResManager::MODE_BUILD, ("Trying to run from derbh and build resources will result in unexpected behaviour: resource binaries will be build, but derbh versions will be loaded."));
        char buildStyle[128];
        if (s3eConfigGetString("RESMANAGER", "ResBuildStyle", buildStyle) == S3E_RESULT_SUCCESS )
        {
            char buffer[256];
            sprintf(buffer, "RacingResources-%s.dz", buildStyle);
            if (!s3eFileCheckExists(buffer) )
                IwError(("Resource file %s does not exist.", buffer));
            dzArchiveAttach(buffer);
        }
        else
        {
            IwError(("Build Style not set..."));
        }
    }

    // If we are being told to build resources by Studio Director IwResManager::BuildResources will
    // generate the required binaries and return true. So we setup all our class factories and then maybe exit...
    RegisterClassFactories();
    if (IwGetResManager()->BuildResources())
    {
       m_ExitState = EXIT_QUIT;    //.bin files have been build, so quit
       return;
    }

    // Load in miscellaneous assets
    IwGetResManager()->LoadGroup("ui/ui.group");
}

//--------------------------------------------------------------------------------

CGame::~CGame()
{
    IW_CALLSTACK("CGame::~CGame");

    if (g_UseDerbh)
    {
        // Un-map the archive now.
        dzArchiveDetach();
    }

#ifdef IW_USE_PROFILE
    delete &CIwProfileMenu::Get();
    delete &CIwProfileManager::Get();
#endif

    delete IwGetMenuManager();


    IwGraphicsTerminate();
    IwGxMotionBlurTerminate();
    IwGxTerminate();
    IwSoundTerminate();


    m_Instance = NULL;
}

//--------------------------------------------------------------------------------

void CGame::Run()
{
    IW_CALLSTACK("CGame::Run");
    IW_PROFILE("Game->Run");

    if (m_ExitState == EXIT_QUIT)
        return;


    {
        CTempBucketSwitch b(MB_STATES);
        PushState(new CFrontendState);
    }

    while (m_States.GetSize() != 0)
    {
        // Start a new frame of the profile.
        IW_PROFILE_NEWFRAME();

        //have at least some yielding (game states should also yield)
        s3eDeviceYield(0);
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        RenderStates();
        Update();
    }
}

//--------------------------------------------------------------------------------

void CGame::UpdateTick()
{
    IW_CALLSTACK("CGame::Update");
    IW_PROFILE("Game->UpdateTick");

    CIwGame::UpdateTick();

    UpdateStates();

    if (m_QueuedState || m_QueuedPop)
    {
        //clear input so next state doesn't receive the same 'events' again
        s3eKeyboardUpdate();
        s3ePointerUpdate();
    }

    if (m_QueuedPop)
    {
        PopState();
        m_QueuedPop = false;
    }

    IwGetSoundManager()->Update();

    // It is essential that this is here, rather than anywhere else...
    // Otherwise the debug manager won't get coherent key input information
    if (IwGetMenuManager())
        IwGetMenuManager()->Update();
}

//--------------------------------------------------------------------------------

void CGame::DebugUpdate()
{
    IW_CALLSTACK("CGame::DebugUpdate");

    CIwGame::DebugUpdate();
}

//--------------------------------------------------------------------------------
