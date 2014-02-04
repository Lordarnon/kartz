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
#ifndef BESTLINE_H
#define BESTLINE_H

#include "IwShapePath.h"

class CBestLine : public CIwManaged
{
public:
    IW_MANAGED_DECLARE(CBestLine);

    ~CBestLine();
    iwfixed GetLength() { return m_Path->GetLength(); }

    void GetPoint(int32 dist, CIwVec3& outPosition, CIwSVec3& outNormal);

    virtual void Serialise();

    virtual void ParseClose(CIwTextParserITX* pParser);
    virtual void ParseCloseChild(CIwTextParserITX* pParser, CIwManaged* pChild);

    virtual void DebugRender();

private:
    CIwPath* m_Path;
};

#endif /* !BESTLINE_H */
