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
 * @addtogroup Entities
 * @brief Base class for entity with behaviours.
 * @{
 */

#ifndef IW_ENT_H
#define IW_ENT_H

// Includes
#include "IwArray.h"
#include "IwFrame.h"
#include "IwGeomSphere.h"
#include "IwString.h"
#include "IwManaged.h"

// Forward declarations
class CIwBehaviour;
class CIwBeMotion;
class CIwFrame;
class CIwWorld;

//-----------------------------------------------------------------------------
// CIwEnt
//-----------------------------------------------------------------------------
/** Base class for entity with behaviours. */
class CIwEnt : public CIwManaged
{
public:
    /** Default stuff for CIwManaged base class. */
    IW_MANAGED_DECLARE_NOCOPY(CIwEnt);

    //-------------------------------------------------------------------------
    // Public API
    //-------------------------------------------------------------------------
    /** Flags */
    enum /*TFlag*/
    {
        // Bits 0..15 are flags that anyone can modify
        UPDATE_F                    = (1 << 0),     //!< update this ent
        SERIALISE_FRAME_F           = (1 << 1),     //!< serialise any owned frame
        SERIALISE_BEHAVIOUR_F       = (1 << 2),     //!< serialise any owned behaviour

        DEBUG_RENDER_NAME_F         = (1 << 8),     //!< display name text, at ent origin
        DEBUG_RENDER_POS_F          = (1 << 9),     //!< display ent origin
        DEBUG_RENDER_BSPHERE_F      = (1 << 10),    //!< display ent bounding sphere

        // Bits 16..23 are system flags that only core code can modify
        FRAME_OWNED_F               = (1 << 16),    //!< ent owns its frame
        DYING_F                     = (1 << 17),    //!< ent will be deleted at next call to UpdateEnts()
        BEHAVIOUR_OWNED_F           = (1 << 18),    //!< ent owns its behaviour
        PRIVATE_FLAGS_MASK          = 0xff0000,

        IN_WORLD_F                  = (1 << 24),    //!< ent is added to a CIwWorld
        USER_F                      = (1 << 25),    //!< start user flags from here
    };

    // Methods
    CIwEnt();
    virtual ~CIwEnt();

    /** Do all entity initialisation */
    virtual void    Init();

    /** Do all entity update */
    virtual void    Update();

    /** Do all entity collision */
    virtual void    Collide() {};

    /** Binary input/output */
    virtual void    Serialise();

    /** Resolve pointers */
    virtual void    Resolve();

    /** Set behaviour, mark as owned by Ent */
    void    SetBehaviour(CIwBehaviour* pBe);

    /** Set behaviour, mark as not owned by Ent */
    void    SetBehaviourShared(CIwBehaviour* pBe);

    /** Clear behaviour */
    void    ClearBehaviour();

    /** Get behaviour */
    inline CIwBehaviour* GetBehaviour() const {return m_Behaviour;}

    /** Write flags */
    inline void WriteFlags(uint32 flags)
    {
        IwAssertMsgN(280,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags = (m_Flags & PRIVATE_FLAGS_MASK) | flags;
    }

    /** Set flags */
    inline void SetFlags(uint32 flags)
    {
        IwAssertMsgN(281,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags |= flags;
    }

    /** Clear flags */
    inline void ClearFlags(uint32 flags)
    {
        IwAssertMsgN(282,ENGINE, !(flags & PRIVATE_FLAGS_MASK), ("Trying to modify system flags"));
        m_Flags &= ~flags;
    }

    /** Get flags */
    inline uint32 GetFlags() const {return m_Flags;}

    /** Is this flag set? */
    inline bool IsFlag(uint32 flag) const {return (m_Flags & flag) != 0;}

    /**
     * Kill the ent.
     * Call this if deleting the entity immediately would cause iteration probs.
     * The entity will be deleted after it is updated.
     * Any functionality that needs to happen when this happens should go in the OnDeath() virtual.
     */
    virtual void Die() {m_Flags |= DYING_F;}

    /**
     * Kill the ent.
     * Call this if you're sure deleting immediately won't cause iteration probs.
     */
    void DieNow()
    {
        OnDeath();
        delete this;
    }

    /** Called when we're about to be deleted. */
    virtual void OnDeath();

    /** Check if ent is dying. */
    inline bool IsDying() const {return (m_Flags & DYING_F) != 0;}

    /** Set a frame, mark as owned by ent */
    void SetFrameOwned(CIwFrame* pFrame);

    /** Set a frame, mark as NOT owned by ent */
    void SetFrameShared(CIwFrame* pFrame);

    /** Get read access to local frame */
    inline const CIwMat* GetMatLocal() const
    {
        IwAssertMsgN(283,ENGINE, m_Frame, ("Entity does not have frame"));
        return m_Frame->GetMatLocal();
    }

    /** Get read access to world frame */
    inline const CIwMat* GetMatWorld() const
    {
        IwAssertMsgN(284,ENGINE, m_Frame, ("Entity does not have frame"));
        return m_Frame->GetMatWorld();
    }

    /** Get write access to local matrix */
    inline CIwMat* ExposeMatLocal() const
    {
        IwAssertMsgN(285,ENGINE, m_Frame, ("Entity does not have frame"));
        return m_Frame->ExposeMatLocal();
    }

    /** Get frame */
    inline CIwFrame* GetFrame() const {return m_Frame;}

    /** Get current world position */
    inline const CIwVec3& GetPosLocal() const
    {
        IwAssertMsgN(286,ENGINE, m_Frame, ("Entity does not have frame"));
        return m_Frame->GetMatLocal()->GetTrans();
    }

    /** Get current world position */
    inline const CIwVec3& GetPosWorld() const
    {
        IwAssertMsgN(287,ENGINE, m_Frame, ("Entity does not have frame"));
        return m_Frame->GetMatWorld()->GetTrans();
    }

    /** Update a list of ents. */
    static void UpdateList(CIwManagedList& list);

    /** Initialise a list of ents. */
    static void InitList(CIwManagedList& list);

    /** Delete a list of ents. */
    static void DeleteList(CIwManagedList& list);

    /** Parse from text file: parse attribute/value pair, return true only if attribute parsed. */
    bool    ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);

    /** Parse from text file: end block. */
    void    ParseClose(CIwTextParserITX* pParser);

    /** Proximity queries */
    inline bool TestDistSq(CIwEnt* pEnt, iwfixed distSq) const;
    inline bool TestDistSq(const CIwVec3& vPos, iwfixed distSq) const;
    inline iwfixed GetDistSq(CIwEnt* pEnt) const;

    /**
     * Handle events of all types.
     * @param pEvent the event
     * @return true if the event was handled, false otherwise
     */
    bool    HandleEvent(CIwEvent* pEvent);


#ifdef IW_DEBUG
    /** Debug rendering */
    void    DebugRender();

    /** Debug method to add items to menu */
    void    DebugAddMenuItems(CIwMenu* pMenu);
#endif

public:
    // Members
    CIwSphere       m_BSphere;      //!< bounding sphere

protected:
    //-------------------------------------------------------------------------
    // Private API
    //-------------------------------------------------------------------------
    uint32          m_Flags;        //!< e.g. DEBUG_RENDER_NAME_F
    CIwFrame*       m_Frame;        //!< coordinate frame
    CIwBehaviour*   m_Behaviour;    //!< current behaviour, or NULL

private:
    //-------------------------------------------------------------------------
    // Private utils
    //-------------------------------------------------------------------------

    friend class CIwWorld;
};

//-----------------------------------------------------------------------------
// Inlines
//-----------------------------------------------------------------------------
// Proximity queries
inline bool CIwEnt::TestDistSq(CIwEnt* pEnt, iwfixed distSq) const
{
    return TestDistSq(pEnt->GetPosLocal(), distSq);
}

inline bool CIwEnt::TestDistSq(const CIwVec3& vPos, iwfixed distSq) const
{
    const CIwVec3 dist = vPos - GetPosLocal();
    return dist.GetLengthSquared() < distSq;
}

inline iwfixed CIwEnt::GetDistSq(CIwEnt* pEnt) const
{
    const CIwVec3 dist = pEnt->GetPosLocal() - GetPosLocal();
    return dist.GetLengthSquared();
}

#endif

/** @} */
