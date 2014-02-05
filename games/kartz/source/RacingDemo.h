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
#ifndef RACINGDEMO_H
#define RACINGDEMO_H

#include "s3e.h"
#include "IwGx.h"

#include "MemoryManager.h"

//use data errors when there is a *Fatal* flaw in the provided data set
#define RCD_DATA_ERROR(_s) IwError(_s)

//use data warn when the data set contains non-fatal errors
#define RCD_DATA_WARN(_s) IwError(_s)

#endif
