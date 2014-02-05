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
 * @file IwHorizon.h
 * @brief Store of background-rendering objects
 */
#ifndef IW_HORIZON_H
#define IW_HORIZON_H

// Includes
#include "IwModel.h"
#include "IwResource.h"

// Forward Declarations
class CIwModel;
class CIwHorizon;
class CIwMaterial;

/** Background types */
enum
{
    IW_BACKGROUND_TYPE_CYLINDER,
    IW_BACKGROUND_TYPE_2DBITMAP,
};

IW_SINGLETON_EXTERN(Horizon);

class CIwHorizon : public CIwResource
{
public:
    //methods
    IW_MANAGED_DECLARE_NOCOPY(CIwHorizon);

    CIwHorizon();
    ~CIwHorizon();

    //register necessary class factories for loading a horizon
    static void     RegisterClassFactories();

    /** Render a CIwHorizon */
    virtual void    Render();

    /** Read/write binary file */
    virtual void    Serialise();

    /** Resolve references to other objects once serialisation is complete */
    virtual void    Resolve();

#ifdef IW_DEBUG
    void ParseOpen(CIwTextParserITX* pParser);
    bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    void ParseClose(CIwTextParserITX* pParser);
#endif

    void            SetYScale(iwfixed scale);

    /** Camera yaw - useful for background renderers to refer to */
    iwangle              m_CameraYaw;
    iwangle              m_Spin;
    iwangle              m_SpinSpeed;

    /** Camera pitch - useful for background renderers to refer to */
    iwangle              m_CameraPitch;

    /** Camera roll - useful for background renderers to refer to */
    iwangle              m_CameraRoll;

    /** Set this to your viewspace y scale */
    iwfixed              m_InvYScale;

private:
    enum
    {
        AUTO_SCALE_F  = 1,  //! Allow the horizon to automatically scale itself so that it always behaves correctly
        USE_FIXED_FOV_F = 2,    //! Use the stored FOV value rather than using the current render FOV; Prevents horizon responding to changes in FOV
    };

    //can use the fast path render when horizon is horizontal (no roll) and 1:1 scale
    void RenderFastPath(int32 offset_x, int32 offset_y);
    void RenderSlowPath(iwfixed scale, int32 offset_x, int32 offset_y);
    void RenderSlowPathSW(int32 offset_x, int32 offset_y, int32 ystart, int32 yend,
        int32 displayWidth, int32 displayHeight,
        int32 extraX, int32 extraY,
        int32 scaledWidth, int32 fixedScaledWidth, int32 scaledHeight,
        int32 sina, int32 cosa, CIwVec2& centre);
    void RenderSlowPathHW(int32 offset_x, int32 offset_y, int32 ystart, int32 yend,
        int32 displayWidth, int32 displayHeight,
        int32 extraX, int32 extraY,
        int32 scaledWidth, int32 fixedScaledWidth, int32 scaledHeight,
        int32 sina, int32 cosa, CIwVec2& centre);

    int32               m_TileWidth; // how wide each tile is
    int32               m_TileHeight; // how high each tile is
    int32               m_NumPerRow; //how many tiles across
    int32               m_NumRows; //how many tiles up/down
    int32               m_XOffset; //where is the horizon located on the screen?
    int32               m_YOffset; //where is the horizon located on the screen?

    /** Array of renderable objects on the horizon */
    struct CTile
    {
        void Serialise();

        CIwMaterial* m_Material;
        bool m_Mirror;
    };

    CIwArray<CTile>     m_Tiles;

    uint32              m_Flags;
    int32               m_NumWraps; //how many times the skybox goes round
    uint32              m_FakePerspMul; // Fake perspmul if in use

    CIwColour           m_FillBelow;
    CIwColour           m_FillAbove;

    CIwMaterial*        m_AboveMaterial;
    CIwMaterial*        m_BelowMaterial;

    friend class CIwHorizonBuilder;
};

#endif /* !IW_HORIZON_H */
