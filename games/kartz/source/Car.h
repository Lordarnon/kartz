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
#ifndef CAR_H
#define CAR_H

#include "IwBehaviour.h"
#include "TrackCollision.h"
#include "BestLineTracker.h"
#include "IwManagedList.h"

class CDriver;
class CWheel;
class CIwModel;

class CCar : public CIwBehaviour
{
public:
    CCar(CDriver* driver, CIwModel* pModel);
    ~CCar();
    virtual void Update();
    void Reset();

    iwfixed GetSteering() { return m_Steering; }
    iwfixed GetTargetSpeed() { return m_TargetSpeed; }
    void SetSteering(iwfixed steer) { m_Steering = steer; }
    void SetTargetSpeed(iwfixed speed) { m_TargetSpeed = speed; }

    void SetHeading(iwangle heading) { m_Heading = heading; }
    iwangle GetHeading() { return m_Heading; }
    iwfixed GetSpeedKmh() const;
    iwfixed GetSpeedPerFrame() const;
    int32 GetPosition() const;

    const CBestLineTracker& GetTracker() const { return m_BestLineTracker; }
    CBestLineTracker& GetTracker() { return m_BestLineTracker; }

    int16 MoveInfo(const CIwVec3& toPos, CTrackCollision::MoveInfo& out, bool updatePoly = false);

    void AddWheel(int wheelNum, CWheel*);

    bool PlayCollisionSFX();

#ifdef IW_DEBUG
    virtual void DebugRender();
    // Get Driver, so we can get ID of car we are avoiding, etc for debugging
    CDriver* GetDriver() { return m_Driver; }
#endif

    static void DoCarCollisions(CIwManagedList& cars);

private:
    void DoMove();
    void DoSteering();
    void DoAcceleration();
    void DoOrient(const CIwVec3& newPos, const CIwVec3& norm);

    CDriver* m_Driver; //who's driving this car
    iwfixed m_Steering; //how much are we turning (per frame)
    iwfixed m_TargetSpeed; //how fast do we want to go?

    iwangle m_Heading;
    CIwVec3 m_Momentum;

    bool m_OnGround;

    bool m_PlayCollisionSFX;

    int32 m_TrackPoly;

    CBestLineTracker m_BestLineTracker;

    CWheel*     m_Wheels[4];
    CIwModel*   m_pModel;
};

#endif /* !CAR_H */
