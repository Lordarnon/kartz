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
#ifndef IW_RENDERMANAGER_H
#define IW_RENDERMANAGER_H

// Includes
#include "IwGeomVec.h"
#include "IwManaged.h"
#include "IwManagedList.h"
#include "IwMenu.h"
#include "IwSingleton.h"

/**
 * @defgroup IwRenderManager IwRenderManager
 */


/**
 * @addtogroup IwRenderManager
 * @brief Render manager singleton for controlling the render process.
 * @{
 */

// Forward declarations
class CIwCamera;
class CIwMenu;
class CIwRenderSlot;
class CIwWorld;

/** Debug text stuff */
typedef CIwString<0x40>     CIwDebugTextString;
struct CIwDebugText
{
    CIwDebugTextString  m_Text;     //!< text to display
    CIwSVec3            m_Pos;      //!< world pos
    uint16              m_ColID;    //!< colour ID
};
#define IW_RENDERMANAGER_DEBUG_TEXT_MAX 0x100       //!< max number of debug text slots
#define IW_RENDERMANAGER_DEBUG_TEXT_2D  0x7fff      //!< set m_Pos.z to this to force 2D text

//-----------------------------------------------------------------------------
// CIwRenderManager
//-----------------------------------------------------------------------------
/** CIwRenderManager class. */
class CIwRenderManager
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    CIwRenderManager();
    virtual ~CIwRenderManager();

    /** Destroy slots */
    inline void DestroySlots() {m_Slots.Delete();}

    /** Render (our own slot list) */
    virtual void    Render();

    /** Render debug text */
#ifdef IW_DEBUG
    virtual void    DebugRenderText();
#endif

    /** Set camera */
    inline  void    SetCamera(CIwCamera* pCam)  {m_Camera = pCam;}

    /** Get camera */
    inline  CIwCamera* GetCamera() const {return m_Camera;}

    /** Set world */
    inline  void    SetWorld(CIwWorld* pWorld)  {m_World = pWorld;}

    /** Get world */
    inline  CIwWorld* GetWorld() const {return m_World;}

    /**
     * Render (a specified slot list)
     * @param list The list to pass in
     */
    virtual void    RenderSlotList(const CIwManagedList& list);

    /** Add render slot */
    void    AddSlot(CIwRenderSlot* pSlot);

#ifdef IW_DEBUG
    /** Debug method to add items to menu */
    virtual void    DebugAddMenuItems(CIwMenu* pMenu);

    /**
     * Add debug text
     * @param text Text string to add
     * @param pos Position of text
     * @param colID Colour ID for text
     */
    inline  void    DebugAddText(const char* text, const CIwSVec3& pos, uint16 colID)
    {
        IwAssertMsgN(296,ENGINE, m_DebugTextNext < m_DebugText + IW_RENDERMANAGER_DEBUG_TEXT_MAX, ("Debug text overflow"));
        m_DebugTextNext->m_Text     = text;
        m_DebugTextNext->m_Pos      = pos;
        m_DebugTextNext->m_ColID    = colID;
        m_DebugTextNext++;
    }
#endif

    // Members
    CIwManagedList          m_Slots;                //!< all render slots

    // Debug text
#ifdef IW_DEBUG
    uint32                  m_DebugFlags;       //!< debug: flags
    CIwDebugText            m_DebugText[IW_RENDERMANAGER_DEBUG_TEXT_MAX];   //!< array of debug text slots
    CIwDebugText*           m_DebugTextNext;    //!< ptr to next available slot
#endif

    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
protected:
    CIwWorld*               m_World;            //!< convenience: world to render
    CIwCamera*              m_Camera;           //!< convenience: camera to render from
    int32                   m_TimeFraction;     //!< fractional position through current frame

    // Static members
#ifdef IW_DEBUG
    static  const char*     g_DebugFlagNames[]; //!< debug flag names
#endif
};
IW_SINGLETON_EXTERN(RenderManager);

//-----------------------------------------------------------------------------
// CIwMenuItemRenderManager
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
/** CIwMenuItemRenderManager class. */
class CIwMenuItemRenderManager : public CIwMenuItem
{
public:
    CIwMenuItemRenderManager(const char* pString) : CIwMenuItem("", pString) {};

    void    Select()
    {
        // Create menu
        CIwMenu* pMenu = new CIwMenu;
        IwGetMenuManager()->AddMenu(pMenu);

        // Add title
        pMenu->AddItem(new CIwMenuItemTitle("RenderManager"));

        // Add items
        IwGetRenderManager()->DebugAddMenuItems(pMenu);
        if (pMenu->m_ItemArray.size() <= 1)
            pMenu->AddItem(new CIwMenuItemString("No items", ""));
        pMenu->SetItemID(1);
    };
};
#endif

#endif

// @}
