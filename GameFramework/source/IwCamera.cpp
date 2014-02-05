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
/*
 * Camera object with position (from ent frame) and FOV info
 */
#include "IwCamera.h"
#include "IwGx.h"
#include "IwGxState.h"
#include "IwTextParserITX.h"
#include "IwWorld.h"

IW_CLASS_FACTORY(CIwCamera);

//--------------------------------------------------------------------------------
// CIwCamera
//--------------------------------------------------------------------------------
IW_MANAGED_IMPLEMENT(CIwCamera)
CIwCamera::CIwCamera()
{
    IW_CALLSTACK("CIwCamera::CIwCamera")

    SetName("CIwCamera");

    // Camera allocates its own frame by default
    CIwFrame* pFrame = new CIwFrame;
    pFrame->Init();
    SetFrameOwned(pFrame);

    InitFromGx();
}
//--------------------------------------------------------------------------------
CIwCamera::~CIwCamera()
{
    IW_CALLSTACK("CIwCamera::~CIwCamera")

}
//--------------------------------------------------------------------------------
void    CIwCamera::Serialise()
{
    IW_CALLSTACK("CIwCamera::Serialise")

    CIwEnt::Serialise();
    IwSerialiseInt32(m_NearZ);
    IwSerialiseInt32(m_FarZ);
    IwSerialiseInt32(m_PerspMul);
}
//--------------------------------------------------------------------------------
void    CIwCamera::SetFOV(iwangle fov)
{
    IW_CALLSTACK("CIwCamera::SetFOV")

    int32 pm1 = IW_GEOM_COS(fov / 2);
    int32 pm3 = (IwGxGetScreenWidth() * pm1)/2;
    int32 pm2 =  IW_GEOM_SIN(fov / 2);
    int32 pm = pm3 / pm2;
    SetPerspMul(pm);
}
//--------------------------------------------------------------------------------
void    CIwCamera::InitFromGx()
{
    IW_CALLSTACK("CIwCamera::InitFromGx")

    m_NearZ     = IwGetGxState()->m_NearZ;
    m_FarZ      = IwGetGxState()->m_FarZ;
    m_PerspMul  = IwGetGxState()->m_PerspMul;
}
//--------------------------------------------------------------------------------
/*void CIwCamera::Clone(const CIwCamera& src)
{
    IW_CALLSTACK("CIwCamera::Clone")

    CIwEnt::Clone(*(CIwEnt*)&src);
    SetPerspMul(src.m_PerspMul);
    SetNearFar(src.m_NearZ, src.m_FarZ);
}*/
//--------------------------------------------------------------------------------
bool    CIwCamera::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwCamera::ParseAttribute")

#ifdef IW_BUILD_RESOURCES

    if (strcmp(pAttrName,"np") == 0)        // Near clip plane
    {
        pParser->ReadInt32(&m_NearZ);
    }
    else if (strcmp(pAttrName,"fp") == 0)   // Far clip plane
    {
        pParser->ReadInt32(&m_FarZ);
    }
    else if (strcmp(pAttrName,"fov") == 0)  // Field of view
    {
        iwangle fov;
        pParser->ReadAngleRadians(&fov);
        SetFOV(fov);
    }
    else if (strcmp(pAttrName, "rot") == 0) // Rotation
    {
        CIwEnt::ParseAttribute(pParser, pAttrName);

        // Convert from Maya camera coordinates to IwCamera coordinates
        // This is a 180 degree rotation about x
        ExposeMatLocal()->PreRotateX(IW_ANGLE_PI);
    }
    else
    {
        return CIwEnt::ParseAttribute(pParser, pAttrName);
    }
#endif // #ifdef IW_BUILD_RESOURCES

    return true;
}
//--------------------------------------------------------------------------------
void    CIwCamera::ParseClose(CIwTextParserITX* pParser)
{
    IW_CALLSTACK("CIwCamera::ParseClose")

#ifdef IW_BUILD_RESOURCES
    // CIwCamera is likely to be used as either a top-level or nested object
    // within the ITX file. If there is a parent object then make sure it is
    // notified, otherwise do nothing.

    IwAssertMsgN(304,UTIL, pParser, ("Null ptr"));
    CIwManaged* pParent = (CIwManaged*)pParser->GetObject(-1);

    if (pParent)
    {
        // Parent exists - Let the parent object decide what to do with this
        pParent->ParseCloseChild(pParser, this);
    }
    else
    {
        // No parent - Add the camera to the CIwWorld
        CIwWorld::GetContextSafe()->AddObject(this);
    }
#endif // #ifdef IW_BUILD_RESOURCES
}
//--------------------------------------------------------------------------------
void    CIwCamera::Update()
{
    IW_CALLSTACK("CIwCamera::Update")

    CIwEnt::Update();
}
//--------------------------------------------------------------------------------
