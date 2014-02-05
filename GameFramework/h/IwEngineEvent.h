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
#ifndef IWENGINEEVENTS_H
#define IWENGINEEVENTS_H

// Library includes
#include "IwGeomVec3.h"
#include "IwEvent.h"

//-----------------------------------------------------------------------------
// Engine Events
//-----------------------------------------------------------------------------

enum EVENT_TABLE_IWENGINE
{
    Event_EntSetPos = IW_EVENT_ENGINE,
    Event_EntOffset,
    Event_EntSetRotY,
};

//--------------------------------------------------------------------------------
// forward declarations

//--------------------------------------------------------------------------------
// events

class CIwEventEntSetPos : public CIwEvent
{
    IW_EVENT_DECLARE(CIwEventEntSetPos, Event_EntSetPos);

    CIwVec3 m_Pos;
    CIwEventEntSetPos(const CIwVec3 &pos) : m_Pos(pos) { Create(); }
};

class CIwEventEntOffset : public CIwEvent
{
    IW_EVENT_DECLARE(CIwEventEntOffset, Event_EntOffset);

    CIwVec3 m_Off;
    CIwEventEntOffset(const CIwVec3 &off) : m_Off(off) { Create(); }
};

class CIwEventEntSetRotY : public CIwEvent
{
    IW_EVENT_DECLARE(CIwEventEntSetRotY, Event_EntSetRotY);

    iwangle m_Rot;
    CIwEventEntSetRotY(iwangle rot) : m_Rot(rot) { Create(); }
};

#endif
