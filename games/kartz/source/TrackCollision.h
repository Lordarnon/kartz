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
 * @file TrackCollision.h
 * @brief Defines walkable areas of the world mesh
 */
//--------------------------------------------------------------------------------

#ifndef TRACKCOLLISION_H
#define TRACKCOLLISION_H

#include "IwArray.h"
#include "IwGeomVec.h"
#include "IwVolume.h"


/**
 * @class CTrackCollision
 *      This class encapsulates the information required to detect collision with the
 *      racing track.
 */
class CTrackCollision
{
public:
    CTrackCollision(CIwSVec3 const * vert, int numVerts);

    //--------------------------------------------------------------------------------
    // Property API
    //--------------------------------------------------------------------------------

    /** Gets the centre point of a polygon. */
    CIwVec3 GetCentroid(int polyID);

    /** Retrieves the normal a polygon. */
    CIwVec3 GetNormal(int polyID);

    /** Gets the vertex of a polygon. */
    CIwVec3 GetVertex(int polyID, int id);

    int NumPolys() { return m_Poly.size(); }

    CIwSVec3 const & operator [] (int i) const {
        return GetVert(i);
    }
    CIwSVec3 const & GetVert (int i) const {
        IwAssert(RCD, i<m_NumVerts);
        return i<0 ? m_ExtraVert[-i] : m_Vert[i];
    }

    //--------------------------------------------------------------------------------
    // Movement API
    //--------------------------------------------------------------------------------

    int FindFloor(const CIwVec3 & position);

    struct MoveInfo
    {
        int32       m_Height;
        CIwVec3     m_GroundNormal;
        CIwVec3     m_WallPosition;
        CIwVec3     m_WallNormal;
        bool        m_OverTrack;
    };
    int MoveFloor(int startPoly, CIwVec3 const & start_position, CIwVec3 position, int r, int height, MoveInfo& out);

    int WallPush(const int startPoly, CIwVec3 & position, CIwVec3 & normal, int r);

    //--------------------------------------------------------------------------------
    // Construction API
    //--------------------------------------------------------------------------------

    int AddVert(CIwSVec3 const & a, int pos);

    int AddTri(int ai, int bi, int ci);

    void FinishedAdd();

#ifdef IW_DEBUG
    void DrawDebug(int tile, int colour, int level);
#endif //IW_DEBUG

private:
    /**
     * @struct Poly
     *      Simple structure containing data and methods required to represent a triangle polygon.
     */
    struct Poly
    {
        /** Indices of the vertices that form the point of the triangle. */
        short v[3];

        /** Indices of the adjacent triangle to this triangle. */
        short adj[3];

        int Join(Poly & p, int idThis, int idP, int i)
        {
            for (int j=0; j<3; j++)
                if (adj[i] == -1 && p.adj[j] == -1 && v[i] == p.v[(j + 1) % 3] && v[(i + 1) % 3] == p.v[j])
                {
                    adj[i] = idP;
                    p.adj[j] = idThis;
                    return 1;
                }
            return 0;
        }

        CIwVec3 GetPerpendicular(int i, CTrackCollision const & vertices) const
        {
            int j=((i == 2) ? 0 : i + 1);
            CIwVec3 perp(vertices[v[j]].z-vertices[v[i]].z, 0, vertices[v[i]].x-vertices[v[j]].x);
            perp.Normalise();
            return perp;
        }

        int GetHeight(CIwVec3 const & position, CTrackCollision const & vertices) const
        {
            CIwVec3 v1 = GetPerpendicular(0, vertices);
            CIwVec3 v2 = GetPerpendicular(2, vertices);
            CIwVec3 p  = position - vertices[v[0]];
            int d1 = (vertices[v[2]] - vertices[v[0]]) * v1;
            int d2 = (vertices[v[1]] - vertices[v[0]]) * v2;
            if (!d1 || !d2)
                return vertices[v[0]].y;
            int i1 = IW_FIXED_DIV( (p * v1), d1 );
            int i2 = IW_FIXED_DIV( (p * v2), d2 );
            return vertices[v[0]].y
                + IW_FIXED_MUL_SAFE( vertices[v[1]].y - vertices[v[0]].y, i2 )
                + IW_FIXED_MUL_SAFE( vertices[v[2]].y - vertices[v[0]].y, i1 );
        }

        int Distance(CIwVec3 const & position, CTrackCollision const & vertices) const
        {
            int closest = 0;
            for (int i=0; i<3; i++)
            {
                int dist = (position - vertices[v[i]]) * GetPerpendicular(i, vertices);
                if (dist <= 0)
                {
                    if (i == 0 || (dist > closest && closest <= 0))
                        closest = dist;
                }
                else
                {
                    if (dist<closest || closest<=0)
                        closest = dist;
                }
            }
            return closest;
        }

        int MovePoint(CIwVec3 & position, int oldPoly, CTrackCollision const & vertices, MoveInfo& out) const
        {
            bool inTri = true;
            for (int i=0; i<3; i++)
            {
                if (oldPoly >= 0 && oldPoly == adj[i])
                    continue; //don't go back into old poly

                CIwVec3 perp = GetPerpendicular(i, vertices);
                int dist = (position - vertices[v[i]]) * perp;
                if (dist > 2)
                {
                    inTri = false;
                    if (adj[i] == -1)
                    {
                        // Move into triangle
                        position -= dist * perp;

                        out.m_WallNormal = perp;

                        // Re-examine all edges
                        if (i != 0) i=-1;
                        continue;
                    }
                    // Move into triangle
                    else
                        return adj[i];
                }
            }
            if (!inTri)
            {
                out.m_OverTrack = false;
                out.m_WallPosition = position;
            }
            return -1;
        }
    };

    static int Hash(int v0, int v1);

    CIwVec3 GetPerpendicular(int i, int j) const;

    int FindClosestVert(CIwSVec3 const & v, int tolerance);
    int ReLinkSlow(int, int16& a, int16& b);


    CIwSVec3 const*     m_Vert;
    int                 m_NumVerts;
    CIwArray<Poly>      m_Poly;
    CIwArray<CIwSVec3>  m_ExtraVert;

    #define MAX_HASH  15
    #define MAX_TABLE 0x400

    struct HashEntry {
        short size;
        short x[MAX_HASH];

        HashEntry() : size(0)
        {}

        void Add(short y)
        {
            IwAssert(RCD, size<MAX_HASH);
            x[size++] = y;
        }

        void Remove(short y)
        {
            for (int i=0; i<MAX_HASH; i++)
                if (x[i] == y)
                {
                    x[i] = x[size-1];
                    size--;
                    return;
                }
        }
    };

    CIwArray< HashEntry > m_HashEdge;   // From hash of centre of edge to poly IDs
};

#endif // !TRACKCOLLISION_H
