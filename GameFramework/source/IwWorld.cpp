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
 * World class. Essentially a container for entities.
 */

#include "IwWorld.h"
#include "IwCamera.h"
#include "IwEngine.h"
#include "IwGeomClass.h"
#include "IwGx.h"
#include "IwRenderManager.h"
#include "IwTextParserITX.h"

// Static members
IW_MANAGED_IMPLEMENT(CIwWorld)
CIwWorld* CIwWorld::g_Context = NULL;

//-----------------------------------------------------------------------------
// CIwWorld
//-----------------------------------------------------------------------------
IW_CLASS_FACTORY(CIwWorld);
CIwWorld::CIwWorld()
{
    IW_CALLSTACK("CIwWorld::CIwWorld")

    g_Context = this;
}
//-----------------------------------------------------------------------------
CIwWorld::~CIwWorld()
{
    IW_CALLSTACK("CIwWorld::~CIwWorld")

    m_Objects.Delete();
}
//-----------------------------------------------------------------------------
void    CIwWorld::Serialise()
{
    IW_CALLSTACK("CIwWorld::Serialise")

    CIwManaged::Serialise();
    m_Objects.Serialise();
}
//-----------------------------------------------------------------------------
void    CIwWorld::Resolve()
{
    IW_CALLSTACK("CIwWorld::Resolve")

    CIwManaged::Resolve();
    IW_MANAGED_LIST_ITERATE(m_Objects, itO)
        (*itO)->Resolve();

    // Test
//  IwGetRenderManager()->SetWorld(this);
}
//-----------------------------------------------------------------------------
void    CIwWorld::ParseOpen(CIwTextParserITX* pParser)
{
    IW_CALLSTACK("CIwWorld::ParseOpen")

#ifdef IW_BUILD_RESOURCES

    g_Context = this;

#endif // #ifdef IW_BUILD_RESOURCES
}
//-----------------------------------------------------------------------------
bool    CIwWorld::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwWorld::ParseAttribute")

    return CIwManaged::ParseAttribute(pParser, pAttrName);
}
//-----------------------------------------------------------------------------
void    CIwWorld::ParseClose(CIwTextParserITX* pParser)
{
    IW_CALLSTACK("CIwWorld::ParseClose")

#ifdef IW_BUILD_RESOURCES

    pParser->SetReturnValue(g_Context);
    g_Context = NULL;

#endif // #ifdef IW_BUILD_RESOURCES
}

#ifdef IW_DEBUG
//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
void    CIwWorld::DebugRender()
{
    IW_CALLSTACK("CIwWorld::DebugRender")

    IW_MANAGED_LIST_ITERATE(m_Objects, itO)
        (*itO)->DebugRender();
}
//-----------------------------------------------------------------------------
void    CIwWorld::DebugAddMenuItems(CIwMenu* pMenu)
{
    IW_CALLSTACK("CIwWorld::DebugAddMenuItems")

    pMenu->AddItem(new CIwMenuItemManagedList("Objects", &m_Objects));
}
#endif // IW_DEBUG
