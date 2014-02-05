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
#ifndef IW_WORLD_H
#define IW_WORLD_H

// Includes
#include "IwManagedList.h"
#include "IwResource.h"

/**
 * @defgroup IwWorld IwWorld
 *
 * @addtogroup IwWorld
 * @brief World class. Essentially a container for entities.
 * @{
 */
class CIwWorld : public CIwResource
{
public:
    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    IW_MANAGED_DECLARE_NOCOPY(CIwWorld);

    CIwWorld();
    ~CIwWorld();
    virtual void Serialise();
    virtual void Resolve();
    virtual void ParseOpen(CIwTextParserITX* pParser);
    virtual bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
    virtual void ParseClose(CIwTextParserITX* pParser);

    /** Add an object */
    inline  void AddObject(CIwManaged* pObj)
    {
        IwAssertMsgN(300,ENGINE, pObj, ("Null ptr"));
        m_Objects.Add(pObj);
    }

    /** Remove an object */
    inline  void RemoveObject(CIwManaged* pObj)
    {
        IwAssertMsgN(301,ENGINE, pObj, ("Null ptr"));
        m_Objects.RemoveFast(pObj);
    }

    /** Get a named object */
    inline CIwManaged* GetObjNamed(const char* name) const {return m_Objects.GetObjNamed(name);}

    /** Get a hashed object */
    inline CIwManaged* GetObjHashed(uint32 hash) const {return m_Objects.GetObjHashed(hash);}

#ifdef IW_DEBUG
    /** Debug rendering */
    void    DebugRender();

    /** Add menu items to a CIwMenu for debugging purposes */
    void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

    // Static methods
    static void SetContext(CIwWorld* pWorld) {g_Context = pWorld;}
    static CIwWorld* GetContext() {return g_Context;}
    static CIwWorld* GetContextSafe() {IwAssertMsgN(302,ENGINE, g_Context, ("Context not set")); return g_Context;}

    // Members
    CIwManagedList  m_Objects;      //!< all objects owned by the world

    // Static members
    static CIwWorld* g_Context;
};

#endif

// @}
