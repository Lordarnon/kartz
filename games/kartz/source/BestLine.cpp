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
#include "BestLine.h"
#include "World.h"

IW_MANAGED_IMPLEMENT(CBestLine);
IW_CLASS_FACTORY(CBestLine);


CBestLine::~CBestLine()
{
    delete m_Path;
}

void CBestLine::GetPoint(int32 dist, CIwVec3& outPosition, CIwSVec3& outNormal)
{
    m_Path->CalcPointAtDist(dist, &outPosition, &outNormal);
}


void CBestLine::Serialise()
{
    CIwManaged::Serialise();

    IwSerialiseManagedObject(*(CIwManaged**)&m_Path);
    if (m_Path)
    {
        m_Path->Init();
        m_Path->SetFlags( CIwShape::DEBUG_RENDER_F );
    }
}


void CBestLine::ParseClose(CIwTextParserITX* pParser)
{
#ifdef IW_BUILD_RESOURCES

    GetWorld().SetBestLine(this);

#endif // #ifdef IW_BUILD_RESOURCES
}

void CBestLine::ParseCloseChild(CIwTextParserITX* pParser, CIwManaged* pChild)
{
#ifdef IW_BUILD_RESOURCES

    IwAssert(RCD, strncmp(pChild->GetClassName(), "CIwPath", 7) == 0);
    m_Path = (CIwPath*)pChild;

#endif // #ifdef IW_BUILD_RESOURCES
}



void CBestLine::DebugRender()
{
#ifdef IW_DEBUG
//  m_Path->DebugRender();
#endif
}
