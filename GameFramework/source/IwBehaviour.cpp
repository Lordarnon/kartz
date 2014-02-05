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
#include "IwBehaviour.h"
#include "IwEnt.h"

IW_MANAGED_IMPLEMENT(CIwBehaviour)
IW_MANAGED_IMPLEMENT(CIwBeCollision)
IW_MANAGED_IMPLEMENT(CIwBeMotion)
IW_MANAGED_IMPLEMENT(CIwBeRender)

//-----------------------------------------------------------------------------
// CIwBehaviour
//-----------------------------------------------------------------------------
CIwBehaviour::CIwBehaviour()
{
    IW_CALLSTACK("CIwBehaviour::CIwBehaviour")

    m_BaseFlags = ACTIVE_F;
    m_Ent   = NULL;
}
//-----------------------------------------------------------------------------
void    CIwBehaviour::Serialise()
{
    IW_CALLSTACK("CIwBehaviour::Serialise")

    CIwManaged::Serialise();
    IwSerialiseUInt32(m_BaseFlags);
}
//-----------------------------------------------------------------------------
void    CIwBehaviour::SetClassFlags(uint32 flags)
{
    IW_CALLSTACK("CIwBehaviour::SetClassFlags")

    IwAssertMsgN(303,ENGINE, !(m_BaseFlags & CLASS_FLAG_MASK), ("Class flags already set"));
    m_BaseFlags |= flags;
}

#ifdef IW_DEBUG
//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
void    CIwBehaviour::DebugRender()
{
    IW_CALLSTACK("CIwBehaviour::DebugRender")
}
//-----------------------------------------------------------------------------
void    CIwBehaviour::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwBehaviour::DebugAddMenuItems")

    // Other
    pMenu->AddItem(new CIwMenuItemEditFlags("ACTIVE", &m_BaseFlags, ACTIVE_F));
}
#endif // IW_DEBUG
