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
#ifndef IW_INTERPOLATION_H
#define IW_INTERPOLATION_H

// Includes
#include "s3eTypes.h"

/**
 * @defgroup IwInterpolation IwInterpolation
 */

/**
 * @addtogroup IwInterpolation
 * @brief Classes and functions for general value interpolation.
 * @{
 */

//-----------------------------------------------------------------------------
// General helpers for angles etc.
//-----------------------------------------------------------------------------
/** Potentially change phase of dest to give min numerical diff between curr and dest */
int32   IwInterpMinimiseAngleDiff(int32 curr, int32& dest);

/** Potentially change phase of curr to stay in range -PI..PI */
void    IwInterpBindAnglePhase(int32& curr);

/** Potentially change phase of (curr, dest) */
void    IwInterpBindAnglePhase(int32& curr, int32& dest);

//-----------------------------------------------------------------------------
// CIwInterpS
//-----------------------------------------------------------------------------
/** Templated class for interpolated value, immediate */
template<class X, int32 div>
class CIwInterpS
{
public:
    // Methods
    inline  void    SetCurr(const X& val)   {m_Curr = val;}
    inline  X       GetCurr()               {return m_Curr;}
    inline  void    SetDest(const X& val)   {m_Dest = val;}
    inline  X       GetDest()               {return m_Dest;}
    inline  void    Interpolate()
    {
        m_Curr += (m_Dest - m_Curr) / div;
    }
    inline  X*      DebugGetCurrPtr()       {return &m_Curr;}
    inline  X*      DebugGetDestPtr()       {return &m_Dest;}

    // Operator overloads
    X& operator = (const X& val)            {m_Curr = m_Dest = val; return m_Curr;}

    // Members
private:
    X   m_Curr;
    X   m_Dest;
};

//-----------------------------------------------------------------------------
// CIwInterpT
//-----------------------------------------------------------------------------
/** Templated class for interpolated value, timed */
template<class X>
class CIwInterpT
{
public:
    // Methods
    inline  CIwInterpT() : m_Timer(0) {}

    inline  void    SetCurr(const X& val)   {m_Curr = val;}
    inline  X       GetCurr()               {return m_Curr;}
    inline  void    SetDest(const X& val)   {m_Dest = val;}
    inline  X       GetDest()               {return m_Dest;}
    inline  void    StartTimer(uint32 t)
    {
        m_Timer = t;
        m_Delta = ((m_Dest - m_Curr) << 12) / t;
    }
    inline  void    Interpolate()
    {
        if (m_Timer > 0)
        {
            if (!(--m_Timer))
                m_Curr = m_Dest;
            else
                m_Curr += m_Delta;
        }
    }

    // Operator overloads
    X& operator = (const X& val)            {m_Curr = m_Dest = val; return m_Curr;}

    // Members
private:
    X       m_Curr;
    X       m_Dest;
    X       m_Delta;
    uint32  m_Timer;
};

#endif

// @}
