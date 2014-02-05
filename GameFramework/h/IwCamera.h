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

/**
 * @defgroup Entities Entities
 */

/**
 * @addtogroup Entities
 * @brief Camera object with position (from ent) and FOV info.
 * @{
 */


#ifndef IW_CAMERA_H
#define IW_CAMERA_H

// Includes
#include "IwManaged.h"
#include "IwEnt.h"

//-----------------------------------------------------------------------------
// CIwCamera
//-----------------------------------------------------------------------------
/** CIwCamera class */
class CIwCamera : public CIwEnt
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    IW_MANAGED_DECLARE(CIwCamera);

    // Methods
    CIwCamera();
    virtual ~CIwCamera();
    virtual void Serialise();
    virtual bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    virtual void ParseClose(CIwTextParserITX* pParser);

    /** Initialise the near, far, persmul from current GX values */
    void    InitFromGx();

    /** Set the near and far clip plane distances */
    inline void     SetNearFar(int32 nearZ, int32 farZ)
    {
        m_NearZ = nearZ;
        m_FarZ  = farZ;
    }

    /** Set the perspective multiple */
    inline void     SetPerspMul(int32 perspMul)
    {
        m_PerspMul = perspMul;
    }

    /** Set the field of view (fixed-point angle in X) */
    virtual void    SetFOV(iwangle fov);

    /** Sets the current gx view paramaters to be that of this camera */
    virtual void    Update();

    int32   m_NearZ;        //!< Near clip plane
    int32   m_FarZ;         //!< Far clip plane
    int32   m_PerspMul;     //!< Half viewplane distance (zoom factor)
};

#endif // IW_CAMERA_H

/** @} */
