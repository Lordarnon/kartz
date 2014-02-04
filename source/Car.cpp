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
#include "Car.h"

#include "World.h"
#include "IwEnt.h"
#include "Driver.h"
#include "GeomUtils.h"
#include "Wheel.h"
#include "IwModel.h"

#define COEFF_GRIP IW_FIXED(0.7)

CCar::CCar(CDriver* driver, CIwModel* pModel)
: m_Driver(driver)
, m_Heading(0)
, m_Steering(0)
, m_TargetSpeed(0)
, m_Momentum(0,0,0)
, m_TrackPoly(-1)
, m_OnGround(true)
, m_pModel( pModel )
{
    for (int i = 0; i < 4; i++)
    {
        m_Wheels[i] = 0;
    }
}


CCar::~CCar()
{
    delete m_Driver;
    delete m_pModel;
}


void CCar::Update()
{
    if (m_Driver)
        m_Driver->Update(this);

    if (!GetWorld().GetCountDownTicks() )
    {
        DoSteering();
        DoAcceleration();
    }

    m_Momentum *= IW_FIXED(0.99); //friction (better to come);
    m_Momentum.y -= IW_FIXED(0.98)*25/20;

    DoMove();

    for (int i = 0; i < 4; i++)
    {
        if (m_Wheels[i])
        {
            m_Wheels[i]->Update();
        }
    }
}


void CCar::Reset()
{
    m_BestLineTracker = CBestLineTracker();
    m_TrackPoly = -1;
    m_Momentum = CIwVec3(0,0,0);
}


#define CAR_RADIUS IW_FIXED_MUL(IW_FIXED(32.0), WORLD_SCALE)

static bool GetCorrection(CCar* car, const CIwVec3&pos, CIwVec3& correct)
{
    CTrackCollision::MoveInfo info;
    car->MoveInfo(pos, info);

    bool needsCorrect = false;

    if (!info.m_OverTrack)
    {
        correct -= (pos - info.m_WallPosition);
        needsCorrect = true;
    }

    if (pos.y < info.m_Height)
    {
        correct.y += info.m_Height - pos.y;
        needsCorrect = true;
    }

    return needsCorrect;
}


void CCar::DoCarCollisions(CIwManagedList& cars)
{
    IW_PROFILE("Car Collisions");

    for (uint32 c = 0; c < cars.GetSize(); c++)
    {
        CCar* car1 = (CCar*)((CIwEnt*)cars[c])->GetBehaviour();

        for (uint32 c2 = c+1; c2 < cars.GetSize(); c2++)
        {
            CCar* car2 = (CCar*)((CIwEnt*)cars[c2])->GetBehaviour();

            if (car1 != car2)
            {
                CIwVec3 Car1Pos = car1->GetEnt()->GetPosLocal();
                CIwVec3 Car2Pos = car2->GetEnt()->GetPosLocal();

                CIwVec3 between = Car2Pos - Car1Pos;
                if (between.GetLength() < CAR_RADIUS )
                {
                    if (between.IsZero() )
                        between = CIwVec3(1,0,0);
                    //correct positions
                    CIwVec3 centre = between / IW_FIXED(2.0);
                    centre += Car1Pos;

                    between.Normalise();

                    CIwVec3 correct = between * (CAR_RADIUS/2);
                    Car1Pos = centre - correct;
                    Car2Pos = centre + correct;

                    //if either is going to be pushed off the drivable surface we
                    //need to bring both back on...
                    correct = CIwVec3(0,0,0);
                    if (GetCorrection(car1, Car1Pos, correct) ||
                        GetCorrection(car2, Car2Pos, correct) )
                    {
                        Car1Pos += correct;
                        Car2Pos += correct;
                    }

                    car1->GetEnt()->GetFrame()->SetTransLocal(Car1Pos);
                    car2->GetEnt()->GetFrame()->SetTransLocal(Car2Pos);

                    //simple collision: cars have the same mass and are cylindrical
                    iwfixed impulse = (car1->m_Momentum - car2->m_Momentum).Dot(between);
                    car1->m_Momentum -= between * impulse;
                    car2->m_Momentum += between * impulse;

                    car1->m_PlayCollisionSFX = true;
                    car2->m_PlayCollisionSFX = true;
                 }

            }
        }

    }
}


static iwfixed ClipMomentum(CIwVec3& mom, const CIwVec3& norm, iwfixed rebound)
{
    //component perpendicular to collided surface
    iwfixed forceOfRestitution = mom.Dot(norm);

    if (forceOfRestitution < 0)
    {
        forceOfRestitution = IW_FIXED_MUL(-forceOfRestitution, rebound);
        mom += norm * forceOfRestitution;
    }
    return forceOfRestitution;
}


void CCar::DoMove()
{
    CIwVec3 pos = m_Ent->GetPosLocal();

    CIwVec3 newPos = pos + m_Momentum * WORLD_SCALE;

    CIwVec3 normal;

    CTrackCollision::MoveInfo info;
    m_TrackPoly = MoveInfo(newPos, info);

    info.m_Height += 20;

    if (!info.m_OverTrack)
    {
        iwfixed force = ClipMomentum(m_Momentum, -info.m_WallNormal, IW_FIXED(1.3));
        if (force > 20000)
            m_PlayCollisionSFX = true;


        if (m_Momentum.GetLengthSquaredSafe() )
        {
            m_Heading += CGeomUtils::GetShortestTurn(m_Heading, CGeomUtils::GetHeading(m_Momentum)) / 5;
        }
        newPos.x = info.m_WallPosition.x;
        newPos.z = info.m_WallPosition.z;
    }

    int32 height = newPos.y - info.m_Height;

    m_OnGround = height <= 0;

    if (height < 0)
    {
        newPos.y -= height;

        ClipMomentum(m_Momentum, info.m_GroundNormal, IW_FIXED(1.4));
    }

    CIwVec3 gripvec = m_Ent->GetFrame()->GetMatLocal()->RowX();
    iwfixed grip = IW_FIXED_MUL(m_Momentum.Dot(gripvec), COEFF_GRIP);
    m_Momentum -= gripvec * grip;

    if (m_OnGround)
    {
        DoOrient(newPos, info.m_GroundNormal);
    }
    else
    {
        DoOrient(newPos, m_Ent->GetFrame()->GetMatLocal()->RowY());
    }

    m_BestLineTracker.Update(newPos);
}

void CCar::DoSteering()
{
    if (m_OnGround)
    {
        m_Heading += iwfixed(m_Steering / 1.5);
        m_Heading = m_Heading % IW_ANGLE_2PI;
    }
}


void CCar::DoAcceleration()
{
    if (m_OnGround)
    {
        iwfixed speed = m_Momentum.GetLength();

        // If we have not reached the target speed the keep accelerating
        if (speed < m_TargetSpeed)
        {
            m_Momentum += m_Ent->GetFrame()->GetMatLocal()->RowZ() * IW_FIXED(1.0);
        }
        else if ((m_TargetSpeed < 0) && (speed > m_TargetSpeed) )
        {
            m_Momentum -= m_Ent->GetFrame()->GetMatLocal()->RowZ() * IW_FIXED(1.0);
        }
    }
}


iwfixed CCar::GetSpeedKmh() const
{
    iwfixed speedPerFrame = m_Momentum.GetLength() ;

    return  speedPerFrame
            * FPS // m/s
            * 60 // m/h
            / 1000; // km/h
}

#ifdef IW_DEBUG
void CCar::DebugRender()
{
    m_BestLineTracker.DebugRender();
    m_Driver->DebugRender();
}
#endif

iwfixed CCar::GetSpeedPerFrame() const
{
    CIwVec3 mom = m_Momentum;
    mom.y = 0;
    return mom.GetLength();
}


int32 CCar::GetPosition() const
{
    CIwManagedList& cars = GetWorld().GetCars();

    int32 position = 1;
    iwfixed myprogress = GetTracker().GetDistance(GetEnt()->GetPosLocal());

    IW_MANAGED_LIST_ITERATE(cars, itC)
    {
        CCar* car = (CCar*)((CIwEnt*)(*itC))->GetBehaviour();

        if (car != this && car->GetTracker().GetDistance(car->GetEnt()->GetPosLocal()) > myprogress )
            position++;
    }

    return position;
}


void CCar::DoOrient(const CIwVec3& newPos, const CIwVec3& norm)
{
    CIwMat mat(CIwMat::g_Identity);
    mat.SetRotY(m_Heading);
    CIwVec3 forwards = mat.RowX().Cross(norm);
    CIwVec3 right = forwards.Cross(norm);

    forwards.Normalise();
    right.Normalise();

    *(CIwVec3*)&mat.m[0][0] = right;
    *(CIwVec3*)&mat.m[1][0] = norm;
    *(CIwVec3*)&mat.m[2][0] = -forwards;
    mat.t = newPos;

    mat.InterpRot(*m_Ent->GetFrame()->GetMatLocal(), mat, IW_FIXED(0.5));

    m_Ent->GetFrame()->SetMatLocal(mat);
}


int16 CCar::MoveInfo(const CIwVec3& toPos, CTrackCollision::MoveInfo& out, bool updatePoly)
{
    // Find out where we are one the floor.
    CIwVec3 pos = m_Ent->GetPosLocal();
    int poly = GetWorld().GetCollision()->MoveFloor(m_TrackPoly, pos, toPos, 0, pos.y, out);

    if (updatePoly)
        m_TrackPoly = poly;

    // Check for a collision with the walls of the racing track.
    pos = toPos;
    CIwVec3 normal;
    if (GetWorld().GetCollision()->WallPush(m_TrackPoly, pos, normal, m_Ent->m_BSphere.GetRadius() ) )
    {
        out.m_WallPosition  = pos;
        out.m_OverTrack     = false;
        out.m_WallNormal    = normal;
    }

    return poly;
}

void CCar::AddWheel(int i, CWheel* theNewWheel)
{
    IwAssert(RCD, i < 4);
    m_Wheels[i] = theNewWheel;
}


bool CCar::PlayCollisionSFX()
{
    bool ret = m_PlayCollisionSFX;
    m_PlayCollisionSFX = false;
    return ret;
}
