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
#ifndef BESTLINETRACKER_H
#define BESTLINETRACKER_H

//little utility class to encapsulate following a best line

class CBestLineTracker
{
public:
    CBestLineTracker();

    CIwVec3 GetPoint() const                            { return m_Point; }
    CIwVec3 GetDir() const                              { return m_Dir; }
    iwfixed GetDistance(const CIwVec3& position) const; //how far has this car driven?

    /**
     * Update the line tracker based on a cars current position.
     *  @param  position
     *      Current position of a car on the track.
     */
    void Update(const CIwVec3& position);

#ifdef IW_DEBUG
    void DebugRender();
#endif

private:

    /** Initialilse the best line tracker. */
    void Init(const CIwVec3& position);

    void SetPoint(iwfixed distance);
    iwfixed GetDistanceToPoint(const CIwVec3& position) const;

    iwfixed m_Distance; //how far around the curve the last point was

    /** Has this instance been initilaised. */
    bool m_Inited;

    //cached information

    /** This is the next target point which racers will hear towards. */
    CIwVec3 m_Point;

    CIwVec3 m_Dir;

    CIwVec3 m_LastPoint;
    CIwVec3 m_LastDir;

    /** What point did the race start at? */
    static iwfixed m_DistanceStart;
};

#endif // !BESTLINETRACKER_H
