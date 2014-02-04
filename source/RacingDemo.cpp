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
#include "RacingDemo.h"
#include "Game.h"
#include "MemoryManager.h"
#include "s3eBackgroundMusic.h"

int main()
{
    //Initialise the memory manager.
    MemoryManagerInit();
    {
        CGame* game;
        {
            //create CGame object in 'Misc' memory bucket.
            CTempBucketSwitch b(MB_MISC);
            game = new CGame;
        }

        // To pass Microsoft submission requirenments app have to satisfy rules
        // for background music management.
        // On Windows Phone 8 application should prompt to pause background music.
        if (s3eDeviceGetInt(S3E_DEVICE_OS) == S3E_OS_ID_WP8 &&
            s3eBackgroundMusicAvailable())
            s3eBackgroundMusicShowPauseBackgroundMediaDialog();

        //run the game until it exits.
        game->Run();

        delete game;
    }
    //Terminate the memory manager, warning about any remaining allocations (these are leaks).
    MemoryManagerTerm();
    return 0;
}
