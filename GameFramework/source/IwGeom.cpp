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
#include "IwGeomClass.h"
#include "IwMenu.h"

//-----------------------------------------------------------------------------
// CIwGeom
//-----------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwGeom)

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwGeom::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwGeom::DebugAddMenuItems")

    CIwEnt::DebugAddMenuItems(pMenu);
}
#endif
