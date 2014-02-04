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
#ifndef GAME_H
#define GAME_H

#include "IwGame.h"

class CGame : public CIwGame
{
public:
    CGame();
    virtual ~CGame();

    // CIwGame virtuals
    virtual void    Run();
    virtual void    Update() { CIwGame::Update(); }
    virtual void    UpdateTick();
    virtual void    UpdateControls()  { CIwGame::UpdateControls(); }
    virtual void    DebugUpdate();

    void QueuePop()
    {
        m_QueuedPop = true;
    }
private:
    bool m_QueuedPop;

    static CGame* m_Instance;
    friend CGame& GetGame();
};

inline CGame& GetGame()
{
    IwAssert(RCD, CGame::m_Instance);
    return *CGame::m_Instance;
}

#endif /* !GAME_H */
