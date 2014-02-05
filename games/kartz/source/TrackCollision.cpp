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
#include "RacingDemo.h"
#include "TrackCollision.h"

#include "IwDebugPrim.h"
#include "IwVolumeBBox.h"

#define DELTA 1 // Extra move-out with collisions

//-------------------------------------------------------------------------------------------------

static int moveLine(CIwVec3 & to, CIwVec3 const & block1, CIwVec3 const & block2, int r)
{
    IW_CALLSTACK("moveLine")

    CIwVec3 perp(block1.z-block2.z, 0, block2.x-block1.x);
    perp.Normalise();

    int x = (to - block1) * (block2 - block1);
    if (x<0 || x>(block2 - block1)*(block2 - block1))
        return 0;

    CIwVec3 B = to-block1;
    int v2 = perp * B;
    if (v2>=r) return 0;
    if (v2<-r/2) return 0;
    to -= perp * (v2-r - DELTA);
    return 1;
}

//-------------------------------------------------------------------------------------------------

// Note: r2 must be r*r (integer, NOT fixed point multiply - the numbers it's supposed to work with are too small for fixed point to work!)
static int movePoint(CIwVec3 & to, CIwVec3 const & block, int r, int r2)
{
    IW_CALLSTACK("movePoint")

    CIwVec3 Y = to - block;
    if (Y.x*Y.x+Y.z*Y.z < r2)
    {
        Y.y = 0;
        to = block + Y.GetNormalisedSafe() * (r + DELTA);
        return 1;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------

// Returns the intersect of walls specified by normals after moving the walls forwards by r
// Used to determine where to position a circle (radius r) such that it touches both walls.
static CIwVec3 GetChunkyIntersection(CIwVec3 pos1, CIwVec3 const& norm1, CIwVec3 pos2, CIwVec3 const& norm2, int r, CIwVec3 const & fallback)
{
    IW_CALLSTACK("GetChunkyIntersection")

    pos1 -= norm1 * r;
    pos2 -= norm2 * r;

    //CIwColour c = {0,0,255};
    //IwDebugPrimLine(pos1, pos1-norm1*100, c);
    //IwDebugPrimLine(pos2, pos2-norm2*100);

    //CIwVec3 dir1(-norm1.z, 0, norm1.x);
    //CIwVec3 dir2(-norm2.z, 0, norm2.x);
    //dir1 *=200; dir2*=200;
    //IwDebugPrimLine(pos1-dir1, pos1+dir1);
    //IwDebugPrimLine(pos2-dir2, pos2+dir2, c);

    // Recenter to as if pos1==(0,0,0)
    pos2 -= pos1;

    // Solve A.norm1=0  and  A.norm2 = pos2.norm2 ( = K )
    int K = pos2 * norm2;

    //      Ax.x1 + Ay.y1 = 0
    // ->   Ax = - Ay.y1 / x1

    //      Ax.x2 + Ay.y2 - K = 0
    // ->   -(Ay.y1 / x1).x2 + Ay.y2 - K = 0
    // ->   Ay(y2 - y1*x2/x1) = K
    // ->   Ay = K / (y2 - y1*x2/x1)
    // ->   Ay = K.x1 / (y2.x1 - y1.x2)

    CIwVec3 A(0,0,0);
    int div = IW_FIXED_MUL(norm2.z, norm1.x) - IW_FIXED_MUL(norm1.z, norm2.x);
    if (div == 0) return fallback;
    A.z = K * norm1.x / div;
    A.x = K * norm1.z / -div;

    A += pos1;

    //IwDebugPrimLine(A, A+CIwVec3(0,1000,0), c);

    return A;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::Hash(int v0, int v1)
{
    IW_CALLSTACK("CTrackCollision::Hash")

    int hashv = v0 ^ (v1 << 7);
    return hashv & (MAX_TABLE-1);
}

//-------------------------------------------------------------------------------------------------

CIwVec3 CTrackCollision::GetPerpendicular(int i, int j) const
{
    CIwVec3 perp(GetVert(j).z-GetVert(i).z, 0, GetVert(i).x-GetVert(j).x);
    perp.Normalise();
    return perp;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::FindClosestVert(CIwSVec3 const & v, int tolerance)
{
    IW_CALLSTACK("CTrackCollision::FindClosestVert")

    CIwVec3 vl (v);
    int closest = -1;
    int dist = tolerance;
    for (int i=0; i<m_NumVerts; i++)
    {
        int d = (m_Vert[i]-vl).GetLengthSquaredUnshifted();
        if (d < dist)
            closest=i, dist=d;
    }
    return closest;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::ReLinkSlow(int polyID, int16& v1, int16& v2)
{
    IW_CALLSTACK("CTrackCollision::ReLinkSlow")

    int i1 = FindClosestVert(GetVert(v1), (1000*1000));
    int i2 = FindClosestVert(GetVert(v2), (1000*1000));
    if (i1<0 || i2<0 || i1 == i2)
        return -1;

    for (uint32 p=0; p<m_Poly.size(); p++)
        for (int i=0; i<3; i++)
            if (m_Poly[p].adj[i] == -1 && m_Poly[p].v[i]==i2 && m_Poly[p].v[(i+1)%3]==i1)
            {
                m_Poly[p].adj[i] = polyID;
//              v1 = i1;
//              v2 = i2;
                return p;
            }

    return -1;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::WallPush(const int startPoly, CIwVec3& position, CIwVec3& normal, int r)
{
    IW_CALLSTACK("CTrackCollision::WallPush")

    if (startPoly<0) return 0;

    int r2 = r*r;

    CIwArray<int> polys;
    CIwArray<int> edges;
    int pBuf[60];
    int eBuf[60];
    polys.Share(pBuf, 0, sizeof(pBuf)/sizeof(pBuf[0]));
    edges.Share(eBuf, 0, sizeof(eBuf)/sizeof(pBuf[0]));

    polys.append(startPoly);
    uint32 head=0;
    int scanr = r*2;    // Scan slightly larger area for interesting polygons so we don't overlook ones we might be interested in after the first collision
    while (head < polys.size())
    {
        Poly & p = m_Poly[polys[head++]];
        for (int i=0; i<3; i++)
        {
            CIwVec3 const perp = p.GetPerpendicular(i, *this);
            int dist = perp * (position - GetVert(p.v[i]));

            if (dist < -scanr || dist > scanr) continue;

            CIwSVec3 const& v1 = GetVert(p.v[i]);
            CIwSVec3 const& v2 = GetVert(p.v[(i+1) % 3]);
            if (position.x<MIN(v1.x, v2.x)-scanr) continue;
            if (position.x>MAX(v1.x, v2.x)+scanr) continue;
            if (position.z<MIN(v1.z, v2.z)-scanr) continue;
            if (position.z>MAX(v1.z, v2.z)+scanr) continue;

            if (p.adj[i] == -1)
            {
                edges.append( (p.v[i]&0xffff) | p.v[(i+1)%3] << 16);
            }
            else
            {
                if (polys.find(p.adj[i]) != -1) continue;
                polys.append(p.adj[i]);
            }
        }
    }

    int32 i;
    int motion = 0;
    int lastEdge = 0;

    for (i=0; i<(int32)edges.size(); i++)
    {
        if (edges[i] == lastEdge) continue;

        int a = (int16)(edges[i] & 0xffff);
        int b = edges[i]>>16;
        if (moveLine(position, CIwVec3(GetVert(a)), CIwVec3(GetVert(b)), r))
        {
            normal = GetPerpendicular(a, b);
            motion = 1;
            if (lastEdge)
            {
                CIwVec3 p1 = GetPerpendicular((int16)(lastEdge & 0xffff), lastEdge>>16);
                CIwVec3 p2 = GetPerpendicular(a, b);
                int d = p1 * p2;
                // If the angle is _really_ acute, give up (else we tend to generate ridiculous positions)
                if (d<0 && d>-0xff0)
                {
                    // Special case... moving into acute angled corner
                    position = GetChunkyIntersection(
                        CIwVec3(GetVert(a)),
                        p2,
                        CIwVec3(GetVert((int16)(lastEdge & 0xffff))),
                        p1,
                        r+DELTA,
                        position );
                    break;
                }
                // Note: if something collides with >2 edges at once it might not get handled correctly at the moment.
            }
            else
            {
                lastEdge = edges[i];
                i=-1;   // restart loop
            }
        }
    }

    for (i=0; i<(int32)edges.size(); i++)
    {
        int a = (int16)(edges[i] & 0xffff);
        int b = edges[i]>>16;

        if (movePoint(position, CIwVec3(GetVert(a)), r, r2) )
        {
            normal = GetPerpendicular(a, b);
            motion = 1;
        }

        if (movePoint(position, CIwVec3(GetVert(b)), r, r2) )
        {
            normal = GetPerpendicular(a, b);
            motion = 1;
        }
//      motion |= movePoint(position, CIwVec3(GetVert(a)), r, r2);
//      motion |= movePoint(position, CIwVec3(GetVert(b)), r, r2);
    }

    #ifdef IW_DEBUG
        if (0)
        {
            // Render which polygons we searched
            DrawDebug(-1,0,-1);
            DrawDebug(polys[0], 0xffff00, 0);

            for (i=0; i<(int32)polys.size(); i++)
            {
                DrawDebug(polys[i], 0x00ff00, 0);
            }
        }
    #endif

    return motion;
}

//-------------------------------------------------------------------------------------------------

CTrackCollision::CTrackCollision(CIwSVec3 const * vert, int numVerts)
{
    IW_CALLSTACK("CTrackCollision::CTrackCollision")

    m_Vert = vert;
    m_NumVerts = numVerts;
    m_ExtraVert.reserve(20); // TEMP

    m_HashEdge.resize(MAX_TABLE);
}

//-------------------------------------------------------------------------------------------------

void CTrackCollision::FinishedAdd()
{
    IW_CALLSTACK("CTrackCollision::FinishedAdd")

    m_HashEdge.clear_optimised();
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::FindFloor(const CIwVec3 & position)
{
    IW_CALLSTACK("CTrackCollision::FindFloor")

    int bestHeight = 0;
    int bestTriID = -1;

    // Find the closest floor triangle which is in line with the X&Z components of the position.
    for (uint32 i=0; i != m_Poly.size(); i++)
    {
        if (m_Poly[i].Distance(position, *this) <= 0)
        {
            const int newHeight = m_Poly[i].GetHeight(position, *this) - position.y;
            if (bestTriID == -1 || ABS(newHeight) < ABS(bestHeight))
                bestTriID = i, bestHeight = newHeight;
        }
    }

    return bestTriID;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::MoveFloor(int startPoly, CIwVec3 const & start_position, CIwVec3 position, int r, int height, CTrackCollision::MoveInfo& out)
{
    IW_CALLSTACK("CTrackCollision::MoveFloor")

    memset(&out, 0, sizeof(out));
    out.m_OverTrack = true;

    if (startPoly<0 || startPoly>(int)m_Poly.size())
    {
        startPoly = FindFloor(start_position);
        if (startPoly<0)
        {
            IwAssertMsg(RCD, false, ("CTrackCollision::Start position is not over the track"));
            return -1;
        }
    }

    int oldPoly = -1;
    short poly = startPoly;
    while (1)
    {
        int nextPoly = m_Poly[poly].MovePoint(position, oldPoly, *this, out);

        if (nextPoly == poly || nextPoly==-1)
        {
            break;
        }

        oldPoly = poly;
        poly = nextPoly;
    }

    out.m_Height = m_Poly[poly].GetHeight(position, *this);
    out.m_GroundNormal = GetNormal(poly);

    return poly;
}

//-------------------------------------------------------------------------------------------------

int CTrackCollision::AddTri(int ai, int bi, int ci)
{
    IW_CALLSTACK("CTrackCollision::AddTri")

    IwAssert(RCD, m_HashEdge.size() == MAX_TABLE);

    CIwSVec3 const & a = m_Vert[ai];
    CIwSVec3 const & b = m_Vert[bi];
    CIwSVec3 const & c = m_Vert[ci];

    if (a == b || a==c || b==c)
        return -1;

    int idP = m_Poly.push_back(Poly());
    Poly & p = m_Poly[idP];

    p.v[0] = ai; p.v[1] = bi; p.v[2] = ci;
    p.adj[0] = p.adj[1] = p.adj[2] = -1;

    for (int i=0; i<3; i++)
    {
        const int v0 = p.v[i];
        const int v1 = p.v[(i == 2)?0:i+1];

        HashEntry& hash = m_HashEdge[Hash(v0, v1)];

        bool linked = false;

        for (int j=0; j<hash.size; j++)
        {
            int other = hash.x[j];

            if (p.Join(m_Poly[other], idP, other, i))
            {
                hash.Remove(other);
                linked = true;
                break;
            }
        }

        if (!linked)
        {
            // Note: Edges are added to the (v1,v0) hash, and searched for by (v0,v1)
            m_HashEdge[Hash(v1, v0)].Add(idP);
        }
    }

    return idP;
}

//-------------------------------------------------------------------------------------------------

CIwVec3 CTrackCollision::GetCentroid(int polyID)
{
    IwAssert(RCD, polyID>=0 && polyID<(int)m_Poly.size());
    CIwVec3 c(0,0,0);
    for (int i=0; i<3; i++)
        c += GetVert(m_Poly[polyID].v[i]);
    return c/IW_FIXED(3);
}

//-------------------------------------------------------------------------------------------------

CIwVec3 CTrackCollision::GetVertex(int polyID, int i)
{
    IwAssert(RCD, polyID>=0 && polyID<(int)m_Poly.size());
    IwAssert(RCD, i>=0 && i<3);
    return (CIwVec3) GetVert(m_Poly[polyID].v[i]);
}

//-------------------------------------------------------------------------------------------------

CIwVec3 CTrackCollision::GetNormal(int polyID)
{
    CIwSVec3 v0 = GetVert(m_Poly[polyID].v[0]);
    CIwSVec3 s1 = GetVert(m_Poly[polyID].v[2]) - v0;
    CIwSVec3 s2 = GetVert(m_Poly[polyID].v[1]) - v0;

    CIwVec3 v = (CIwVec3)s1.Cross(s2);
    v.Normalise();
    if (v.y < 0)
        v *= -IW_GEOM_ONE;

    return v;
}

//-------------------------------------------------------------------------------------------------

#ifdef IW_DEBUG
    void CTrackCollision::DrawDebug(int tile, int colour, int level)
    {
/*colour=0x000080;
for (int q=0; q<m_Poly.size(); q++)
{
    for (int i=0; i<3; i++)
    {
        int j = (i == 2) ? 0 : i+1;
        IwDebugPrimLine(
            CIwVec3( m_Vert[m_Poly[q].v[i]] ),
            CIwVec3( m_Vert[m_Poly[q].v[j]] ),
            *(CIwColour*)&colour
            );
    }
}
return;*/

        static short tt[2000];
        static int ttq=0;
        if (level == -1)
        {
            ttq=0;
            return;
        }

        if (tile < 0)
            return;

        if (ttq == 2000) return;
        for (int j=0; j<ttq; j++)
            if (tt[j] == tile)
                return;
        tt[ttq++] = tile;

        if (!colour) colour=0x000080;

        for (int i=0; i<3; i++)
        {
            int j = (i == 2) ? 0 : i+1;
            IwGxDebugPrimLine(
                CIwVec3( GetVert(m_Poly[tile].v[i]) ),
                CIwVec3( GetVert(m_Poly[tile].v[j]) ),
                *(CIwColour*)&colour
                );
        }

        if (level)
            for (int i=0; i<3; i++)
                DrawDebug(m_Poly[tile].adj[i], colour>>8, level-1);

        if (colour == 0xffffff)
        {
            CIwVec3 a = (CIwVec3)GetVert(m_Poly[tile].v[0]);
            CIwVec3 b = (CIwVec3)GetVert(m_Poly[tile].v[1]);
            CIwVec3 c = (CIwVec3)GetVert(m_Poly[tile].v[2]);
            CIwVec3 normal = (a-b).GetNormalisedSafe() ^ (b-c).GetNormalisedSafe();
            normal.NormaliseSafe();
            int cc = 0xff00ff;
            CIwVec3 mid = (a+b+c) * int(IW_GEOM_ONE / 3.0);
            IwGxDebugPrimLine(
                mid,
                mid - normal * 0x100,
                *(CIwColour*)&cc
                );
            cc = 0x808080;

            IwGxDebugPrimLine(mid, a, *(CIwColour*)&cc);
            IwGxDebugPrimLine(mid, b, *(CIwColour*)&cc);
            IwGxDebugPrimLine(mid, c, *(CIwColour*)&cc);
        }
    }
#endif //IW_DEBUG

//-------------------------------------------------------------------------------------------------
