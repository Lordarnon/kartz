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
#ifndef IW_MOTIONPATH_H
#define IW_MOTIONPATH_H

// Includes
#include "IwBehaviour.h"
//#include "IwPath.h"


/**
 * @addtogroup IwPath
 * @brief Motion behaviour: follow path
 * @{
 */

// Forward declarations
class CIwPath;

//-----------------------------------------------------------------------------
// CIwBeMotionPath
//-----------------------------------------------------------------------------
/** Motion behaviour: follow path */
class CIwBeMotionPath : public CIwBeMotion
{
public:
    IW_MANAGED_DECLARE(CIwBeMotionPath);

    /** Flags */
    enum
    {
        UPDATE_ENT_POS_F    = (1 << 0),     //!< update ent pos from path pos
        UPDATE_ENT_ROT_F    = (1 << 1),     //!< update ent rot from path tangent
        OFF_AT_END_F        = (1 << 2),     //!< when end of path reached: turn off behaviour
        NEXT_AT_END_F       = (1 << 3),     //!< when end of path reached: connect to next path
    };

    // Methods
    CIwBeMotionPath();

    virtual void Update();
    virtual void DebugAddMenuItems(CIwMenu* pMenu);

    // Members
    uint32      m_Flags;        //!< flags
    CIwPath*    m_Path;         //!< target path
    int32       m_Dist;         //!< current distance along path
    int32       m_Speed;        //!< speed to move along path (16:16)
    int32       m_Acc;          //!< acceleration/deceleration (16:16)
    CIwVec3     m_LastPos;      //!< last calculated position
    CIwSVec3    m_LastTangent;  //!< last calculated tangent
};

#endif

// @}
