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
#ifndef IW_ENGINE_H
#define IW_ENGINE_H

// Incudes
#ifdef IW_DEBUG
#include "IwMenu.h"
#endif

/**
 * @defgroup IwEngine IwEngine
 */

/**
 * @addtogroup IwEngine
 * @brief Initialisation/Shutdown helper functions
 * @{
 */


/** Resource types */
#define IW_ENGINE_RESTYPE_WORLD     "CIwWorld"
#define IW_ENGINE_RESTYPE_TIMELINE  "CIwTimeLine"

/** Initialise all core functionality for IwEngine, must call before IwEngine is used */
void    IwEngineInit();

/** Shut down IwEngine and free all memory */
void    IwEngineTerminate();

#ifdef IW_DEBUG
//-----------------------------------------------------------------------------
// CIwMenuItemEngine
//-----------------------------------------------------------------------------
/** Helper class to spawn debug menu */
class CIwMenuItemEngine : public CIwMenuItem
{
public:
    CIwMenuItemEngine() : CIwMenuItem("", "IwEngine", 0, CIwMenuItem::MENU) {};
    void Select();
};
#endif  // IW_DEBUG

#endif // IW_ENGINE_H
/** @} */
