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
#ifndef IW_RESHANDLERWORLD_H
#define IW_RESHANDLERWORLD_H

// Includes
#include "IwArray.h"
#include "IwResManager.h"

/**
 * @defgroup IwResHandlerWORLD IwResHandlerWORLD
 */

/**
 * @addtogroup IwResHandlerWORLD
 * @brief Resource handler for .world files.
 * @{
 */

// Forward declarations
class CIwResource;

//-------------------------------------------------------------------------
// CIwResHandlerWORLD
//-------------------------------------------------------------------------
/** Resource handler for .world files. */
class CIwResHandlerWORLD : public CIwResHandler
{
public:
    IW_MANAGED_DECLARE(CIwResHandlerWORLD);

    // Methods
    CIwResHandlerWORLD();
    virtual CIwResource*    Build(const CIwStringL& pathname);

    // Static members
    static  const char* g_FlagNames[];
};

#endif

// @}
