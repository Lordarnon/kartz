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
#ifndef DRIVER_H
#define DRIVER_H

#define MAX_SPEED KPH_TO_MPF(36) //100 km/s

class CCar;

// Driver acts as a behaviour for the car behaviour.
class CDriver
{
public:
    virtual ~CDriver() {}
    virtual void Update(CCar* car) = 0;

#ifdef IW_DEBUG
    virtual void DebugRender() {};
    void SetDriverNum(int num) { m_DriverNum = num; }
#endif

    virtual void ApplySteering(CCar* car, int32 steer, int32 maxSteer) = 0;

#ifdef IW_DEBUG
    int m_DriverNum;
#endif

};

#endif /* !DRIVER_H */
