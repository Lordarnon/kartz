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
#include "CarBody.h"
#include "IwTextParserITX.h"
#include "Chassis.h"

IW_MANAGED_IMPLEMENT(CCarBody);
IW_CLASS_FACTORY(CCarBody);

CCarBody::CCarBody()
{
}

void CCarBody::Serialise()
{
    m_GeoName.Serialise();
    m_Pos.Serialise();
}

bool CCarBody::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
#ifdef IW_BUILD_RESOURCES
    if (strcmp(pAttrName, "model") == 0 )
    {
        pParser->ReadString(m_GeoName);
        return true;
    }
    else if (strcmp(pAttrName, "pos") == 0 )
    {
        pParser->ReadFixedArray(&m_Pos.x, 3);
        IwTrace(RCD, ("Found car body pos: x:%d, y:%d, z:%d", m_Pos.x, m_Pos.y, m_Pos.z));
        return true;
    }
#endif // #ifdef IW_BUILD_RESOURCES

    return CIwManaged::ParseAttribute(pParser, pAttrName);
}

void CCarBody::ParseClose(CIwTextParserITX* pParser)
{
#ifdef IW_BUILD_RESOURCES

    CChassis::Instance()->SetBody(*this);

#endif // #ifdef IW_BUILD_RESOURCES
}
