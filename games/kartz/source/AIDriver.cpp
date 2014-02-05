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
#include "AIDriver.h"

#include "IwEnt.h"
#include "BestLineTracker.h"
#include "Car.h"
#include "GeomUtils.h"
#include "World.h"
#include "IwGxPrint.h"

#define DO_AVOID_OTHER_CARS


const uint32 CAIDriver::STEERING_DIFF   = 16;
const uint32 CAIDriver::STEERING_RETURN = (STEERING_DIFF * 2);


CAIDriver::CAIDriver(iwfixed speedmod)
: m_SpeedMod(speedmod)
#ifdef IW_DEBUG
, m_Car(0)
, m_AvoidCarNum(-1)
#endif
{
    //TODO: Set these members differently, so each AI car has different characteristics
    m_MinConeAngle = IW_ANGLE_FROM_DEGREES(30);
    m_MaxConeAngle = IW_ANGLE_FROM_DEGREES(90);
    m_ConeLength = IW_FIXED(0.5);
    m_MaxHeadingChange = IW_ANGLE_FROM_DEGREES(15);
    m_AdjustTargetLen = IW_FIXED(0.5);
}


void CAIDriver::Update(CCar* car)
{
#ifdef IW_DEBUG
    m_Car = car;
#endif

    CIwVec3 target = car->GetTracker().GetPoint();

    CTrackCollision::MoveInfo info;
    car->MoveInfo(target, info);
    if (!info.m_OverTrack)
    {
        target = info.m_WallPosition;
        target += info.m_WallNormal * IW_FIXED_MUL(IW_FIXED(10.0), WORLD_SCALE);
    }

    CIwVec3 toTarget = target - car->GetEnt()->GetPosLocal();
    iwfixed targetHeading = CGeomUtils::GetHeading(toTarget);

#ifdef IW_DEBUG
    iwfixed headingBefore = targetHeading; // For DEBUG trace only
#endif

    iwfixed adjustHeading = 0;
    iwfixed adjustSpeed = IW_FIXED(1);

#ifdef DO_AVOID_OTHER_CARS
    bool avoiding =
        AvoidOtherCars(
        car,
        &adjustHeading,
        &adjustSpeed);
#else
    bool avoiding = false;
#endif

    if (avoiding)
    {
        targetHeading += adjustHeading;
    }

#ifdef IW_DEBUG
    m_AdjustHeading = adjustHeading;
    m_TargetHeading = targetHeading;
#endif

    iwangle turn = CGeomUtils::GetShortestTurn(car->GetHeading(), targetHeading);
    ApplySteering(car, turn, abs(turn/5));

    iwfixed speed = 0;

    speed = IW_FIXED_MUL_SAFE(GetSpeedForTurn(turn), m_SpeedMod);

#ifdef IW_DEBUG
    iwfixed speedBefore = speed; //  DEBUG only
#endif

    speed = IW_FIXED_MUL_SAFE(speed, adjustSpeed);

    if (avoiding)
    {
        IwTrace(RCD_AI, ("AI CAR %d: old heading: %d, new heading: %d",
            m_DriverNum,
            IW_ANGLE_TO_DEGREES(headingBefore),
            IW_ANGLE_TO_DEGREES(targetHeading)));

        IwTrace(RCD_AI, ("AI CAR %d: target speed WAS: %d, is NOW: %d",
            m_DriverNum,
            speedBefore,
            speed));
    }

    car->SetTargetSpeed(speed);
}


#ifdef IW_DEBUG
void CAIDriver::DebugRender()
{
//  if (m_Car)
//  {
//      if (m_AvoidCarNum != -1)
//      {
//          char buffer[100];
//          sprintf(buffer, "Car %d:avoid:%d",
//              m_DriverNum,
//              m_AvoidCarNum);
//          IwGxPrintString(0, m_DriverNum * 8, buffer);
//      }
//
//      // Debug colour for this car
//      CIwColour col;
//      col.Set(0xff00ffff); // yellow
//
//      const CIwVec3 thisCarPos = m_Car->GetEnt()->GetPosLocal();
//
//      // Show Cone in which we look for cars to avoid
//      iwangle a = m_Car->GetHeading();
//
//      {
//          CIwVec3 heading(0, 0, 0);
//          heading.x = IwGeomSin(a + m_ConeAngle);
//          heading.z = -IwGeomCos(a + m_ConeAngle);
//          heading *= m_ConeLength;
//          heading += thisCarPos;
//          IwGxDebugPrimLine(thisCarPos, heading, col);
//      }
//
//      {
//          CIwVec3 heading(0, 0, 0);
//          heading.x = IwGeomSin(a - m_ConeAngle);
//          heading.z = -IwGeomCos(a - m_ConeAngle);
//          heading *= m_ConeLength;
//          heading += thisCarPos;
//          IwGxDebugPrimLine(thisCarPos, heading, col);
//      }
//
//      // Show target heading
//      col.Set(0xff0000ff); // red
//      {
//          CIwVec3 heading(0, 0, 0);
//          heading.x = IwGeomSin(m_TargetHeading);
//          heading.z = -IwGeomCos(m_TargetHeading);
//          // Make length of line proportional to speed
//          heading *= IW_FIXED_MUL_SAFE(m_Car->GetSpeedPerFrame(), IW_FIXED(0.1));
//          heading += thisCarPos;
//          IwGxDebugPrimLine(thisCarPos, heading, col);
//      }
//      // Show target position after adjusting to avoid other cars
//      // The position is shown as a line from the car so it's easier to spot.
//      col.Set(0xff00ff00); // green
//      IwGxDebugPrimLine(thisCarPos, m_AdjustedTarget, col);
//  }
}
#endif


bool CAIDriver::AvoidOtherCars(
    CCar* thisCar,
    iwfixed* adjustHeading,
    iwfixed* adjustSpeed)
{
#ifdef IW_DEBUG
    m_AvoidCarNum = -1; // not avoiding anyone
#endif
    bool avoidedSomething = false;
    const CIwVec3 thisCarPos = thisCar->GetEnt()->GetPosLocal();
    const iwangle heading = thisCar->GetHeading();


#ifdef IW_DEBUG
    m_AdjustedTarget = thisCarPos;
    m_AdjustedTarget.x += IW_FIXED_MUL_SAFE(IwGeomSin(heading), m_AdjustTargetLen);
    m_AdjustedTarget.z -= IW_FIXED_MUL_SAFE(IwGeomCos(heading), m_AdjustTargetLen);
#endif


    // Loop over all cars
    CIwManagedList& cars = GetWorld().GetCars();
    const uint32 s = cars.GetSize();
    for (uint32 c = 0; c < s; c++)
    {
        CCar* otherCar = (CCar*)((CIwEnt*)cars[c])->GetBehaviour();
        if (thisCar != otherCar)
        {
            // Is this car within range ?
            CIwVec3 otherCarPos = otherCar->GetEnt()->GetPosLocal();
            CIwVec3 between = otherCarPos - thisCarPos;
            iwfixed dist = between.GetLength();
            if (dist < m_ConeLength)
            {
                // Is this car's position within a cone in front of us ?
                otherCarPos -= thisCarPos; // Get Rel position

                // Get angle diff between other car bearing and this car's heading
                iwangle otherHeading = CGeomUtils::GetHeading(otherCarPos);
                iwangle diff = CGeomUtils::Normalise(heading) - CGeomUtils::Normalise(otherHeading);
                if (diff > IW_GEOM_HALF)
                {
                    diff = diff - IW_GEOM_ONE;
                }
                // If the other car is outside of this angle cone, ignore.
                // The cone gets wider the closer the other car is to us. This is so we can
                // avoid cars on the left and right.
                // m_ConeAngle = (1.0 - dist/m_ConeLength) * (m_MaxConeAngle - m_MinConeAngle) + m_MinConeAngle
                m_ConeAngle = IW_FIXED_MUL_SAFE(
                    IW_GEOM_ONE - IW_FIXED_DIV(dist, m_ConeLength),
                    (m_MaxConeAngle - m_MinConeAngle)) + m_MinConeAngle;

                IwAssert(RCD, m_ConeAngle >= m_MinConeAngle);
                IwAssert(RCD, m_ConeAngle <= m_MaxConeAngle);

                if (ABS(diff) > m_ConeAngle)
                {
                    continue;
                }
                avoidedSomething = true;
#ifdef IW_DEBUG
                m_AvoidCarNum = otherCar->GetDriver()->m_DriverNum;
#endif
                IwTrace(RCD_AI, ("AI CAR %d:    heading: %d, other car: %d, diff: %d",
                    m_DriverNum,
                    IW_ANGLE_TO_DEGREES(CGeomUtils::Normalise(heading)),
                    IW_ANGLE_TO_DEGREES(CGeomUtils::Normalise(otherHeading)),
                    IW_ANGLE_TO_DEGREES(diff)));

                // Adjust heading to avoid car.
                // Direction of adjustment is in opposite direction to angle diff.
                // Size of adjustment depends on how close other car is -
                // m_ConeAngle is narrower the further away the other car is.
                iwangle headingChange = m_MaxHeadingChange;

                if (diff < 0)
                {
                    headingChange = -headingChange;
                }
                // Check if new heading will take this car off the track -
                // If it will, then don't adjust the angle, so we will just slow down.
                iwangle finalHeading = thisCar->GetHeading() + headingChange;

                // Test the adjusted target to see if it leads off the track
                CIwVec3 adjustedTargetTest = thisCarPos;

                adjustedTargetTest.x += IW_FIXED_MUL_SAFE(IwGeomSin(finalHeading), m_AdjustTargetLen);
                adjustedTargetTest.z -= IW_FIXED_MUL_SAFE(IwGeomCos(finalHeading), m_AdjustTargetLen);

                // Check adjustedTarget to see if it's still on the track
                CTrackCollision::MoveInfo info;

                thisCar->MoveInfo(adjustedTargetTest, info);
#ifdef IW_DEBUG
                m_AdjustedTarget = adjustedTargetTest;
#endif
                if (info.m_OverTrack)
                {
                    // If avoiding more than one car, add heading changes together.
                    *adjustHeading += headingChange;
                }
                else
                {
                    // Go the other way
                    *adjustHeading -= headingChange;
                }

                // Let's only worry about avoiding one car.
                return true; // we avoided something
            }
        }
    }
    return avoidedSomething;
}

void CAIDriver::ApplySteering(CCar* car, int32 dsteer, int32 maxSteer)
{
    iwfixed steer = car->GetSteering();

    // If there is no delta steer but there is current steer then
    if (!dsteer && steer)
    {
        // Return steer to neutral.
        if (abs(steer) < (int)STEERING_RETURN )
            steer = 0;
        else
            steer += STEERING_RETURN * (steer > 0 ? -1 : 1);
    }
    else if (dsteer && !(abs(steer) >= maxSteer && (steer > 0) == (dsteer > 0)))
    {
        int32 steerdiff = 1;
        if (steer != 0 && (steer > 0) != (dsteer > 0) )
            steerdiff = 3;

        steer += STEERING_DIFF * (dsteer > 0 ? steerdiff : -steerdiff);
    }
    car->SetSteering(steer);
}

iwfixed CAIDriver::GetSpeedForTurn(iwangle turn)
{
    turn = abs(turn);
    if (turn > IW_ANGLE_2PI / 8)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.1));
    if (turn > IW_ANGLE_2PI / 16)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.2));
    if (turn > IW_ANGLE_2PI / 32)
        return IW_FIXED_MUL(MAX_SPEED, IW_FIXED(0.9));

    return MAX_SPEED;
}
