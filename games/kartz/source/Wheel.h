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
#ifndef WHEEL_H
#define WHEEL_H

#include "IwBehaviour.h"

class CCar;

class CWheel : public CIwBehaviour
{
public:
    CWheel(
        CCar* theCar, // the car to which this wheel is attached
        bool isSteerable); // does this wheel turn when the car is steered

    virtual void Update();

    CCar* GetCar() { return m_Car; }

    static void SetRadius(iwfixed r) { s_Radius = r; }
    static iwfixed GetRadius() { return s_Radius; }

private:
    // If true, wheel can be rotated about vertical axis independently of the car
    bool m_IsSteerable;

    // Spring for suspension: we want wheels to move up and down on the car frame.
    struct Suspension
    {
        iwfixed m_MinLength; // spring fully squashed
        iwfixed m_MaxLength; // spring fully stretched
    };

    // The car to which this wheel is joined
    CCar* m_Car;

    iwangle m_Roll; // rotation around wheel axle

    static iwfixed s_Radius;
};

#endif /* !WHEEL_H */
