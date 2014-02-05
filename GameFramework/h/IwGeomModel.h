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
 * Entity, renderable with model.
 */

/**
 * @addtogroup Entities
 * @{
 */

#ifndef IW_GEOMMODEL_H
#define IW_GEOMMODEL_H

// Includes
#include "IwGeomClass.h"

// Forward declarations
class CIwModel;
class CIwModelBlock;

class CIwGeomModel : public CIwGeom
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    IW_MANAGED_DECLARE(CIwGeomModel);
    CIwGeomModel();
    ~CIwGeomModel();
    virtual void Serialise();
    virtual void Resolve();
    virtual void Render();

    void SetVertCols();
    void ClearVertCols();

    void ParseCloseChild(CIwTextParserITX* pParser, CIwManaged* pChild);

    //if we have a vertex colour override get the colour set at the specified ID (otherwise 0xffffffff)
    //colIDs are visible from model builder
    CIwColour GetColourFromID(uint32 colID);


    // Members
    CIwModel*       m_Model;    //!< model to render

    CIwModelBlock*  m_VertCols; //!< optional vertex colour override
};

#endif // IW_GEOMMODEL_H

/** @} */
