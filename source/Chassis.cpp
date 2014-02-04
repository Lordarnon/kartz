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
#include "Chassis.h"

IW_MANAGED_IMPLEMENT(CChassis);
IW_CLASS_FACTORY(CChassis);

CChassis* CChassis::s_TheChassis = 0;

CChassis::CChassis()
{
    s_TheChassis = this;
    m_CurrentWheelPos = 0;

    //TODO: These values could come from data... a world file, for example
    m_Body.SetAttributes("goKart", CIwVec3( IW_FIXED_FROM_FLOAT( 0.000000f ),
                                            IW_FIXED_FROM_FLOAT( 94.819794f ),
                                            IW_FIXED_FROM_FLOAT( 0.000000f ) ) );
    m_WheelPositions[0].SetPos( CIwVec3(IW_FIXED_FROM_FLOAT( 84.863220f ),
                                        IW_FIXED_FROM_FLOAT( 23.023136f ),
                                        IW_FIXED_FROM_FLOAT( 67.600037f ) ) );
    m_WheelPositions[1].SetPos( CIwVec3(IW_FIXED_FROM_FLOAT( 88.773972f ),
                                        IW_FIXED_FROM_FLOAT( 23.023136f ),
                                        IW_FIXED_FROM_FLOAT( -126.257866f ) ) );
    m_WheelPositions[2].SetPos( CIwVec3(IW_FIXED_FROM_FLOAT( -90.002991f ),
                                        IW_FIXED_FROM_FLOAT( 23.023136f ),
                                        IW_FIXED_FROM_FLOAT( -126.257866f) ) );
    m_WheelPositions[3].SetPos( CIwVec3(IW_FIXED_FROM_FLOAT( -86.092239f ),
                                        IW_FIXED_FROM_FLOAT( 23.023136f ),
                                        IW_FIXED_FROM_FLOAT( 67.600037f ) ) );
}

bool CChassis::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    return CIwManaged::ParseAttribute(pParser, pAttrName);
}

void CChassis::ParseClose(CIwTextParserITX* pParser)
{
}

void CChassis::Serialise()
{
    CIwModel::Serialise();
    for (int i = 0; i < 4; i++)
    {
        m_WheelPositions[i].Serialise();
    }
    m_Body.Serialise();
}

void CChassis::AddWheelPos(const CWheelPos& pos)
{
    IwAssert(RCD, m_CurrentWheelPos < 4);

    m_WheelPositions[m_CurrentWheelPos] = pos;
    ++m_CurrentWheelPos;
}

const CWheelPos& CChassis::GetWheelPos(int i)
{
    IwAssert(RCD, i < 4);
    return m_WheelPositions[i];
}
