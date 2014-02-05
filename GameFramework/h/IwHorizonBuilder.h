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
/**
 * @file IwHorizonBuilder.h
 * @brief Constructs several CIwBg2DBitmap's given a large bitmap, cutting the large bitmap into tiles
 */
#ifndef IW_HORIZON_BUILDER_H
#define IW_HORIZON_BUILDER_H

// Includes
#include "IwGeomFVec3.h"

// Forward declarations
class CIwHorizon;
class CIwTexture;
class CIwMaterial;

const int IW_MAX_TILES=128;

#ifdef IW_DEBUG

//--------------------------------------------------------------------------------
// CIwHorizonBuilder
//--------------------------------------------------------------------------------
class CIwHorizonBuilder
{
public:
    //methods

    /**
     * Create a horizon object containing several IwHorizonBuilder's. This will split
     * the bitmapFile up into tileSizeW,tileSizeH sized tiles creating new CIwTextures for each.
     * duplicate and mirrored tiles will be detected.
     */
    CIwHorizon* CreateHorizon (CIwHorizon*,
                                    const char* bitmapFile,
                                    const char* horizonName,
                                    int yoffset = 0,
                                    int tileSizeW=48,
                                    int tileSizeH=48,
                                    bool bFillAbove = false,
                                    bool bFillBelow = false,
                                    bool bUseFixedFOV = true);

    /** Perform the tiling of a large bitmap */
    void SplitTextureIntoTiles(CIwTexture* pBigTex,
                                    CIwMaterial** pTiles,
                                    bool* AreMirrored,
                                    int tileSizeW,
                                    int tileSizeH,
                                    int* numTilesW,
                                    int* numTilesH);

    /** Returns wether two textures are the identical */
    bool AreSame(CIwTexture* pTex1, CIwTexture* pTex2);

    /** Returns wether two textures are identical but with a horizontal mirror */
    bool AreMirrored(CIwTexture* pTex1, CIwTexture* pTex2);

    CIwTexture* Reduce(CIwTexture* pTex);
    void FinishReduce(CIwTexture* pTex);
};

#endif // IW_DEBUG

#endif /* !IW_HORIZON_BUILDER_H */
