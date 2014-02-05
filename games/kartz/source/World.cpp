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
#include "World.h"

#include "IwCamera.h"
#include "GeomModel.h"
#include "IwGraphics.h"
#include "IwModel.h"
#include "IwResManager.h"
#include "IwTextParserITX.h"
#include "IwMotionCamFree.h"
#include "ModelBlockCollisionMesh.h"
#include "TrackCollision.h"
#include "BestLine.h"
#include "GeomUtils.h"
#include "Car.h"
#include "ChaseCamera.h"
#include "AIDriver.h"
#include "HumanDriver.h"
#include "CarFactory.h"
#include "IwHorizon.h"
#include "InCarCamera.h"
#include "IwGx.h"
#include "IwSound.h"


//-------------------------------------------------------------------------------------------------

//During compilation to binary a model exists in IwModelBuilder in a readable format,

//But the compiled binary is in a format optimised for renderer, and unsuitable for game
//code to access.

//We solve this by plugging into the model build process and adding a
//CModelBlockCollisionMesh to the model's binary representation for any geometry we are
//interested in. In this example, the drivable area of the track.

//Then, when we load the model we can access the raw triangle information we saved
//using the model block interface, and convert it to a track collision post-load.
#if defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )

void TrackModelBuilderPostUserFn(const char* pString, CIwModel* pModel)
{
    if (strcmp(pString, "level") != 0 )
        return; //don't generate drivable area unless you're a track...

    CModelBlockCollisionMesh* pCollideMesh = new CModelBlockCollisionMesh();

    IW_MANAGED_LIST_ITERATE(pModel->m_Exts, itE)
    {
        if (strcmp((*itE)->GetClassName(), "CIwModelExtSelSetFace") != 0)
            continue; //only interested in face selection sets

        const char* name = (*itE)->DebugGetName();

        if (!name) continue;
        CIwModelExtSelSetFace* pSet = (CIwModelExtSelSetFace*)(*itE);

        if (!strcmp(name, "drivable"))
        {
            pCollideMesh->Add(*pSet, CModelBlockCollisionMesh::DRIVABLE_F);

            // we'll transform the set into a CModelBlockCollisionMesh, so remove the raw set from the model
            delete pSet;
            pModel->m_Exts.RemoveSlow(pSet);
            itE--;  // So we don't miss a set!
        }
    }

    pCollideMesh->FindCoincidentVertices((CIwSVec3*)IwGetModelBuilder()->m_Model->GetVerts(), IwGetModelBuilder()->m_Model->GetNumVerts());

    if (pCollideMesh->m_NumItems)
        pModel->AddBlock(pCollideMesh);
    else
        delete pCollideMesh;
}
#endif // defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )

//-------------------------------------------------------------------------------------------------

void RegisterClassFactories()
{
//register classes we need to serialise in/out. note that other standard classes
//are automatically registered during IwGraphicsInit()
    IW_CLASS_REGISTER(CModelBlockCollisionMesh);
    IW_CLASS_REGISTER(CWorld);
    IW_CLASS_REGISTER(CBestLine);
    IW_CLASS_REGISTER(CIwPathNurbs);
    IW_CLASS_REGISTER(CIwPathLine);
    IW_CLASS_REGISTER(CIwFrame);
    IW_CLASS_REGISTER_AB(CStatic, CGeomModel);
    IW_CLASS_REGISTER_AB(CCurve, CBestLine);
    IW_CLASS_REGISTER_AB(CPath, CBestLine);
    IW_CLASS_REGISTER(CGeomModel);
    CIwHorizon::RegisterClassFactories();

#if defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )
//register out plug in model parser (see above)
    if (IwGetResManager()->GetMode() == CIwResManager::MODE_BUILD)
    {
        IwGetModelBuilder()->SetPostBuildFn(TrackModelBuilderPostUserFn);

//initialise resource templates
//these provide default settings for imported resources which would otherwise require
//being set individually on each instance of the resource...
        IwGetTextParserITX()->ParseFile("resource_template.itx");
    }
#endif // defined (IW_DEBUG) && defined ( IW_BUILD_RESOURCES )
}

void CreateWorld()
{
//load the group file. This will:
//a) in debug build: create all resources from their 'source' representation
//and save their 'binary' representation
//or b) in release build: just load the 'binary' representation
    char trackName[S3E_CONFIG_STRING_MAX] = "";
    s3eConfigGetString("Game", "Track", trackName);
    if (!strlen(trackName) )
    {
        RCD_DATA_ERROR(("Must specify track name in icf: [Game] Track="));
    }
    char filename[256];
    sprintf(filename, "Tracks\\%s\\%s.group", trackName, trackName);
    IwGetResManager()->LoadGroup(filename);

//The world singleton will have been created by IwResManager during load...
    GetWorld().Create(trackName);
}

//-------------------------------------------------------------------------------------------------

CWorld* CWorld::m_Instance = NULL;

IW_CLASS_FACTORY(CWorld);
IW_MANAGED_IMPLEMENT(CWorld);

CWorld::CWorld()
: m_Camera(NULL)
#ifdef IW_DEBUG
, m_DebugRender(true)
#endif
, m_BestLine(NULL)
, m_Collision(NULL)
, m_ChaseCam(NULL)
, m_InCarCam(NULL)
, m_FreeFlightCam(NULL)
, m_EngineSFX(NULL)
, m_SkidSFX(NULL)

{
    IW_CALLSTACK("CWorld::CWorld");

    IwAssert(RCD, !m_Instance);
    m_Instance = this;
}

//-------------------------------------------------------------------------------------------------

CWorld::~CWorld()
{
    if (m_EngineSFX)
        m_EngineSFX->Stop();
    if (m_SkidSFX)
        m_SkidSFX->Stop();

    delete m_Collision;
    m_Instance = NULL;

    m_Cars.Delete();

    delete m_ChaseCam;
    delete m_InCarCam;
    delete m_FreeFlightCam;

    m_Ents.Delete();
    m_Geoms.Delete();

    delete m_BestLine;
}

//-------------------------------------------------------------------------------------------------

void CWorld::Create(const char* trackName)
{
    IW_CALLSTACK("CWorld::Create");

    if (!GetBestLine() )
        RCD_DATA_WARN(("Track file doesn't contain best line"));

    //find the track and create a collision surface from it.
    CIwModel* Track = (CIwModel*)IwGetResManager()->GetResNamed(trackName, IW_GRAPHICS_RESTYPE_MODEL);
    if (!Track)
        RCD_DATA_ERROR(("could not find track - geometry piece must be named track"));
    CreateCollisionSurface(Track);

    const char* paletteName[] =
    {
        "base",
        "green",
        "orange",
        "yellow",
    };

    //load the car file

    // Create player car
    // Note that the camera will follow the UI Car - set it to one of the AI players to watch them race...
    m_UICar = CCarFactory::Create(
        new CHumanDriver,
        paletteName[0]);

    for (int i = 0; i < 3; i++)
    {
        CAIDriver* driver = new CAIDriver(IW_FIXED(1.0));
#ifdef IW_DEBUG
        driver->SetDriverNum(i + 1);
#endif
        CCarFactory::Create(
            driver,
            paletteName[i + 1]
        );
    }

    Reset();

    //set up the camera
    CIwFrame* Frame = new CIwFrame;
    CIwMat mat;
    mat.LookAt(CIwVec3(IW_FIXED(-0.1), IW_FIXED(0.1), -IW_FIXED(0.5)), CIwVec3(0, 0, 0), CIwVec3(0, -IW_GEOM_ONE, 0));
    mat.t = CIwVec3(IW_FIXED(-0.1), IW_FIXED(0.1), -IW_FIXED(0.5));
    Frame->SetMatLocal(mat);

    CIwCamera* Camera   = new CIwCamera;
    m_ChaseCam          = new CChaseCamera(m_UICar->GetEnt());
    m_InCarCam          = new CInCarCamera(m_UICar->GetEnt());
    m_FreeFlightCam     = new CIwBeMotionCamFree();

    //n.b. we set the camera behaviour shared so the world can maintain ownership of it...
    Camera->SetBehaviourShared(m_ChaseCam);

    //if perspmul is not 200 the horizon cannot render using its fast path,
    //so only modify this if it's really going to look weird otherwise
    if (s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) < 240 )
        IwGxSetPerspMul(200);
    else
        IwGxSetPerspMul(200 * s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) / 208);

    Camera->InitFromGx();
    Camera->SetFrameOwned(Frame);
    m_Camera = Camera;
    m_Ents.Add(Camera);

    m_CollisionSound = (CIwSoundSpec*)IwGetResManager()->GetResNamed("collide", "CIwSoundSpec");
}

//-------------------------------------------------------------------------------------------------

static CIwVec3 FindTrackEdge(CCar* car, const CIwVec3& from, const CIwVec3& dir)
{
    CIwVec3 pos = from;
    CTrackCollision::MoveInfo info;

    //note we move in chunks to avoid finding e.g. the other side of a corner
    do
    {
        pos += dir;
        car->MoveInfo(pos, info, true);
    }
    while (info.m_OverTrack);

    return info.m_WallPosition;
}

//-------------------------------------------------------------------------------------------------

void CWorld::Reset()
{
    IW_CALLSTACK("CWorld::Reset");

    //place the player's car at the first face
    CCar*   playerCar       = (CCar*)(((CIwEnt*)m_Cars[0])->GetBehaviour());
    CIwVec3 homePosition    = m_Collision->GetCentroid(0);
    playerCar->Reset();

    //what does the best line look like here?
    playerCar->GetEnt()->GetFrame()->SetTransLocal(homePosition);
    playerCar->GetTracker().Update(homePosition);
    homePosition = playerCar->GetTracker().GetPoint();

    //if the best line curve leaves the drivable surface bad things will happen,so...
    CTrackCollision::MoveInfo info;
    playerCar->MoveInfo(homePosition, info, true);
    if (!info.m_OverTrack)
        homePosition = info.m_WallPosition + info.m_WallNormal;
    playerCar->GetEnt()->GetFrame()->SetTransLocal(homePosition);

    CIwVec3 homeDir = playerCar->GetTracker().GetDir();
    iwangle homeHeading = CGeomUtils::GetHeading(homeDir);

    //place cars across track...
    //get perp to best line
    homeDir = homeDir.Cross(CIwVec3(0, IW_GEOM_ONE, 0));
    homeDir *= IW_FIXED_MUL(IW_FIXED(10.0), WORLD_SCALE);

    //get left-hand and right-hand edges
    CIwVec3 right = FindTrackEdge(playerCar, homePosition, homeDir);
    CIwVec3 left = FindTrackEdge(playerCar, homePosition, -homeDir);

    right = (right - left) / IW_FIXED(m_Cars.GetSize()+1);

    for (uint32 c = 0; c < m_Cars.GetSize(); c++)
    {
        left += right;
        ((CIwEnt*)m_Cars[c])->GetFrame()->SetTransLocal(left);
        ((CCar*)((CIwEnt*)m_Cars[c])->GetBehaviour())->Reset();
        ((CCar*)((CIwEnt*)m_Cars[c])->GetBehaviour())->SetHeading(homeHeading);
    }

    m_CountDownTicks = COUNTDOWN_DURATION-1;

    CIwSoundGroup* music = (CIwSoundGroup*)IwGetResManager()->GetResNamed("music", "CIwSoundGroup");
    if (music)
        music->Stop();

    //gamesfx might be paused if we are restarting after a pause/exit
    CIwSoundGroup* gamesfx = (CIwSoundGroup*)IwGetResManager()->GetResNamed("gamesfx", "CIwSoundGroup");
    if (gamesfx)
         gamesfx->Resume();

    if (!m_EngineSFX)
    {
        CIwSoundSpec* engineSound = (CIwSoundSpec*)IwGetResManager()->GetResNamed("engine", "CIwSoundSpec");
        if (engineSound)
            m_EngineSFX = engineSound->Play();

        if (m_EngineSFX)
            m_EngineSFX->SetPitch(IW_GEOM_ONE/2);
    }

    if (!m_SkidSFX)
    {
        CIwSoundSpec* skidSound = (CIwSoundSpec*)IwGetResManager()->GetResNamed("skid", "CIwSoundSpec");

        if (skidSound)
            m_SkidSFX = skidSound->Play();
        if (m_SkidSFX)
            m_SkidSFX->SetVol(0);
    }

    //swallow any queued collision sfx
    m_UICar->PlayCollisionSFX();
}

//-------------------------------------------------------------------------------------------------

void CWorld::Serialise()
{
    CIwWorld::Serialise();

    bool hasBestLine = m_BestLine != NULL;
    IwSerialiseBool(hasBestLine);
    if (hasBestLine)
    {
        if (g_IwSerialiseContext.read)
            m_BestLine = new CBestLine;
        m_BestLine->Serialise();
    }
    m_Geoms.Serialise();
}

//-------------------------------------------------------------------------------------------------

void CWorld::Resolve()
{
    m_Geoms.Resolve();
}

//-------------------------------------------------------------------------------------------------

void CWorld::CreateCollisionSurface(CIwModel* model)
{
    const CIwSVec3* vertices = (const CIwSVec3*)model->GetVerts();
    m_Collision = new CTrackCollision(vertices, model->GetNumVerts());

    const uint32 hash = IwHashString("CModelBlockCollisionMesh");
    CIwManaged::ArrayIt ppExt = NULL;

    bool foundSurface = false;

    // walk the model blocks
    for (ppExt = NULL; (ppExt = model->m_Blocks.GetObjHashedNextIt(hash, ppExt)); )
    {
        CModelBlockCollisionMesh* pMesh = (CModelBlockCollisionMesh*)(*ppExt);
        for (uint32 t = 0; t < pMesh->m_NumItems; t++)
        {
            CModelBlockCollisionMesh::CIndexTri tri_i = pMesh->GetTriangle(t);

            if (tri_i.m_Flags & CModelBlockCollisionMesh::DRIVABLE_F)
            {
                foundSurface = true;
                m_Collision->AddTri(tri_i.m_Verts[0], tri_i.m_Verts[1], tri_i.m_Verts[2]);
            }
        }
    }

    if (!foundSurface)
    {
        RCD_DATA_ERROR(("Could not find drivable surface."));
    }

    // Clear temporary buffers
    m_Collision->FinishedAdd();
}

//-------------------------------------------------------------------------------------------------

void CWorld::Update()
{
    IW_CALLSTACK("CWorld::Update");

    IwGxTickUpdate();

    {
        IW_PROFILE("Update Cars");
        CIwEnt::UpdateList(m_Cars);
    }
    {
        IW_PROFILE("Update Entities");
        CIwEnt::UpdateList(m_Ents);
    }

    if (m_CountDownTicks)
    {
        m_CountDownTicks--;
    }

    CCar::DoCarCollisions(m_Cars);

    if (m_EngineSFX && !m_EngineSFX->IsPlaying() )
        m_EngineSFX->Resume();
    if (m_SkidSFX && !m_SkidSFX->IsPlaying() )
        m_SkidSFX->Resume();

    iwfixed speed = m_UICar->GetSpeedPerFrame();
    if (m_EngineSFX)
        m_EngineSFX->SetPitch((MAX(20000, abs(speed))-IwRand()%2000)/18);
    if (m_SkidSFX)
        m_SkidSFX->SetVol(IW_FIXED_MUL(speed/200, abs(m_UICar->GetSteering())*IW_GEOM_ONE/60));

    if (m_UICar->PlayCollisionSFX() )
    {
        m_CollisionSound->Play();
    }
}

//-------------------------------------------------------------------------------------------------

void CWorld::Render()
{
    IW_CALLSTACK("CWorld::Render");
    IW_PROFILE("World->Run");

    CTempBucketSwitch b(MB_GX);

    IwAssertMsg(RCD, m_Camera && m_Camera->GetMatLocal(),
                        ("Attempting to render with an undefined camera"));

    IwGxSetViewMatrix( m_Camera->GetMatLocal() );
    IwGxSetFarZNearZ( 0x8fff, IW_FIXED(0.01));
    IwGxLightingOff();

    if (IwGetHorizon() )
    {
        IW_PROFILE("Render->Horizon");
        IwGetHorizon()->Render();
    }

    IwGxLightingOff();
    {
        IW_PROFILE("Render->Geoms");

        // render all geoms
        IW_MANAGED_LIST_ITERATE(m_Geoms, itE)
        {
            ((CIwGeom*)*itE)->Render();
        }
    }
    {
        IW_PROFILE("Render->Cars");

        IW_MANAGED_LIST_ITERATE(m_Cars, itC)
        {
            ((CGeomModel*)*itC)->Render();
        }
    }
    IwGxLightingOn();

#ifdef IW_DEBUG
    if (m_DebugRender)
        DebugRender();

    m_BestLine->DebugRender();
#endif
}

//-------------------------------------------------------------------------------------------------

void CWorld::ToggleInCarCamera()
{
    if (m_Camera->GetBehaviour() == m_ChaseCam )
        m_Camera->SetBehaviourShared( m_InCarCam );
    else
        m_Camera->SetBehaviourShared( m_ChaseCam );
}

//-------------------------------------------------------------------------------------------------

void CWorld::ToggleFlightCamera()
{
    static CIwBehaviour* pPreviousCamera = NULL;

    if (m_Camera->GetBehaviour() == m_FreeFlightCam )
    {
        m_Camera->SetBehaviourShared( pPreviousCamera );
    }
    else
    {
        pPreviousCamera = m_Camera->GetBehaviour();
        m_Camera->SetBehaviourShared( m_FreeFlightCam );
    }
}

//-------------------------------------------------------------------------------------------------

void CWorld::SetBestLine(CBestLine* BestLine)
{
    IwAssertMsg(RCD, !m_BestLine, ("More than one best line in track file?"));

    m_BestLine = BestLine;
}

//-------------------------------------------------------------------------------------------------

void CWorld::AddGeom(CIwGeom* geom)
{
    m_Geoms.Add(geom);
}

//-------------------------------------------------------------------------------------------------

#ifdef IW_DEBUG
void CWorld::DebugRender()
{
    CIwWorld::DebugRender();

    IW_MANAGED_LIST_ITERATE(m_Cars, itO)
        (*itO)->DebugRender();
}
#endif

//-------------------------------------------------------------------------------------------------
