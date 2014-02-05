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
 * Background textured horizon model
 */

#include "IwOptimiseSpeed.h"
#include "IwOptimiseARM.h"

// Includes
#include "IwHorizon.h"
#include "IwClassFactory.h"
#include "IwGx.h"
#include "IwGxState.h"
#include "IwMaterial.h"
#include "IwModel.h"
#include "IwRenderManager.h"
#include "IwResManager.h"
#include <stdlib.h>

IW_SINGLETON_INSTANTIATE(Horizon);

IW_CLASS_FACTORY(CIwHorizon);
IW_MANAGED_IMPLEMENT(CIwHorizon)
CIwHorizon::CIwHorizon()
{
    IW_CALLSTACK("CIwHorizon::CIwHorizon")

    m_InvYScale = IW_FIXED(1.0);
    if (g_IwHorizon == NULL)
        g_IwHorizon = this;
    m_Spin = 0;
    m_SpinSpeed = 0;
    m_NumWraps = 1;
    m_FillAbove = 0;
    m_FillBelow = 0;
    m_Flags = 0;
    m_XOffset = 0;

    m_AboveMaterial = new CIwMaterial;
    m_AboveMaterial->SetShadeMode(CIwMaterial::SHADE_FLAT);
    m_AboveMaterial->SetColEmissive(0);

    m_BelowMaterial = new CIwMaterial;
    m_BelowMaterial->SetShadeMode(CIwMaterial::SHADE_FLAT);
    m_BelowMaterial->SetColEmissive(0);
}
//--------------------------------------------------------------------------------
CIwHorizon::~CIwHorizon()
{
    IW_CALLSTACK("CIwHorizon::~CIwHorizon")

    if (g_IwHorizon == this)
        g_IwHorizon = NULL;

    delete m_AboveMaterial;
    delete m_BelowMaterial;
}
//--------------------------------------------------------------------------------
void    CIwHorizon::RegisterClassFactories()
{
    IW_CLASS_REGISTER(CIwHorizon);
}
//--------------------------------------------------------------------------------
void    CIwHorizon::Render()
{
    IW_CALLSTACK("CIwHorizon::Render")

    IW_PROFILE("HorzRender");
    const CIwMat* pMat = &IwGetGxState()->m_MatView;

    // The roll calculation can only be used accurately on upright orientations.
    // For this reason we rotate the matrices to compensate for screen orientation.
    // It is essential that this is exact for zero roll only (see the check below).
    CIwMat mat = IwGetGxState()->m_MatView;
#ifdef I3D_ARCH_ARM
    if (!(IwGxGetFlags() & IW_GX_RASTERISATION_HW_F))
#endif
    {
        CIwMat rotZ;
        switch(0)//  apps should not worry about this! //IwGxGetScreenOrient())
        {
        case IW_GX_ORIENT_NONE:
            break;

        default:
            rotZ.SetRotZ(-IwGxGetScreenOrient() * 0x400, false, true);
            mat.PreRotate(rotZ);
            break;
        }
    }
    const CIwMat* pMat2 = &mat;
    m_CameraYaw     = -IW_GEOM_ATAN2(pMat->m[2][0], pMat->m[2][2]);

    m_CameraYaw += m_Spin;
    m_CameraYaw &= 0xfff;

    iwfixed xz      = IW_GEOM_SQRT2(SQR(pMat->m[2][0]) + SQR(pMat->m[2][2])) >> IW_GEOM_POINT;
    m_CameraPitch   = IW_GEOM_ATAN2(IW_FIXED_MUL(pMat->m[2][1],m_InvYScale), xz);//IW_GEOM_SQRT2((1<<24) - SQR(pMat->m[2][1]))>> IW_GEOM_POINT); //arcsin(pMat->m[2][1])
    //iwfixed test = SQR(pMat->m[2][0]) + SQR(pMat->m[2][2]) + SQR(pMat->m[2][1]);
    iwfixed xz2     = IW_GEOM_SQRT2(SQR(pMat2->m[0][0]) + SQR(pMat2->m[0][2])) >> IW_GEOM_POINT;

    // Get the camera roll from the model-view matrix
    m_CameraRoll = IW_GEOM_ATAN2(pMat2->m[0][1], xz2);

//  // Remember though that the model-view will already have been rotated if in landscape... UNLESS
//  // on a device like N93
//#ifdef I3D_ARCH_ARM
//  if (!(IwGxGetFlags() & IW_GX_RASTERISATION_HW_F))
//#endif
//  {
//      m_CameraRoll -= IwGxGetScreenOrient() * 0x400;
//  }

    if (pMat2->m[1][1] > 0)
        m_CameraRoll = IW_ANGLE_FROM_DEGREES(180)-m_CameraRoll;

    // Straighten for nearly aligned.
    // Note: This may cause snapping.
    if (ABS(m_CameraRoll) < IW_ANGLE_FROM_DEGREES(2))
        m_CameraRoll = 0;

    iwfixed scale = IW_FIXED(1.0);

    int numScreenPixels = (int16)i3d_sdiv(((IwGetGxState()->m_DisplayWidth/2))*IW_ANGLE_FROM_DEGREES(360),
                            IW_GEOM_ATAN2(IwGetGxState()->m_DisplayWidth/2, IwGetGxState()->m_PerspMul));

    int32 totalWidth = m_TileWidth * m_NumPerRow;
    if (m_Flags & AUTO_SCALE_F)
    {
        int numPixelsWanted = numScreenPixels;
        scale = i3d_sdiv((numPixelsWanted<<IW_GEOM_POINT), totalWidth * m_NumWraps);
    }

    scale = i3d_sdiv(IwGetGxState()->m_PerspMul*scale, 200);

    uint32 perspMul;
    if (m_Flags & USE_FIXED_FOV_F)
        perspMul = m_FakePerspMul;
    else
        perspMul = IwGetGxState()->m_PerspMul;

    //This calculation works out how many pixels there are per degree of yaw by
    //looking at the screen width and fov
    //(half screen width div half fov angle) * camera yaw - calcs the x-offset of the horizon
    //int16 offset_x = (int16)i3d_sdiv(((IwGetGxState()->m_DisplayWidth/2)<<3)*pHorizon->m_CameraYaw,
    //                      IW_GEOM_ATAN2(IwGetGxState()->m_DisplayWidth/2, perspMul));

    //However horizontally a horizon must do a complete 360 if the camera does. Its no
    //good if after 359.99 degrees of camera rotation if you've only scrolled your horizon half way.
    //therefore calculate horizontal offset by:
    //number of pixels offset = total pixels in horizon * (camera yaw/360)

    //if this doesn't match the calculation above then your horizon will not appear
    //to move at the same 'speed' as objects in the background

    int totalWidthScaled = IW_FIXED_MUL(totalWidth,scale);

    int16 offset_x = (int16)(-m_XOffset + IW_FIXED_MUL(totalWidthScaled, (m_CameraYaw<<IW_GEOM_POINT) / IW_ANGLE_FROM_DEGREES(360)));
    offset_x -= int16(IwGetGxState()->m_DisplayWidth/2);
    if (offset_x < 0) offset_x += totalWidthScaled;

    //for the vertical component it doesnt wrap around vertically so always move it
    //at the correct rate - use the FOV calculation
    int16 offset_y = (int16)i3d_sdiv(((IwGetGxState()->m_DisplayHeight/2))*m_CameraPitch,
                            IW_GEOM_ATAN2(IwGetGxState()->m_DisplayHeight/2, perspMul));

    CIwSVec2 v = CIwSVec2::g_Zero;
//  if (GetCinemascope())
//      v.y += CINEMASCOPE_HEIGHT;

    IwGxSetScreenSpaceOrg(&v);
    IwGxSetScreenSpaceSlot(-2);

    if (m_CameraRoll == 0 && scale == IW_FIXED(1.0) )
        RenderFastPath(offset_x, offset_y);
    else
        RenderSlowPath(scale, offset_x, offset_y);

    IwGxSetScreenSpaceSlot(0);
}
//--------------------------------------------------------------------------------
#ifdef IW_DEBUG
    #include "IwTextParserITX.h"
    #include "IwHorizonBuilder.h"

    struct HorizonBuilder
    {
        int32 offset, xoffset, tileW, tileH;
        char imageFile[256];
        bool fillAbove, fillBelow;

        void Clear()
        {
            imageFile[0] = '\0';
            offset = 0;
            xoffset = 0;
            tileW = tileH = 32;
            fillAbove = false;
            fillBelow = false;
        }
    };
#ifdef IW_BUILD_RESOURCES
    static HorizonBuilder *horizonBuilder = 0;
#endif // #ifdef IW_BUILD_RESOURCES

    void CIwHorizon::ParseOpen(CIwTextParserITX* pParser)
    {
        IW_CALLSTACK("CIwHorizon::ParseOpen")

#ifdef IW_BUILD_RESOURCES

        IwAssert(ENGINE, horizonBuilder == NULL);
        horizonBuilder = new HorizonBuilder();
        horizonBuilder->Clear();

#endif // #ifdef IW_BUILD_RESOURCES
    }
    bool CIwHorizon::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
    {
        IW_CALLSTACK("CIwHorizon::ParseAttribute")

#ifdef IW_BUILD_RESOURCES
        if (strcmp(pAttrName, "offset") == 0)
        {
            pParser->ReadInt32(&horizonBuilder->offset);
        }
        else if (strcmp(pAttrName, "xoffset") == 0)
        {
            pParser->ReadInt32(&horizonBuilder->xoffset);
        }
        else if (strcmp(pAttrName, "tileW") == 0)
        {
            pParser->ReadInt32(&horizonBuilder->tileW);
        }
        else if (strcmp(pAttrName, "tileH") == 0)
        {
            pParser->ReadInt32(&horizonBuilder->tileH);
        }
        else if (strcmp(pAttrName, "image") == 0)
        {
            pParser->ReadString(horizonBuilder->imageFile, sizeof(horizonBuilder->imageFile));
        }
        else if (strcmp(pAttrName, "fillAbove") == 0)
        {
            pParser->ReadBool(&horizonBuilder->fillAbove);
        }
        else if (strcmp(pAttrName, "fillBelow") == 0)
        {
            pParser->ReadBool(&horizonBuilder->fillBelow);
        }
        else
            return CIwResource::ParseAttribute(pParser, pAttrName);
#endif // #ifdef IW_BUILD_RESOURCES

        return true;
    }
    void    CIwHorizon::ParseClose(CIwTextParserITX* pParser)
    {
        IW_CALLSTACK("CIwHorizon::ParseClose")

#ifdef IW_BUILD_RESOURCES

        if (m_Hash == 0)
            IwAssertMsgN(325,ENGINE, g_IwHorizon == this, ("Attempting to instantiate more than one horizon without giving them names."));

        CIwHorizonBuilder* builder = new CIwHorizonBuilder;
        builder->CreateHorizon( this, horizonBuilder->imageFile, "horizon",
                                horizonBuilder->offset, horizonBuilder->tileW, horizonBuilder->tileH,
                                horizonBuilder->fillAbove, horizonBuilder->fillBelow);
        m_XOffset = horizonBuilder->xoffset;

        delete horizonBuilder;
        horizonBuilder = 0;
        delete builder;

        IwGetResManager()->AddRes("CIwHorizon", this);

#endif // #ifdef IW_BUILD_RESOURCES
    }
#endif

//--------------------------------------------------------------------------------
void    CIwHorizon::Resolve()
{
    IW_CALLSTACK("CIwHorizon::Resolve")

    for (int32 i = 0; i < m_NumRows * m_NumPerRow; i++)
        IwResolveManagedHash(&m_Tiles[i].m_Material, IW_GX_RESTYPE_MATERIAL);
}
//--------------------------------------------------------------------------------
void    CIwHorizon::SetYScale(iwfixed scale)
{
    IW_CALLSTACK("CIwHorizon::SetYScale")

    m_InvYScale = scale;
}
//--------------------------------------------------------------------------------
void CIwHorizon::CTile::Serialise()
{
    IwSerialiseManagedHash(&m_Material);
    IwSerialiseBool(m_Mirror);
}
//--------------------------------------------------------------------------------
void    CIwHorizon::Serialise()
{
    IW_CALLSTACK("CIwHorizon::Serialise")

    CIwManaged::Serialise();

    IwSerialiseInt32(m_TileWidth);
    IwSerialiseInt32(m_TileHeight);
    IwSerialiseInt32(m_NumPerRow);
    IwSerialiseInt32(m_NumRows);
    IwSerialiseInt32(m_XOffset);
    IwSerialiseInt32(m_YOffset);

    int32 numTiles = m_NumRows * m_NumPerRow;

    if (g_IwSerialiseContext.read)
    {
        m_Tiles.resize(numTiles);
    }

    for (int32 i = 0; i < numTiles; i++)
    {
        m_Tiles[i].Serialise();
    }

    IwSerialiseUInt32(m_Flags);
    IwSerialiseUInt32(m_FakePerspMul);

    m_FillAbove.Serialise();
    m_FillBelow.Serialise();
}
//--------------------------------------------------------------------------------
inline void RotatePoint(CIwSVec2& point, iwfixed sina, iwfixed cosa, const CIwVec2& centre)
{
    IW_CALLSTACK("CIwBg2DBitmap::RotatePoints")

    point -= centre;
    int oldx = point.x;
    point.x = (int16)(IW_FIXED_MUL(point.x,cosa) - IW_FIXED_MUL(point.y,sina));
    point.y = (int16)(IW_FIXED_MUL(oldx,sina) + IW_FIXED_MUL(point.y,cosa));
    point += centre;
}
//--------------------------------------------------------------------------------
static void FillScreen(int32 t, int32 b, CIwColour col, CIwMaterial* pMaterial)
{
    t = MAX(0, t);
    b = MIN((int32) IwGetGxState()->m_DisplayHeight, b);

    if (0)// please don't do this yourself
        //the renderer is faster
        // !(IwGxGetFlags() & IW_GX_RASTERISATION_HW_F) )
    {
        uint32 surcol = s3eSurfaceConvertRGB(col.r, col.g, col.b);
        int32 pixelFormat = s3eSurfaceGetInt(S3E_SURFACE_PIXEL_TYPE);
        IwAssert(ENGINE, !(pixelFormat & (S3E_SURFACE_PIXEL_SIZE_8 || S3E_SURFACE_PIXEL_SIZE_24)));

        int32 lineLength = IwGetGxState()->m_DisplayWidth;
        if (pixelFormat & S3E_SURFACE_PIXEL_SIZE_16)
        {
            surcol |= surcol << 16;
            lineLength >>= 1;
        }

        int32 pitch = s3eSurfaceGetInt(S3E_SURFACE_PITCH);
        uint8* pPtr = (uint8*)s3eSurfacePtr() + (((IwGetGxState()->m_DisplayYCentre  - (IwGetGxState()->m_DisplayHeight << 2)) >> 3) + t) * pitch;
        pPtr += (IwGetGxState()->m_DisplayXCentre  - (IwGetGxState()->m_DisplayWidth<<2)) >> 3;
        for (int32 y = t; y < b; y++)
        {
            uint32* pPix = (uint32*)pPtr;
            for (int32 l = lineLength; l > 0 ; l--)
            {
                *pPix++ = surcol;
            }
            pPtr += pitch;
        }
    }
    else
    {
        pMaterial->SetColAmbient(col);
        IwGxSetMaterial(pMaterial);
        CIwSVec2 tl(0, (int16)t);
        CIwSVec2 br((int16)IwGetGxState()->m_DisplayWidth, (int16)(b - t));
        IwGxDrawRectScreenSpace(&tl, &br);
    }
}
//--------------------------------------------------------------------------------
static void FillScreenSlant(int32 b, int32 t, CIwColour col, CIwMaterial* pMaterial,
                int32 sina, int32 cosa, int32 displayWidth, int32 displayHeight, const CIwVec2& centre)
{
    if (b > t)
        return; //nothing to do

    CIwSVec2 horz((int16)displayWidth, 0);
    RotatePoint(horz, sina, cosa, CIwVec2(0,0));

    CIwSVec2 up(0, IW_GEOM_ONE);
    RotatePoint(up, sina, cosa, CIwVec2(0,0));

    pMaterial->SetColAmbient(col);
    IwGxSetMaterial(pMaterial);

    CIwSVec2* pVecList = IW_GX_ALLOC(CIwSVec2, 4);
    CIwSVec2* pVec = pVecList;

    *pVec++ = centre + up * (-(displayHeight >> 1) + b) - horz;
    *pVec = pVec[-1] + up * (t+3 - (b-3));
    pVec++;
    *pVec = pVec[-1] + horz * IW_FIXED(2);
    pVec++;
    *pVec = pVec[-1] - up * (t+3 - (b-3));

    IwGxSetVertStreamScreenSpace(pVecList, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}
//--------------------------------------------------------------------------------
const int16 g_UV[4] = { 0, 0, 1 << 12, 1 << 12 };
const int16 g_MirroredUV[4] = { 1 << 12, 0, 0, 1 << 12 };

void CIwHorizon::RenderFastPath(int32 offset_x, int32 offset_y)
{
    IW_CALLSTACK("CIwHorizon::RenderFastPath")

    // Turn off lighting and colour stream
    IwGxLightingOff();
    IwGxSetColStream(0);

    const int32 displayWidth = IwGetGxState()->m_DisplayWidth;
    const int32 displayHeight = IwGetGxState()->m_DisplayHeight;

    //collect constants for the routine
    offset_y = -offset_y + (m_TileHeight * m_NumRows - m_YOffset) - displayHeight/2;


    const int32 wholeWidth = m_NumPerRow * m_TileWidth;

    int32 ystart = 0;
    int32 yend = m_NumRows;
    if (-offset_y < 0)
    {
        ystart += offset_y / m_TileHeight;
    }
    int32 lowestRow = -offset_y + m_TileHeight * m_NumRows;
    if (lowestRow > displayHeight)
    {
        yend -= (lowestRow-displayHeight) / m_TileHeight;
    }


    //fill above
    if (m_FillAbove != 0 && -offset_y > 0)
    {
        FillScreen(0, -offset_y, m_FillAbove, m_AboveMaterial);
    }

    if (m_FillBelow != 0 && (-offset_y + m_TileHeight * m_NumRows)  )
    {
        FillScreen(-offset_y + m_TileHeight * m_NumRows, displayHeight, m_FillBelow, m_BelowMaterial);
    }

    CTile* pTile = m_Tiles.data() + ystart*m_NumPerRow;

    for (int32 y = ystart; y < yend; y++)
    {
        for (int32 x = 0; x < m_NumPerRow; x++)
        {
            CTile& tile = *pTile++;
            int32 vertex_x = -offset_x + x * m_TileWidth;

            if (vertex_x + m_TileWidth < 0 || vertex_x > displayWidth)
            {
                vertex_x += wholeWidth;
                if (vertex_x + m_TileWidth < 0 || vertex_x > displayWidth)
                    continue;
            }

            CIwSVec2* pVec = IW_GX_ALLOC(CIwSVec2, 1);
            pVec->x = (int16)vertex_x;
            pVec->y = (int16)(-offset_y + y * m_TileHeight);

            IwGxSetMaterial(tile.m_Material);

            if (tile.m_Mirror)
                IwGxSetUVStream((CIwSVec2*)g_MirroredUV, 0);
            else
                IwGxSetUVStream((CIwSVec2*)g_UV, 0);

            IwGxSetVertStreamScreenSpace(pVec, 1);
            IwGxDrawPrims(IW_GX_SPRITE_LIST, NULL, 1);
        }
    }
}
//--------------------------------------------------------------------------------
const int16 g_FullUV[8] = { 0 << 12, 1 << 12,
                            1 << 12, 1 << 12,
                            1 << 12, 0 << 12,
                            0 << 12, 0 << 12};

const int16 g_FullMirroredUV[8] = { 1 << 12, 1 << 12,
                                    0 << 12, 1 << 12,
                                    0 << 12, 0 << 12,
                                    1 << 12, 0 << 12};

inline void CalcExtra(int32& out, CIwSVec2 dir, iwfixed sina, iwfixed cosa, int32 displayWidth, int32 displayHeight, bool horizontal)
{
    RotatePoint(dir, sina, cosa, CIwVec2(0,0));

    //always compare to top right corner...
    dir.x = abs(dir.x);
    dir.y = abs(dir.y);

    iwfixed length = CIwSVec2((int16)displayWidth>>1, (int16)displayHeight>>1).Dot(dir);
    if (horizontal)
        out = length - (displayWidth>>1);
    else
        out = length - (displayHeight>>1);
}

inline void CopyUVs(CIwSVec2* pDst, uint16* pIndList, CIwSVec2* pSrc)
{
    pDst[pIndList[0]] = pSrc[0];
    pDst[pIndList[1]] = pSrc[1];
    pDst[pIndList[2]] = pSrc[2];
    pDst[pIndList[3]] = pSrc[3];
}

void CIwHorizon::RenderSlowPath(iwfixed scale, int32 offset_x, int32 offset_y)
{
    IW_CALLSTACK("CIwHorizon::RenderSlowPath")

    //collect all the constants for the routine
    iwfixed fixedScaledWidth = m_TileWidth * scale;
    int32 scaledWidth = fixedScaledWidth >> IW_GEOM_POINT;
    int32 scaledHeight = IW_FIXED_MUL(m_TileHeight, scale);

    const int32 displayWidth = IwGetGxState()->m_DisplayWidth;
    const int32 displayHeight = IwGetGxState()->m_DisplayHeight;

    offset_y = (-offset_y + IW_FIXED_MUL((m_TileHeight * m_NumRows - m_YOffset), scale)) - displayHeight/2;


    iwfixed sina = IW_GEOM_SIN(m_CameraRoll);
    iwfixed cosa = IW_GEOM_COS(m_CameraRoll);
    CIwVec2 centre (displayWidth>>1, displayHeight>>1);

    int32 extraX = 0;
    int32 extraY = 0;

    if (m_CameraRoll)
    {
        //we need to draw more of the skybox to compensate for the angle
        CalcExtra(extraX, CIwSVec2(IW_FIXED(1.0), 0), sina, cosa, displayWidth, displayHeight, true);
        CalcExtra(extraY, CIwSVec2(0, IW_FIXED(1.0)), sina, cosa, displayWidth, displayHeight, false);
    }

    //which is the first/last row?
    int32 ystart = 0;
    int32 yend = m_NumRows;
    if (-offset_y < extraY)
    {
        ystart += (offset_y - extraY) / scaledHeight;
        if (ystart < 0)
            ystart = 0;
    }
    int32 lowestRow = -offset_y + scaledHeight * m_NumRows;
    if (lowestRow > displayHeight + extraY)
    {
        yend -= (lowestRow-(displayHeight + extraY)) / scaledHeight;
    }

    //Fill above/below if required
    if ((m_FillAbove != 0 && -offset_y > 0) || m_CameraRoll )
    {
        if (!m_CameraRoll)
            FillScreen(0, -offset_y, m_FillAbove, m_AboveMaterial);
        else
            FillScreenSlant(-extraY, -offset_y, m_FillAbove, m_AboveMaterial,
                sina, cosa, displayWidth+extraX*2, displayHeight, centre);
    }

    if (m_FillBelow != 0)
    {
        int32 bottomY = -offset_y + (scaledHeight * m_NumRows);
        if (bottomY < displayHeight || m_CameraRoll)
        {
            if (!m_CameraRoll)
                FillScreen(bottomY, displayHeight, m_FillBelow, m_AboveMaterial);
            else
                FillScreenSlant(bottomY, displayHeight+extraY, m_FillBelow, m_AboveMaterial,
                    sina, cosa, displayWidth+extraX*2, displayHeight, centre);
        }
    }

    if (yend <= ystart)
        return; //nothing to do...

    if (!(IwGxGetFlags() & IW_GX_RASTERISATION_HW_F) )
    {
        RenderSlowPathSW(offset_x, offset_y, ystart, yend,
            displayWidth, displayHeight, extraX, extraY,
            scaledWidth, fixedScaledWidth, scaledHeight, sina, cosa, centre);
    }
    else
    {
        RenderSlowPathHW(offset_x, offset_y, ystart, yend,
            displayWidth, displayHeight, extraX, extraY,
            scaledWidth, fixedScaledWidth, scaledHeight, sina, cosa, centre);
    }

}

inline bool OnScreen(int32 vertex_x, int32& outOffset, int32 scaledWidth, int32 leftCompare, int32 rightCompare, int32 wholeWidth, int32 camera_roll)
{
    if (vertex_x + scaledWidth < leftCompare || vertex_x > rightCompare)
    {
        vertex_x += wholeWidth;
        outOffset = wholeWidth;
        if (vertex_x + scaledWidth < leftCompare || vertex_x > rightCompare)
        {
            if (!camera_roll)
            {
                return false;
            }
            else
            {
            //if camera is rolling we need to check the right hand side overflow also...
                outOffset = -wholeWidth;
                vertex_x -= wholeWidth << 1;
                if (vertex_x + scaledWidth < leftCompare || vertex_x > rightCompare)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

//the SW render saves on screen space transforms - is it much faster? unlikely, but since I've written it and it works
//I'm loath to throw it away
void CIwHorizon::RenderSlowPathSW(int32 offset_x, int32 offset_y, int32 ystart, int32 yend, int32 displayWidth, int32 displayHeight, int32 extraX, int32 extraY, int32 scaledWidth, int32 fixedScaledWidth, int32 scaledHeight, int32 sina, int32 cosa, CIwVec2& centre)
{
    IW_CALLSTACK("CIwHorizon::RenderSlowPathSW")
    //guess how many verts we need (this up to 1 vert per line over)

    const int32 maxTilesPerRow = (displayWidth + 2 * extraX) / (scaledWidth) + 2;
    int32 numVerts = (maxTilesPerRow + 2) * (yend - ystart + 1);
    CIwSVec2* pVecList = IW_GX_ALLOC(CIwSVec2, numVerts);

    CIwSVec2* pVec = pVecList;

    const int32 wholeWidth = IW_FIXED_MUL(m_NumPerRow, fixedScaledWidth) - 1;

    int32 tilesDrawnNeeded = m_NumPerRow / 8 + 1;
    uint8* tilesDrawn = IW_GX_ALLOC(uint8, tilesDrawnNeeded);
    memset(tilesDrawn, 0, tilesDrawnNeeded);
    int16 tilesDrawnCursor = -1;
    uint16 vertPitch = 0;
    bool drewLeft;
    int32 y;

    int32 leftCompare = 0;
    int32 rightCompare = displayWidth;

    leftCompare -= extraX;
    rightCompare += extraX;

    //build the verts
    y = ystart;
    int32 numHorizTiles = 0;
    //generate the first line
    drewLeft = false;
    for (int32 x = 0; x < m_NumPerRow; x++)
    {
        int32 vertex_x = -offset_x + IW_FIXED_MUL(x, fixedScaledWidth);
        int32 move_x = 0;

        tilesDrawnCursor++;

        if (!OnScreen(vertex_x, move_x, scaledWidth, leftCompare, rightCompare, wholeWidth, m_CameraRoll) )
        {
            drewLeft = false;
            continue;
        }

        vertex_x += move_x;

        tilesDrawn[tilesDrawnCursor/8] |= 1 << (tilesDrawnCursor % 8);
        numHorizTiles++;

        if (!drewLeft)
        {
            pVec->x = (int16)vertex_x;
            pVec->y = (int16)(-offset_y + y * scaledHeight);

            pVec++;
        }

        pVec->x = (int16)(vertex_x + scaledWidth);
        pVec->y = (int16)(-offset_y + y * scaledHeight);

        pVec++;

        IwAssert(ENGINE, pVec < pVecList + numVerts);
        drewLeft = true;
    }

    vertPitch = pVec - pVecList;
    int32 totalVertCheck = vertPitch;

    //generate the following line(s)
    for (y = ystart+1; y < yend+1; y++)
    {
        totalVertCheck += vertPitch;
        int ypos = (int16)(-offset_y + y * scaledHeight);
        for (int32 x = 0; x < vertPitch; x++)
        {
            pVec->x = pVec[-vertPitch].x;
            pVec->y = ypos;

            pVec++;
            IwAssert(ENGINE, pVec <= pVecList + numVerts);
        }
    }

    IwAssert(ENGINE, pVec - pVecList == totalVertCheck);
    IwAssert(ENGINE, totalVertCheck <= numVerts);

    //rotate them all
    if (m_CameraRoll)
    {
        for (int32 v = 0; v < pVec-pVecList; v++)
            RotatePoint(pVecList[v], sina, cosa, centre);
    }

    CTile* pTile = m_Tiles.data() + ystart*m_NumPerRow;
    uint16 ind = vertPitch;

    CIwSVec2* pUVs = IW_GX_ALLOC(CIwSVec2, numVerts);
    IwGxSetUVStream(pUVs);

    //set up colours
/*  CIwColour*colStream = (CIwColour*)IW_GX_ALLOC(CIwColour, numVerts);
    memset((uint8*)colStream, 0xFF, numVerts*sizeof(CIwColour));
    IwGxSetColStream( colStream, numVerts) ;
    IwGxSetFlags(IW_GX_LOCK_PREALLOCCOLBUFFER_F);*/
    IwGxLightingOff();
    IwGxSetColStream(0);

    //now render them
    IwGxSetVertStreamScreenSpace(pVecList, numVerts);
    uint16* pIndList = IW_GX_ALLOC(uint16, 4);

    for (y = ystart; y < yend; y++)
    {
        drewLeft = false;
        tilesDrawnCursor = -1;
        int32 numHorizTilesKnt = 0;
        for (int32 x = 0; x < m_NumPerRow; x++)
        {
            CTile& tile = *pTile++;

            tilesDrawnCursor++;

            if (!(tilesDrawn[tilesDrawnCursor/8] & (1 << (tilesDrawnCursor % 8))) )
            {
                if (drewLeft)
                    ind++;
                drewLeft = false;
                continue;
            }
            numHorizTilesKnt++;
            uint16* pInd = pIndList;

            IwAssert(ENGINE, ind + 1 < numVerts);


            *pInd++ = ind;
            *pInd++ = ind + 1;
            *pInd++ = ind + 1 - vertPitch;
            *pInd++ = ind - vertPitch;

            ind++;

            IwGxSetMaterial(tile.m_Material);

            if (tile.m_Mirror)
                CopyUVs(pUVs, pIndList, (CIwSVec2*)g_FullMirroredUV);
            else
                CopyUVs(pUVs, pIndList, (CIwSVec2*)g_FullUV);

            IwGxDrawPrims(IW_GX_QUAD_LIST, pIndList, 4);

            drewLeft = true;
        }
        if (drewLeft)
            ind++;
        IwAssert(ENGINE, numHorizTilesKnt == numHorizTiles);
    }

//  IwGxClearFlags(IW_GX_LOCK_PREALLOCCOLBUFFER_F);
    IwGxLightingOn();
}


void CIwHorizon::RenderSlowPathHW(int32 offset_x, int32 offset_y, int32 ystart, int32 yend,
        int32 displayWidth, int32 displayHeight,
        int32 extraX, int32 extraY,
        int32 scaledWidth, int32 fixedScaledWidth, int32 scaledHeight,
        int32 sina, int32 cosa, CIwVec2& centre)
{
    IW_CALLSTACK("CIwHorizon::RenderSlowPathHW")

    int32 leftCompare = 0;
    int32 rightCompare = displayWidth;

    leftCompare -= extraX;
    rightCompare += extraX;

    const int32 wholeWidth = IW_FIXED_MUL(m_NumPerRow, fixedScaledWidth) - 1;

    int32 top_y = (int16)(-offset_y + ystart * scaledHeight);
    CTile* pTile = m_Tiles.data() + ystart*m_NumPerRow;

//  IwGxSetFlags(IW_GX_LOCK_PREALLOCCOLBUFFER_F);
    IwGxSetColStream(0);
    IwGxLightingOff();

    for (int32 y = ystart; y < yend; y++)
    {
        int32 left_x = -offset_x;

        int32 bottom_y = (int16)(-offset_y + (y+1) * scaledHeight);;

        for (int32 x = 0; x < m_NumPerRow; x++)
        {
            CTile& tile = *pTile++;

            int32 move_x = 0;
            int32 right_x = -offset_x + IW_FIXED_MUL((x+1), fixedScaledWidth);

            if (!OnScreen(left_x, move_x, scaledWidth, leftCompare, rightCompare, wholeWidth, m_CameraRoll) )
            {
                left_x = right_x;
                continue;
            }

            left_x += move_x;
            right_x += move_x;

            CIwSVec2* pVecList = IW_GX_ALLOC(CIwSVec2, 4);
            CIwSVec2* pVec = pVecList;

            pVec->x = (int16)left_x;
            (pVec++)->y = (int16)bottom_y;
            pVec->x = (int16)right_x;
            (pVec++)->y = (int16)bottom_y;
            pVec->x = (int16)right_x;
            (pVec++)->y = (int16)top_y;
            pVec->x = (int16)left_x;
            (pVec++)->y = (int16)top_y;

            if (m_CameraRoll)
            {
                pVec = pVecList;
                RotatePoint(*pVec++, sina, cosa, centre);
                RotatePoint(*pVec++, sina, cosa, centre);
                RotatePoint(*pVec++, sina, cosa, centre);
                RotatePoint(*pVec++, sina, cosa, centre);
            }

            if (tile.m_Mirror)
                IwGxSetUVStream((CIwSVec2*)g_FullMirroredUV);
            else
                IwGxSetUVStream((CIwSVec2*)g_FullUV);

            IwGxSetMaterial(tile.m_Material);
            IwGxSetVertStreamScreenSpace(pVecList, 4);
            IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

            left_x = right_x - move_x;
        }

        top_y = bottom_y;
    }
//  IwGxClearFlags(IW_GX_LOCK_PREALLOCCOLBUFFER_F);
}
