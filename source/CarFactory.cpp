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
#include "CarFactory.h"
#include "Car.h"
#include "Wheel.h"
#include "GeomModel.h"
#include "IwGraphics.h"
#include "IwModel.h"
#include "IwResManager.h"
#include "World.h"
#include "TrackCollision.h"
#include "WheelPos.h"
#include "Chassis.h"
#include "CarBody.h"
#include "IwGeomQuat.h"
#include "MemoryManager.h"

static const char* WHEEL_MODEL = "Wheel";

CCar* CCarFactory::Create(CDriver* theDriver,
                          const char* paletteName)
{
    IwAssert(RCD, paletteName);

    static CIwModel* WheelModel = 0;
    if (!WheelModel)
    {
        // Only need to load once, as the same model is used for every wheel.
        IwGetResManager()->LoadGroup("Car/goKart/goKart.group");

        WheelModel = (CIwModel*)IwGetResManager()->
            GetResNamed(WHEEL_MODEL, IW_GRAPHICS_RESTYPE_MODEL);
        IwAssert(RCD, WheelModel);

        // Get the wheel radius from the model. This is only approximately correct (because the BSphere has one dimension too many!)
        iwfixed radius = WheelModel->GetBSphere()->r;
        CWheel::SetRadius(radius);
    }

    CChassis carChassis;

    // Load the .geo for this car body
    CIwModel* CarModel = (CIwModel*)IwGetResManager()->
        GetResNamed(carChassis.GetBody().GetGeoName(), IW_GRAPHICS_RESTYPE_MODEL);
    IwAssert(RCD, CarModel);

    CGeomModel* CarGeom = new CGeomModel( new CShadow );

    IwMemBucketPush(MB_GX);

    //Clone materials for this racer
    CIwModel* pClonedModel = CarModel->CloneWithNewMaterials();

    IwMemBucketPop();

    const char* materialNames[] =
    {
        "gokart_materials/overaltop_",
        "gokart_materials/arm_",
        NULL,
        "gokart_materials/seat_",
        "gokart_materials/seatback_",
        NULL,
        "gokart_materials/plastic_",
        NULL,
        NULL,
        "gokart_materials/frontup_",
        NULL,
        NULL,
        "gokart_materials/bashhat_",
    };

    char szNameBuffer[256];

    for (uint32 currentMaterial = 0; currentMaterial < pClonedModel->m_Materials.GetSize(); ++currentMaterial)
    {
        if (materialNames[currentMaterial])
        {
            sprintf(szNameBuffer, "%s%s", materialNames[currentMaterial], paletteName);

            ( (CIwMaterial*)pClonedModel->m_Materials[ currentMaterial ] )->Copy(
             *(CIwMaterial*) IwGetResManager()->GetResNamed(szNameBuffer, IW_GX_RESTYPE_MATERIAL)
            );
        }
    }


    // Use CIwFrameLW so we can add Wheels for children
    CIwFrameLW* CarFrame = new CIwFrameLW;
    CarGeom->SetFrameOwned(CarFrame);
    CIwMat mat = CIwMat::g_Identity;
    CarGeom->GetFrame()->SetMatLocal(mat);
    CarGeom->m_Model = pClonedModel;
    GetWorld().GetCars().Add(CarGeom);

    CCar* theNewCar = new CCar(theDriver, pClonedModel);
    CarGeom->SetBehaviour(theNewCar);
    CIwVec3 worldpos = GetWorld().GetCollision()->GetCentroid(0);
    CarGeom->GetFrame()->SetTransLocal(worldpos);


    for (int i = 0; i < 4; i++)
    {
        CIwGeomModel* WheelGeom = new CGeomModel;
        CIwFrameLW* WheelFrame = new CIwFrameLW;
        WheelGeom->SetFrameOwned(WheelFrame);
        WheelGeom->GetFrame()->SetMatLocal(CIwMat::g_Identity);

        // Set position of this wheel on the car.
        const CWheelPos& wheelpos = carChassis.GetWheelPos(i);
        CIwVec3 pos = wheelpos.GetPos();

        pos >>= IW_GEOM_POINT; // Erm, converted to fixed point one time too many ?

        WheelFrame->ExposeMatLocal()->t = pos;

        WheelGeom->m_Model = WheelModel;
        GetWorld().AddGeom(WheelGeom);

        CarFrame->AddChild(WheelFrame);

        // Front wheels turn when car steers
        bool steers = (pos.z > 0);
        CWheel* Wheel = new CWheel(theNewCar, steers);

        WheelGeom->SetBehaviour(Wheel);

        // Add the wheel to the car so it gets updated
        theNewCar->AddWheel(i, Wheel);
    }

    return theNewCar;
}
