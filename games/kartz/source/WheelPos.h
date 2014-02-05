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
#ifndef WHEEL_POS_H_INCLUDED
#define WHEEL_POS_H_INCLUDED

#include "IwManaged.h"
#include "IwGeomVec3.h"

class CWheelPos : public CIwManaged
{
public:
    IW_MANAGED_DECLARE(CWheelPos);

    CWheelPos();
    virtual bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    virtual void ParseClose(CIwTextParserITX* pParser);
    virtual void Serialise();

    const CIwVec3& GetPos() const { return m_Pos; }
    void SetPos(const CIwVec3& position) { m_Pos = position; }

private:
    CIwVec3 m_Pos;
};

#endif
