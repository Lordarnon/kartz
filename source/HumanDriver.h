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
#ifndef HUMANDRIVER_H
#define HUMANDRIVER_H

#include "Driver.h"

class CHumanDriver : public CDriver
{
public:
    CHumanDriver();
    virtual ~CHumanDriver() {}

    void Update(CCar* car);

    virtual void ApplySteering(CCar* car, int32 steer, int32 maxSteer);

private:
    static iwfixed GetSpeedForTurn(iwangle turn);
    bool m_UseAccelerometer;
};

#endif // !HUMANDRIVER_H
