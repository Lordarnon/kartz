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
#include "WheelPos.h"
#include "IwTextParserITX.h"
#include "Chassis.h"

IW_MANAGED_IMPLEMENT(CWheelPos);
IW_CLASS_FACTORY(CWheelPos);

CWheelPos::CWheelPos()
{
    m_Pos.x = 999;
    m_Pos.y = 999;
    m_Pos.z = 999;
}

void CWheelPos::Serialise()
{
    IwSerialiseInt32(m_Pos.x, 3);
}

bool CWheelPos::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
#ifdef IW_BUILD_RESOURCES
    if (strcmp(pAttrName, "pos") == 0 )
    {
        pParser->ReadFixedArray(&m_Pos.x, 3);
        IwTrace(RCD, ("Found wheel pos: x:%d, y:%d, z:%d", m_Pos.x, m_Pos.y, m_Pos.z));
        return true;
    }

#endif // #ifdef IW_BUILD_RESOURCES

    return CIwManaged::ParseAttribute(pParser, pAttrName);
}

void CWheelPos::ParseClose(CIwTextParserITX* pParser)
{
#ifdef IW_BUILD_RESOURCES

    CChassis::Instance()->AddWheelPos(*this);

#endif // #ifdef IW_BUILD_RESOURCES
}
