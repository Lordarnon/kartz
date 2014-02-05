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
#ifndef WORLD_H
#define WORLD_H

#include "IwWorld.h"
class CIwCamera;
class CIwGeom;
class CIwModel;
class CTrackCollision;
class CBestLine;
class CCar;
class CIwBehaviour;
class CDriver;
class CIwSoundInst;
class CIwSoundSpec;

#define FPS     20

#define DOUBLE_METRES_PER_UNIT (500.0)
#define WORLD_SCALE (IW_FIXED(1.0/DOUBLE_METRES_PER_UNIT))
#define OO_WORLD_SCALE (IW_FIXED(DOUBLE_METRES_PER_UNIT))

#define COUNTDOWN_DURATION  4*FPS

#define KPH_TO_MPF(_kph) (IW_FIXED(_kph) * 1000 / (60 * FPS))

class CWorld : public CIwWorld
{
public:
    IW_MANAGED_DECLARE(CWorld);

    CWorld();
    ~CWorld();

    void Create(const char* trackName);
    void Reset();
    virtual void Serialise();
    virtual void Resolve();

    void Update();
    void Render();

    void ToggleInCarCamera();
    void ToggleFlightCamera();

    int32               GetCountDownTicks() const       { return m_CountDownTicks; }
    CCar*               GetUICar()                      { return m_UICar; }
    CBestLine*          GetBestLine()                   { return m_BestLine; }
    CIwManagedList&     GetCars()                       { return m_Cars; }
    CTrackCollision*    GetCollision()                  { return m_Collision; }

    void SetBestLine(CBestLine* bestLine);
    void AddGeom(CIwGeom* geom);


#ifdef IW_DEBUG
    void DebugRender();
#endif

private:
    void CreateCollisionSurface(CIwModel* model);

    static CWorld* m_Instance;
    friend CWorld& GetWorld();

    CIwCamera* m_Camera;
    CIwBehaviour* m_ChaseCam;
    CIwBehaviour* m_InCarCam;
    CIwBehaviour* m_FreeFlightCam;

    CTrackCollision* m_Collision;
    CCar* m_UICar;
    CBestLine* m_BestLine;
    CIwManagedList m_Cars;
    CIwManagedList m_Ents;
    CIwManagedList m_Geoms;

    int32 m_CountDownTicks; // how much longer is left of countdown

    CIwSoundInst*   m_EngineSFX;
    CIwSoundInst*   m_SkidSFX;
    CIwSoundSpec*   m_CollisionSound;

#ifdef IW_DEBUG
    bool m_DebugRender;
#endif
};

inline CWorld& GetWorld()
{
    IwAssert(RCD, CWorld::m_Instance);
    return *CWorld::m_Instance;
}

void RegisterClassFactories();
void CreateWorld();

#endif /* !WORLD_H */
