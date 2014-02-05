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
 * Constructs several CIwBg2DBitmap's given a large bitmap, cutting the large bitmap into tiles
 */

#include "IwHorizonBuilder.h"
#include "IwTexture.h"
#include "IwHorizon.h"
#include "IwGx.h"
#include "IwMaterial.h"
#include "IwResManager.h"

#ifdef IW_DEBUG

static void Peek(int x, int y, CIwTexture * t, CIwColour & col)
{
    IW_CALLSTACK("Peek")

    int p = x + y*t->GetPitch();
    int i = t->GetTexels()[p];
    if (!t->GetPalette())
        col.Set(0, 0, 0, 0xff);
    else
    {
        col.a = 0xff;
        col.r = ((uint8*)(t->GetPalette()))[i*3+2];
        col.g = ((uint8*)(t->GetPalette()))[i*3+1];
        col.b = ((uint8*)(t->GetPalette()))[i*3+0];
    }
}

CIwHorizon* CIwHorizonBuilder::CreateHorizon (CIwHorizon* pNewHorz, const char* bitmapFile, const char* horizonName,
                                                 int yoffset, int tileSizeW, int tileSizeH, bool bFillAbove,
                                                 bool bFillBelow, bool bUseFixedFOV)
{
    IW_CALLSTACK("CIwHorizonBuilder::CreateHorizon")

//  CIwTexture* pBigTex = IwGxTextureCreate("undefined");
    CIwTexture* pBigTex = new CIwTexture;
    pBigTex->SetName("undefined");
    pBigTex->LoadFromFile(bitmapFile);

//  CIwMaterial* pTiles[IW_MAX_TILES][IW_MAX_TILES];            //array of 128x128 tile poiners
//  bool        mirrored[IW_MAX_TILES][IW_MAX_TILES];       //array of 128x128 flags to indicate mirrored status

    CIwMaterial** pTiles = new CIwMaterial*[IW_MAX_TILES*IW_MAX_TILES];
    bool* mirrored = new bool[IW_MAX_TILES*IW_MAX_TILES];


    int numTilesW;                                          //number of tiles across
    int numTilesH;                                          //number of tiles down

    if  (
        (bFillAbove) &&
        (*(uint32*)&pNewHorz->m_FillAbove == 0)
        )
        Peek(0, 0, pBigTex, pNewHorz->m_FillAbove);

    if  (
        (bFillBelow) &&
        (*(uint32*)&pNewHorz->m_FillBelow == 0)
        )
        Peek(0, pBigTex->GetHeight()-1, pBigTex, pNewHorz->m_FillBelow);

    SplitTextureIntoTiles(pBigTex, pTiles, mirrored, tileSizeW, tileSizeH, &numTilesW, &numTilesH);

    IwGxAddPaletteSharing(IW_GX_SHARE_8BIT_PALS | IW_GX_SHARE_4BIT_PALS);

    FinishReduce(pBigTex);

    pNewHorz->m_NumRows = numTilesH;
    pNewHorz->m_NumPerRow = numTilesW;
    pNewHorz->m_TileWidth = tileSizeW;
    pNewHorz->m_TileHeight = tileSizeH;
    pNewHorz->m_YOffset = yoffset;

    pNewHorz->m_Tiles.resize(numTilesH*numTilesW);

    int32 numUniqueTextures = 0;

    for (int y=0;y<numTilesH;y++)
    {
        for (int x=0;x<numTilesW;x++)
        {
            CIwHorizon::CTile& tile = pNewHorz->m_Tiles[x + y * numTilesW];
            tile.m_Material = pTiles[y*IW_MAX_TILES + x];
            tile.m_Mirror = mirrored[y*IW_MAX_TILES + x];

            if (IwGetResManager()->GetResHashed(pTiles[y*IW_MAX_TILES + x]->m_Hash, IW_GX_RESTYPE_MATERIAL, IW_RES_PERMIT_NULL_F) != NULL)
                continue;

            IwGetResManager()->AddRes(IW_GX_RESTYPE_MATERIAL, pTiles[y*IW_MAX_TILES + x]);

            // Set clamp mode to avoid filtering artefacts in HW
            pTiles[y*IW_MAX_TILES + x]->SetClamping(true);

            IwGetResManager()->AddRes(IW_GX_RESTYPE_TEXTURE, pTiles[y*IW_MAX_TILES + x]->GetTexture());

            numUniqueTextures++;
        }
    }

    IwTrace(ENGINE, ("Created horizon with %d unique textures and %d tiles", numUniqueTextures, numTilesH*numTilesW));

    delete pBigTex;

//  IwGxRemovePaletteSharing(IW_GX_SHARE_8BIT_PALS | IW_GX_SHARE_4BIT_PALS);

    delete pTiles;
    delete mirrored;

    return pNewHorz;
}
//--------------------------------------------------------------------------------
void CIwHorizonBuilder::SplitTextureIntoTiles(CIwTexture* pBigTex,
                                                 CIwMaterial** pTiles,
                                                 bool* mirrored,
                                                 int tileSizeW,
                                                 int tileSizeH,
                                                 int* numTilesW,
                                                 int* numTilesH)
{
    IW_CALLSTACK("CIwHorizonBuilder::SplitTextureIntoTiles")

    IwAssertMsgN(326,ENGINE, pBigTex->GetWidth() % tileSizeW == 0, ("Bitmap width %u is not a multiple of tile size %d!", pBigTex->GetWidth(), tileSizeW));
    IwAssertMsgN(327,ENGINE, pBigTex->GetHeight() % tileSizeH == 0, ("Bitmap height %u is not a multiple of tile size %d!", pBigTex->GetHeight(), tileSizeH));

    *numTilesW = pBigTex->GetWidth()/tileSizeW;
    *numTilesH = pBigTex->GetHeight()/tileSizeH;

    int i;
//  CIwTexture* pTex[IW_MAX_TILES][IW_MAX_TILES] = {0};
    CIwTexture** pTex = new CIwTexture*[IW_MAX_TILES*IW_MAX_TILES];
    memset(pTex, 0, IW_MAX_TILES*IW_MAX_TILES*sizeof(CIwTexture*));

    for (i=0;i<*numTilesH;i++) {
        for (int j=0;j<*numTilesW;j++) {
            int x = j*tileSizeW;
            int y = i*tileSizeH;
            char name[256];
            sprintf (name, "HZN_TileTex %d,%d",i,j);
            pTex[i*IW_MAX_TILES + j] = pBigTex->ExtractSubTexture(x, y, tileSizeW, tileSizeH, name);

            pTiles[i*IW_MAX_TILES + j] = new CIwMaterial();
            pTiles[i*IW_MAX_TILES + j]->Init();
            pTiles[i*IW_MAX_TILES + j]->SetTexture(pTex[i*IW_MAX_TILES + j]);
            pTiles[i*IW_MAX_TILES + j]->SetZDepthOfs((int16)IwGetGxState()->m_FarZ);
            pTiles[i*IW_MAX_TILES + j]->SetClamping(true);

            // Set material properties
            sprintf (name, "HZN_TileMat %d,%d",i,j);
            pTiles[i*IW_MAX_TILES + j]->SetName(name);
            pTiles[i*IW_MAX_TILES + j]->SetColAmbient(0xffffffff); //(0xff808080)
            pTiles[i*IW_MAX_TILES + j]->SetColDiffuse(0);
            pTiles[i*IW_MAX_TILES + j]->SetShadeMode(CIwMaterial::SHADE_FLAT);
            pTiles[i*IW_MAX_TILES + j]->SetModulateMode(CIwMaterial::MODULATE_NONE);

            mirrored[i*IW_MAX_TILES + j] = false;
        }
    }

    for (i=0;i<*numTilesH;i++) {
        for (int j=0;j<*numTilesW;j++) {

            for (int k=i;k<*numTilesH;k++) {
                for (int l=((k == i)?j+1:0);l<*numTilesW;l++) {
                    if (pTiles[i*IW_MAX_TILES + j] != pTiles[k*IW_MAX_TILES + l]) {
                        if (AreSame(pTiles[i*IW_MAX_TILES + j]->GetTexture(), pTiles[k*IW_MAX_TILES + l]->GetTexture())){
                            delete pTiles[k*IW_MAX_TILES + l];
                            delete pTex[k*IW_MAX_TILES + l];
                            pTiles[k*IW_MAX_TILES + l] = pTiles[i*IW_MAX_TILES + j];
                            continue;
                        }
                        if (AreMirrored(pTiles[i*IW_MAX_TILES + j]->GetTexture(), pTiles[k*IW_MAX_TILES + l]->GetTexture())) {
                            delete pTiles[k*IW_MAX_TILES + l];
                            delete pTex[k*IW_MAX_TILES + l];
                            pTiles[k*IW_MAX_TILES + l] = pTiles[i*IW_MAX_TILES + j];
                            mirrored[k*IW_MAX_TILES + l] = true;
                            continue;
                        }
                    }
                }
            }
        }
    }

    for (i=0;i<*numTilesH;i++) {
        for (int j=0;j<*numTilesW;j++) {

            CIwTexture* reduce = Reduce( pTiles[i*IW_MAX_TILES + j]->GetTexture() );
            if (reduce)
            {
                delete pTiles[i*IW_MAX_TILES + j]->GetTexture();
                pTiles[i*IW_MAX_TILES + j]->SetTexture(reduce);
            }
            else if (pTiles[i*IW_MAX_TILES + j]->GetTexture()->GetFormat() != CIwImage::PALETTE4_ABGR_1555 )
            {
                IwTrace(ENGINE, ("Warning: Horizon could not reduce tile %d, %d to 16 colours", j, i));
            }

        }
    }

    delete pTex;
}

struct ClutMaking
{
    CIwArray<CIwTexture*> users;
    int col[16];
    int num;

    ClutMaking() : num(0)
    {}

    bool Include(const int* pal, int size, bool extend)
    {
        int need[16], needCount=0;

        for (int i=0; i<size; i++)
        {
            bool found = false;

            for (int j=0; j<num; j++)
                if (col[j] == pal[i])
                {
                    found = true;
                    break;
                }

            if (!found)
            {
                if (needCount >= 16)
                    return false;
                need[needCount++] = pal[i];
            }
        }

        if (!needCount)
            return true;
        if (!extend && needCount)
            return false;
        if (needCount + num > 16)
            return false;

        memcpy(&col[num], need, sizeof(need[0]) * needCount);
        num += needCount;

        return true;
    }

    void Final(CIwTexture* pTex)
    {
        if (pTex->GetPalette())
        {
            uint16 pal[16];
            memset(pal, 0xff, 16*2);
            for (int i=0; i<num; i++)
            {
                int c = col[i];
                pal[i] =
                    (int(((uint8*)(pTex->GetPalette()))[3*c+0] >> 3) << 10) |
                    (int(((uint8*)(pTex->GetPalette()))[3*c+1] >> 3) << 5) |
                    (int(((uint8*)(pTex->GetPalette()))[3*c+2] >> 3) << 0) |
                    0x8000;
            }

            for (uint32 j=0; j<users.size(); j++)
                users[j]->ChangePalette((uint16*)pal,CIwImage::PALETTE4_ABGR_1555);
                //memcpy((uint8*)users[j]->GetPalette(), pal, 16*2);
        }
    }
};
CIwArray<ClutMaking> cm;
ClutMaking* GetCM(int* pal, int size)
{
    IW_CALLSTACK("GetCM")

    uint32 i;
    for (i=0; i<cm.size(); i++)
    {
        if (cm[i].Include(pal, size, false))
            return &cm[i];
    }
    for (i=0; i<cm.size(); i++)
    {
        if (cm[i].Include(pal, size, true))
            return &cm[i];
    }

    cm.push_back();
    cm.back().Include(pal, size, true);
    return &cm.back();
}
void CIwHorizonBuilder::FinishReduce(CIwTexture* pTex)
{
    IW_CALLSTACK("CIwHorizonBuilder::FinishReduce")

    for (uint32 i=0; i<cm.size(); i++)
        cm[i].Final(pTex);
    cm.clear_optimised();
}

CIwTexture* CIwHorizonBuilder::Reduce(CIwTexture* pTex)
{
    IW_CALLSTACK("CIwHorizonBuilder::Reduce")

    if (pTex->GetFormat() != CIwImage::PALETTE8_ABGR_1555)
        return NULL;

    uint8 * tex = (uint8*)pTex->GetTexels();
    int pitch = pTex->GetPitch();
    int w = pTex->GetWidth();
    int h = pTex->GetHeight();

    int used[256] = {0};

    for (int x=0; x<w; x++)
        for (int y=0; y<h; y++)
            used[tex[x+y*pitch]]++;

    int i;
    int palCount = 0;
    int palIdx[16] = {0};
    for (i=0; i<256; i++)
    {
        if (used[i])
        {
            if (palCount>=16)
            {
                //break;
                return NULL;
            }
            palIdx[palCount++] = i;
        }
    }

    ClutMaking* cm = GetCM(palIdx, palCount);

    // Note: we create the texture with a bogus palette for the moment - it gets replaced later!
    memset(used, 0xff, sizeof(used));
    uint16 pal[16];
    memset(pal, 0xff, 16*2);

    for (i=0; i<cm->num; i++)
    {
        int c = cm->col[i];
        used[c] = i;
    }
    int p2 = (w+1)/2;
    uint8* img = new uint8 [h * p2];
    memset(img, 0, h*p2);

    {
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
            {
                img[x/2 + y*p2] |= used[tex[x+y*pitch]] << ((x&1)*4);
            }
    }

    CIwTexture* r = new CIwTexture;

    // Give new texture a name!
    char tmp[100];
    sprintf(tmp, "%s_reduced", pTex->DebugGetName());
    r->SetName(tmp);

//  r->CopyFromBuffer(w, h, IW_IMAGE_FORMAT_PALETTISED_4BIT_1555, p2, img, pal);
    r->CopyFromBuffer(w, h, CIwImage::PALETTE4_ABGR_1555, p2, img, pal);
    IwAssert(ENGINE, r->GetFormat() == CIwImage::PALETTE4_ABGR_1555);

    cm->users.append(r);

    delete [] img;

    return r;
}
//--------------------------------------------------------------------------------
bool CIwHorizonBuilder::AreSame(CIwTexture* pTex1, CIwTexture* pTex2)
{
    IW_CALLSTACK("CIwHorizonBuilder::AreSame")

    IwAssert(ENGINE, pTex1->GetWidth() == pTex2->GetWidth() && pTex1->GetHeight() == pTex2->GetHeight());
    bool r =    (
            (!memcmp(pTex1->GetTexels(), pTex2->GetTexels(), pTex1->GetTexelsMemSize())) //&&
            //(!memcmp(pTex1->GetPalette(), pTex2->GetPalette(), pTex1->GetPaletteDataSize()))
            );

    return r;
}
//--------------------------------------------------------------------------------
bool CIwHorizonBuilder::AreMirrored(CIwTexture* pTex1, CIwTexture* pTex2)
{
    IW_CALLSTACK("CIwHorizonBuilder::AreMirrored")

    // Doesn't work for 4-bit
    if (pTex1->GetFormat() == CIwImage::PALETTE4_ABGR_1555)
        return false;

    IwAssert(ENGINE, pTex1->GetWidth() == pTex2->GetWidth() && pTex1->GetHeight() == pTex2->GetHeight());
    IwAssert(ENGINE, pTex1->GetFormat() == pTex2->GetFormat());

    for (uint32 y=0;y<pTex1->GetHeight();y++) {
        uint8* pLine1 = (uint8*)pTex1->GetTexels() + pTex1->GetPitch() * y;
        uint8* pLine2 = (uint8*)pTex2->GetTexels() + pTex2->GetPitch() * y;
        for (uint32 x=0;x<pTex1->GetWidth();x++) {
            if (pLine1[x] != pLine2[pTex1->GetWidth()-x-1])
                return false;
        }
    }
    return true;
}

#endif // IW_DEBUG
