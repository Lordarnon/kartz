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
#ifndef CHASSIS_H_INCLUDED
#define CHASSIS_H_INCLUDED

#include "IwModel.h"
#include "WheelPos.h"
#include "CarBody.h"

class CChassis : public CIwModel
{
public:
    IW_MANAGED_DECLARE(CChassis);

    CChassis();
    virtual bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    virtual void ParseClose(CIwTextParserITX* pParser);
    virtual void Serialise();

    void AddWheelPos(const CWheelPos& );
    const CWheelPos& GetWheelPos(int i);

    void SetBody(const CCarBody& body) { m_Body = body; }
    const CCarBody& GetBody() const { return m_Body; }

    static CChassis* Instance() { return s_TheChassis; }

private:
    CWheelPos m_WheelPositions[4];
    int m_CurrentWheelPos;
    CCarBody m_Body;

    static CChassis* s_TheChassis;
};


#endif
