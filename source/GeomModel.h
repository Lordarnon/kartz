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
#ifndef GEOMMODEL_H
#define GEOMMODEL_H

#include "IwGeomModel.h"

class CShadow;

/**
 * CGeomModel provides a concrete implementation of CIwGeomModel
 * suitable for displaying a CIwModel at a position in world space
 *
 * The extra functionality provided by this class is:
 *
 * Automatically creates a frame and mark it for serialise, so
 * CIwEnt::ParseAttribute() can understand "pos" and "rot" itx parameters
 * and the resulting transform can be restored.
 *
 * ParseAttribute understands the "model" itx parameter
 * looks up the specified model from IwResManager)
 *
 * Render updates the model matrix before rendering the actual geometry
 */
class CGeomModel : public CIwGeomModel
{
public:
    IW_MANAGED_DECLARE(CGeomModel);

    CGeomModel(CShadow* pShadow = NULL);
    ~CGeomModel();

    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    virtual void ParseClose(CIwTextParserITX* pParser);

    virtual void Render();

private:
    CShadow*    m_pShadow;
};


class CShadow : public CIwGeomModel
{
public:
    IW_MANAGED_DECLARE(CShadow)

    CShadow();

    virtual void    Render();

    // must call before rendering
    virtual void    Init();

    void            SetGeometry(CGeomModel* pGeometry)  { m_pGeometry = pGeometry; m_Frame = m_pGeometry->GetFrame(); }
    void            SetIntensity(uint8 intensity)       { m_Intensity = intensity; }
    uint8           GetIntensity()                      { return m_Intensity; }

private:
    // shadow material
    CIwMaterial*    m_Mat;

    uint8           m_Intensity;

    CGeomModel*     m_pGeometry;
};

#endif /* !GEOMMODEL_H */
