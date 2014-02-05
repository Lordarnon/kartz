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
#ifndef IW_GEOMCLASS_H
#define IW_GEOMCLASS_H

/**
 * @addtogroup Entities
 * @{
 */

// Includes
#include "IwEnt.h"
#include "IwGx.h"

/**
 * Base class for renderable entity.
 */
class CIwGeom : public CIwEnt
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    IW_MANAGED_DECLARE(CIwGeom);

    /** Render interface to all renderable ents */
    virtual void Render() = 0;

    /** Helper method to set gx modelview transform from entity local matrix */
    inline  void    SetModelViewTransform()
    {
        IwAssertMsgN(294,ENGINE, m_Frame,           ("Setting model view transform, but we don't have a frame!"));

        // Ensure world matrix is valid
        m_Frame->Update();

        IwGxSetModelMatrix(GetMatWorld());
        //IwGxUpdateModelViewMatrix();
    }

#ifdef IW_DEBUG
    /** Add menu items to a CIwMenu for debugging purposes */
    void    DebugAddMenuItems(CIwMenu* pMenu);
#endif
};

/**
 * @}
 */

#endif /* !IW_GEOMCLASS_H */
