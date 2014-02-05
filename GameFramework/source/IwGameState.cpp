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
#include "IwGameState.h"
#include "IwGame.h"
#include "IwRuntime.h"

//-----------------------------------------------------------------------------
// CIwGameState
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwGameState)
CIwGameState::CIwGameState()
{
    IW_CALLSTACK("CIwGameState::CIwGameState")
    IwAssert(ENGINE, IwGetGame());

    // Default: state will be updated and rendered, and will replace existing top state
    m_Flags = UPDATE_F | RENDER_F | REPLACE_PREV_F;
}
//-----------------------------------------------------------------------------
CIwGameState::~CIwGameState()
{
    IW_CALLSTACK("CIwGameState::~CIwGameState")
    IwAssert(ENGINE, IwGetGame());

    // Remove this from the game stack, if it is in the stack.
    IwGetGame()->RemoveState(this);
}

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwGameState::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwGameState::DebugAddMenuItems")

    CIwManaged::DebugAddMenuItems(pMenu);
    pMenu->AddItem(new CIwMenuItemEditFlags("UPDATE", &m_Flags, UPDATE_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("RENDER", &m_Flags, RENDER_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("REPLACE_STATE", &m_Flags, REPLACE_PREV_F));
    pMenu->AddItem(new CIwMenuItemEditFlags("DELETE_STATE", &m_Flags, DELETE_PREV_F));
}
#endif
