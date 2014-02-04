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
#ifndef AIDRIVER_H
#define AIDRIVER_H

#include "Driver.h"

class CAIDriver : public CDriver
{
public:
    CAIDriver(iwfixed speedmod);
    virtual ~CAIDriver() {}

    virtual void Update(CCar* car);

#ifdef IW_DEBUG
    virtual void DebugRender();
#endif

    virtual void ApplySteering(CCar* car, int32 steer, int32 maxSteer);

private:
    // Try to avoid other cars within a cone in front, by changing the
    // current speed and heading.
    // Returns true if one or more cars are in the way, and so adjustments
    // have been set.
    bool AvoidOtherCars(
        CCar* thisCar,  // the car which we are driving
        iwfixed* adjustHeading, // adjustment to heading to avoid other cars
        iwfixed* adjustSpeed); // adjustment to speed to avoid other cars

    static iwfixed GetSpeedForTurn(iwangle turn);

private:
    iwfixed m_SpeedMod;

    // Steering to avoid other cars: these members tune the way this AI
    // avoids other cars.
    // Half the angle of the cone in which we look for other cars. Cone angle
    // varies with distance of other car.
    iwangle m_MinConeAngle;
    iwangle m_MaxConeAngle;
    iwangle m_ConeAngle; // current cone angle, between min and max.
    // Length of cone
    iwfixed m_ConeLength;
    // Max adjustment to car heading
    iwangle m_MaxHeadingChange;
    // Look ahead distance to see if the adjusted heading leads off the track
    iwfixed m_AdjustTargetLen;

#ifdef IW_DEBUG
    CCar* m_Car;
    iwangle m_TargetHeading;
    iwangle m_AdjustHeading; // adjustment to target so we avoid Cars in front
    CIwVec3 m_AdjustedTarget; // target position, adjusted to avoid other cars
    int m_AvoidCarNum;
#endif

    static const uint32 STEERING_DIFF;
    static const uint32 STEERING_RETURN;
};

#endif /* !AIDRIVER_H */
