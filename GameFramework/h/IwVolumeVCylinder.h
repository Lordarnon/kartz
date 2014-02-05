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
#ifndef IW_VOLUMEVCYLINDER_H
#define IW_VOLUMEVCYLINDER_H

// Our includes
#include "IwGeomBBox.h"
#include "IwVolume.h"

/**
 * @addtogroup IwVolume
 * @brief A vertical cylinder (major axis is y)
 * @{
 *
 * A vertical cylinder (major axis is y)
 */
class CIwVolumeVCylinder : public CIwVolume
{
public:
    IW_MANAGED_DECLARE(CIwVolumeVCylinder);

    /** Constructor */
    CIwVolumeVCylinder();
    /** Initialisation */
    virtual void    Init();
    /** Returns a point at the centre of the volume */
    virtual CIwVec3 Centre();
    /** Check if a world point is inside the volume */
    virtual bool    IsPointInside(const CIwVec3& p);
    /** IO functions */
    virtual void    Serialise();
    virtual bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
#ifdef IW_DEBUG
    void            DebugRender();
#endif

protected:
    CIwVec3     m_Centre;       //!< Position of centre
    int32       m_Length;       //!< Length of major axis
    int32       m_Radius;       //!< Radius
    iwfixed     m_RadiusSqF;    //!< Radius squared in fixed point
};

#endif /* !IW_VOLUMEVCYLINDER_H */

// @}
