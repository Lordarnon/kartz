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
#ifndef LOADSTATE_H
#define LOADSTATE_H

#include "BaseState.h"

class CLoadState : public CBaseState
{
public:
    IW_MANAGED_DECLARE(CLoadState);

    CLoadState();

    virtual void Update();
    virtual void Render();

private:
    static bool m_Loaded;
};


#endif /* !LOADSTATE_H */
