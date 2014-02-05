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
#ifndef IW_MOTIONCAMFREE_H
#define IW_MOTIONCAMFREE_H

// Includes
#include "IwBehaviour.h"

/**
 * @defgroup IwMotionCamFree IwMotionCamFree
 */

/**
 * @addtogroup IwMotionCamFree
 * @brief Camera behaviour: free-cam
 * @{
 */

// Forward declarations

//-----------------------------------------------------------------------------
// CBeMotionCamFree
//-----------------------------------------------------------------------------
/** Free (fly around) camera behaviour */
class CIwBeMotionCamFree : public CIwBeMotion
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE(CIwBeMotionCamFree);

    // Methods
    CIwBeMotionCamFree();

    /** Set max ang vel */
    inline void SetMaxAngVel(int32 v) {m_MaxAngVel = v;}

    /** Set max speed */
    inline void SetMaxSpeed(int32 v) {m_MaxSpeed = v;}

    /** Init */
    void    Init();

    /** Update */
    void    Update();

    void    SetUseWorldY(); //make the camera rotate around the world Y axis rather than its local Y...

    // Members
private:
    int32   m_MaxAngVel;
    int32   m_MaxSpeed;
    bool    m_UseWorldY;
};

#endif

// @}
