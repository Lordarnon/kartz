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
 * Resource handler for .world files.
 */

#include "IwResHandlerWORLD.h"
#include "IwClassFactory.h"
#include "IwDebug.h"
#include "IwEngine.h"
#include "IwPath.h"
#include "IwRenderManager.h"
#include "IwResource.h"
#include "IwRuntime.h"
#include "IwTextParserITX.h"
#include "IwWorld.h"
#include "s3eFile.h"

/** Flag names */
const char* CIwResHandlerWORLD::g_FlagNames[] =
{
    NULL,
};

//-------------------------------------------------------------------------
// CIwResHandlerWORLD
//-------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwResHandlerWORLD)
CIwResHandlerWORLD::CIwResHandlerWORLD() : CIwResHandler("world", IW_ENGINE_RESTYPE_WORLD)
{
    SetName("WORLD");
}
//-------------------------------------------------------------------------
CIwResource*    CIwResHandlerWORLD::Build(const CIwStringL& pathname)
{
    IW_CALLSTACK("CIwResHandlerWORLD::Build")

#ifndef IW_BUILD_RESOURCES
    IwAssertMsgN(361,GRAPHICS, false, ("Project not built with IW_BUILD_RESOURCES"));
    return NULL;
#else
    char fileonly[128];
    IwPathGetFilename(pathname.c_str(), fileonly, false);

    IwAssertMsgN(362,ENGINE, IwGetTextParserITX(), ("IwEngine has no parser"));
    CIwWorld* pWorld = (CIwWorld*)IwClassFactoryCreate("CWorld");
    pWorld->SetName(fileonly);
    CIwWorld::g_Context = pWorld;
    IwGetTextParserITX()->PushObject( pWorld );
    IwGetTextParserITX()->ParseFile(pathname.c_str());
    IwGetTextParserITX()->PopObject();

    return pWorld;
#endif
}
