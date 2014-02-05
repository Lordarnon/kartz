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
#include "MemoryManager.h"
#include "IwMemBucketHelpers.h"
#include "IwUtilInitTerm.h"
#include "RacingDemo.h"

class CMemoryManager
{
public:
    CMemoryManager();
    ~CMemoryManager();

    CMemoryBucket SetCurrentBucket(CMemoryBucket bucket);

    CMemoryBucket m_CurrentBucket;
};



static const uint32 NUM_BUCKETS = 6;

struct BucketInfo
{
    uint32      UniqueID;
    const char*       pName;
};

// Information about the bucket in use
static const BucketInfo s_BUCKET_INFO[NUM_BUCKETS] =
{
    { MB_GX,        "Gx"    },
    { MB_TRACK,     "Track" },
    { MB_CARS,      "Cars"  },
    { MB_GUI,       "Gui"   },
    { MB_STATES,    "States"},
    { MB_MISC,      "Misc"  },
};


#define SMALL_ALLOC_SIZE 32

//------------------------------------------------------------------------------------------------------------

CMemoryManager* g_Memory = NULL;


void MemoryManagerInit()
{
    IW_CALLSTACK("MemoryManagerInit");

    g_Memory = new CMemoryManager();
}


void MemoryManagerTerm()
{
    IW_CALLSTACK("MemoryManagerTerm");

    IwAssert(RCD, g_Memory);

    delete g_Memory;
}

CMemoryManager::CMemoryManager()
: m_CurrentBucket(MB_NONE)
{
    IW_CALLSTACK("CMemoryManager::CMemoryManager");

    IwUtilInit();

    for (uint32 b = 0; b < NUM_BUCKETS; b++)
    {
        int size;
        if (s3eConfigGetInt("RacingDemoMemory", s_BUCKET_INFO[b].pName, &size) != S3E_RESULT_SUCCESS )
        {
            RCD_DATA_ERROR(("Couldn't find setting for bucket %s", s_BUCKET_INFO[b].pName));
        }

        IwMemBucketCreate(s_BUCKET_INFO[b].UniqueID, s_BUCKET_INFO[b].pName, size);
    }
}


CMemoryManager::~CMemoryManager()
{
    IW_CALLSTACK("CMemoryManager::~CMemoryManager");

    for (uint32 b = 0; b < NUM_BUCKETS; b++)
    {
        IwAssertMsg(RCD, IwMemBucketGetUsed( s_BUCKET_INFO[b].UniqueID ) == 0, ("%u still allocated in %s", IwMemBucketGetUsed( s_BUCKET_INFO[b].UniqueID ), s_BUCKET_INFO[b].pName) );

        IwMemBucketFree( s_BUCKET_INFO[b].UniqueID );
    }

    IwUtilTerminate();
}


CMemoryBucket CMemoryManager::SetCurrentBucket(CMemoryBucket bucket)
{
    CMemoryBucket OldBucket = m_CurrentBucket;
    m_CurrentBucket = bucket;
    return OldBucket;
}

int32 MemoryManagerGetUsed(CMemoryBucket bucket)
{
    return IwMemBucketGetUsed( bucket );
}

//------------------------------------------------------------------------------------------------------------

CTempBucketSwitch::CTempBucketSwitch(CMemoryBucket bucket)
:   m_PreviousBucket( static_cast<CMemoryBucket>( IwMemBucketGetID() ) )
{
    IwMemBucketPush(bucket);
}

CTempBucketSwitch::~CTempBucketSwitch()
{
    IwMemBucketPop();
}
