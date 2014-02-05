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
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "IwMemBucket.h"

void MemoryManagerInit();
void MemoryManagerTerm();

enum CMemoryBucket
{
    MB_GX = IW_MEM_BUCKET_ID_USER,      //for gx and renderer

    MB_TRACK,   //space to load the level into
    MB_CARS,    //space for cars
    MB_GUI,     //gui elements here
    MB_STATES,  //game states

    MB_MISC,    //all other allocs

    MB_COUNT,   //number of buckets
    MB_NONE,    //no bucket (default value)
};

int32 MemoryManagerGetUsed(CMemoryBucket bucket);

class CTempBucketSwitch
{
public:
    CTempBucketSwitch(CMemoryBucket bucket);
    ~CTempBucketSwitch();
private:
    CMemoryBucket m_PreviousBucket;
};

#endif /* !MEMORYMANAGER_H */
