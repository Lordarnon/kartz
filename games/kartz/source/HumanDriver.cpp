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
#include "HumanDriver.h"

#include "Car.h"
#include "World.h"
#include "GeomUtils.h"

#define SPEED_DIFF IW_FIXED(10)

#define STEERING_DIFF   16
#define STEERING_RETURN (STEERING_DIFF * 2)

CHumanDriver::CHumanDriver()
{
#ifdef IW_DEBUG
    m_DriverNum = 0;
#endif
    m_UseAccelerometer = s3eAccelerometerGetInt(S3E_ACCELEROMETER_AVAILABLE) != 0;
}

void CHumanDriver::Update(CCar* car)
{
    bool decel = false;
    if (m_UseAccelerometer)
    {
        // calculate tilt

        // use the largest of Y and Z axis
        // this allows the roll of the device to be measured
        int axis = s3eAccelerometerGetY();
        if (abs(s3eAccelerometerGetZ()) > abs(axis) )
            axis = s3eAccelerometerGetZ();

        iwangle tilt = IwGeomAtan2(s3eAccelerometerGetX(), axis) + IW_ANGLE_2PI/2;

        // move tilt into -180 to +180 degree angle range
        if (tilt > IW_ANGLE_2PI/2)
            tilt -= IW_ANGLE_2PI;

        // limit steer to match other control schemes
        int dsteer = tilt > 0 ? 1 : -1;
        int steer = abs(tilt)/2;
        car->SetSteering(-MIN(steer, 80)*dsteer);

        // brake on any touch
        if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_DOWN )
            decel = true;

    }
    if (!decel && s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_DOWN )
    {
        // analogue steering based on x-axis position
        int32 steer = s3ePointerGetX();
        steer -= IwGxGetScreenWidth() / 2;
        steer = steer * 80 / ((int32)IwGxGetScreenWidth() / 2);
        car->SetSteering(steer);

        //digital braking if touch is in the lower quarter of the screen...
        decel = s3ePointerGetY() > (int32)IwGxGetScreenHeight() / 4;
    }
    else
    {
        // Calculate and apply the amount of steer.
        int32 dsteer =  (s3eKeyboardGetState(s3eKeyAbsRight) & S3E_KEY_STATE_DOWN) -
                        (s3eKeyboardGetState(s3eKeyAbsLeft) & S3E_KEY_STATE_DOWN);
        if (dsteer)
            m_UseAccelerometer = false;//stop using accelerometer if keys are used
        if (!m_UseAccelerometer)
            ApplySteering(car, dsteer, 80);
    }

    // If the user is pressing down then decelerate.
    if ((s3eKeyboardGetState(s3eKeyAbsDown) & S3E_KEY_STATE_DOWN) || decel )
    {
        if (car->GetSpeedKmh() > IW_FIXED(10.0) )
        {
            car->SetTargetSpeed( 0 );
        }
        else
        {
            car->SetTargetSpeed( -MAX_SPEED );
        }
    }
    else
    {
        // If we're not breaking to accelerate.
        car->SetTargetSpeed( GetSpeedForTurn( car->GetSteering() ) );
    }
}

void CHumanDriver::ApplySteering(CCar* car, int32 dsteer, int32 maxSteer)
{
    iwfixed steer = car->GetSteering();

    int steerIncrease = STEERING_DIFF;
    int steerDecrease = STEERING_RETURN * 2;

    int clampedSteer        =   (abs(steer) > maxSteer) ? ( maxSteer ) : ( abs(steer) );

    if (0 != steer)
    {
        if (abs(steer) < (maxSteer / 6))
        {
            iwfixed adjustedSteerIncrease = IW_FIXED_MUL( IW_FIXED( clampedSteer ), IW_FIXED( 1.25 ) );

            steerIncrease = adjustedSteerIncrease / IW_GEOM_ONE;
        }
        else if (abs(steer) > ((maxSteer / 3) * 2) )
        {
            iwfixed adjustedSteerIncrease = IW_FIXED_MUL( IW_FIXED( clampedSteer ), IW_FIXED( 0.5 ) );

            steerIncrease = adjustedSteerIncrease / IW_GEOM_ONE;
        }
    }

    const bool changeInSteerRequested       = (0 != dsteer);
    const bool streerCurrentlyApplied       = (0 != steer);
    const bool returnSteerToNeutral         = !changeInSteerRequested && streerCurrentlyApplied;
    const bool steerExceedsMax              = (abs(steer) >= maxSteer);
    const bool dsteerInDirectionOfSteer     = (steer > 0) == (dsteer > 0);
    const bool changeInSteerIsNeeded        = !(steerExceedsMax && dsteerInDirectionOfSteer);

    // If there is no delta steer but there is current steer then
    if (returnSteerToNeutral)
    {
        // Return steer to neutral.
        if (abs(steer) < steerDecrease )
        {
            steer = 0;
        }
        else
        {
            steer += steerDecrease * (steer > 0 ? -1 : 1);
        }
    }
    else if (changeInSteerRequested && changeInSteerIsNeeded)
    {
        int32 steerdiff = 1;
        if (streerCurrentlyApplied && (steer > 0) != (dsteer > 0) )
            steerdiff = 3;

        steer += steerIncrease * (dsteer > 0 ? steerdiff : -steerdiff);
    }

    car->SetSteering(steer);
}


iwfixed CHumanDriver::GetSpeedForTurn(iwangle turn)
{
    turn = abs(turn);
    if (turn > 60)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.1));
    if (turn > 40)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.6));
    if (turn > 20)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.9));

    return MAX_SPEED;
}
