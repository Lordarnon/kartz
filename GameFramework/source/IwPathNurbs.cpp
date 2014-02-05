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
/*
 * Path: beziers.
 */

#include "IwPathNurbs.h"
#include "IwTextParserITX.h"

#ifdef IW_DEBUG
#include "IwDebugPrim.h"
#endif

IW_CLASS_FACTORY(CIwPathNurbs);

#define MAX_POLYNOMIAL_TERMS 5
class CIwPolynomial
{
public:
    iwfixed m_coeff[MAX_POLYNOMIAL_TERMS];

    CIwPolynomial();
    //CIwPolynomial(iwfixed a);
    CIwPolynomial(iwfixed b, iwfixed a);

    iwfixed Evaluate(iwfixed t);
    iwfixed EvaluateDifferential(iwfixed t);

    CIwPolynomial& operator=(const iwfixed that);
    CIwPolynomial& operator=(const CIwPolynomial& that);
    CIwPolynomial& operator += (const CIwPolynomial& that);

    bool operator!=(const iwfixed that);

    CIwPolynomial operator*(const iwfixed that);
    CIwPolynomial operator*(const CIwPolynomial& that);
    CIwPolynomial operator/(const iwfixed that);
    CIwPolynomial operator+(const CIwPolynomial& that);
};

IW_MANAGED_IMPLEMENT(CIwPathNurbs)

CIwPolynomial::CIwPolynomial()
{
    IW_CALLSTACK("CIwPolynomial::CIwPolynomial")

    for (int i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        m_coeff[i] = 0;
    }
}

CIwPolynomial::CIwPolynomial(iwfixed b, iwfixed a)
{
    IW_CALLSTACK("CIwPolynomial::CIwPolynomial")

    m_coeff[0] = a;
    m_coeff[1] = b;

    for (int i=2;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        m_coeff[i] = 0;
    }
}

CIwPolynomial& CIwPolynomial::operator=(const iwfixed that)
{
    IW_CALLSTACK("CIwPolynomial::operator=")

    m_coeff[0] = that;
    for (int i=1;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        m_coeff[i] = 0;
    }
    return *this;
}

CIwPolynomial& CIwPolynomial::operator += (const CIwPolynomial& that)
{
    IW_CALLSTACK("CIwPolynomial::operator+=")

    *this = *this + that;
    return *this;
}

CIwPolynomial& CIwPolynomial::operator=(const CIwPolynomial& that)
{
    IW_CALLSTACK("CIwPolynomial::operator=")

    for (int i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        m_coeff[i] = that.m_coeff[i];
    }
    return *this;
}

bool CIwPolynomial::operator!=(const iwfixed that)
{
    IW_CALLSTACK("CIwPolynomial::operator!=")

    if (m_coeff[0] != that) return true;

    for (int i=1;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        if (m_coeff[i] != 0) return true;
    }
    return false;
}

CIwPolynomial CIwPolynomial::operator/(const iwfixed that)
{
    IW_CALLSTACK("CIwPolynomial::operator/")

    CIwPolynomial res;
    for (int i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        res.m_coeff[i]= ( m_coeff[i]/ (that>>IW_GEOM_POINT) );
    }
    return res;
}

CIwPolynomial CIwPolynomial::operator+(const CIwPolynomial& that)
{
    IW_CALLSTACK("CIwPolynomial::operator+")

    CIwPolynomial res;
    for (int i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        res.m_coeff[i]=m_coeff[i]+that.m_coeff[i];
    }
    return res;
}

CIwPolynomial CIwPolynomial::operator*(const CIwPolynomial& that)
{
    CIwPolynomial res;

    int i,j;
    for (i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        for (j=0;j<MAX_POLYNOMIAL_TERMS;j++)
        {
            int power = i+j;
            if (power < MAX_POLYNOMIAL_TERMS)
                res.m_coeff[power] += IW_FIXED_MUL_SAFE(m_coeff[i],that.m_coeff[j]);
        }
    }
    return res;
}

CIwPolynomial CIwPolynomial::operator*(const iwfixed that)
{
    CIwPolynomial res;

    int i;
    for (i=0;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        res.m_coeff[i] = IW_FIXED_MUL_SAFE(m_coeff[i],that);
    }
    return res;
}

iwfixed CIwPolynomial::Evaluate(iwfixed t)
{
    IW_CALLSTACK("CIwPolynomial::Evaluate")

    iwfixed res(0);
    iwfixed powr(IW_GEOM_ONE);

    res = m_coeff[0];

    for (int i=1;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        powr = IW_FIXED_MUL_SAFE(powr,t);
        res += IW_FIXED_MUL_SAFE(m_coeff[i],powr);
    }

    return res;
}

iwfixed CIwPolynomial::EvaluateDifferential(iwfixed t)
{
    IW_CALLSTACK("CIwPolynomial::EvaluateDifferential")

    iwfixed res(0);
    iwfixed powr(IW_GEOM_ONE);

    res = m_coeff[1];

    for (int i=2;i<MAX_POLYNOMIAL_TERMS;i++)
    {
        powr = IW_FIXED_MUL_SAFE(powr,t);
        res += IW_FIXED_MUL_SAFE(i*m_coeff[i],powr);
    }

    return res;
}

//-----------------------------------------------------------------------------
// CIwPathNurbs
//-----------------------------------------------------------------------------
CIwPathNurbs::CIwPathNurbs()
: m_Order(3)
{
    for (int i=0;i<3;i++)
    {
        m_SegPolynomials[i] = NULL;
    }
}
//-----------------------------------------------------------------------------
CIwPathNurbs::~CIwPathNurbs()
{
    IW_CALLSTACK("CIwPathNurbs::~CIwPathNurbs")

    for (int i=0;i<3;i++)
    {
        if (m_SegPolynomials[i])
        {
            delete [] m_SegPolynomials[i];
            m_SegPolynomials[i] = NULL;
        }
    }
    if (m_SegSumPolynomials)
    {
        delete [] m_SegSumPolynomials;
        m_SegSumPolynomials = NULL;
    }
}
//-----------------------------------------------------------------------------
void    CIwPathNurbs::SetNumPoints(uint32 n)
{
    IW_CALLSTACK("CIwPathNurbs::SetNumPoints")
    IwAssertMsgN(354,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));

    m_NumPoints     = n;
    m_NumSegments   = (m_Flags & CLOSED_F) ? m_NumPoints-3 : (m_NumPoints - 1);

    // Allocate space for segments
    m_Segments  = new Segment[m_NumSegments];

    // Allocate space for points
    uint32  numVecs = m_NumPoints;
    IwAssert(ENGINE, !m_Points);
    if (n)
        m_Points = new CIwVec3[numVecs];
}
//-----------------------------------------------------------------------------
void    CIwPathNurbs::Init()
{
    IW_CALLSTACK("CIwPathNurbs::Init")

    #define PATH_LENGTH_LINES_PER_SEG 10

    // Only initialise once
    if (TestFlag(INITIALISED_F))
        return;

    IwAssertMsgN(355,ENGINE, !IsInitialised(), ("Path %s is already initialised", DebugGetName()));
    IwAssertMsgN(356,ENGINE, m_Points, ("Path %s has no points", DebugGetName()));
    IwAssertMsgN(357,ENGINE, m_Segments, ("Path %s has no segments", DebugGetName()));

    Segment*    pSeg    = m_Segments;
    int32       d       = 0;
    CIwVec3 v0;

    // Calculate segment lengths and entire path length (if not done so already)
    if (!(m_Flags & INITIALISED_F))
    {
        //m_Order = ((m_Flags & CLOSED_F))? 4 : 5; // I don't know why but these values match my tests in Maya the best

        for (int i=0;i<3;i++)
        {
            m_SegPolynomials[i] = new CIwPolynomial[m_NumSegments];
        }
        m_SegSumPolynomials = new CIwPolynomial[m_NumSegments];
        ComputeSegPolynomials();

        for (uint32 s = 0; s < m_NumSegments; s++)
        {
            // Store path length at start of segment
            pSeg->m_PathLength  = d;

            CIwVec3 last_pos;
            CIwVec3 next_pos;
            GetNurbsPos(&last_pos, s, 0);
            pSeg->m_SegLength = 0;

            for (uint32 n = 1; n <= PATH_LENGTH_LINES_PER_SEG; n++)
            {
                GetNurbsPos(&next_pos, s, (n << IW_GEOM_POINT) / PATH_LENGTH_LINES_PER_SEG);

                v0 = next_pos - last_pos;
                last_pos = next_pos;
                pSeg->m_SegLength += v0.GetLength();
            }
            d += pSeg->m_SegLength;
            pSeg++;
        }

        // Store entire path length
        m_Length = d;
    }
    // Superclass
    CIwPath::Init();
}
//-----------------------------------------------------------------------------
void    CIwPathNurbs::CalcPointAtDist(int32 d, CIwVec3* pPoint, CIwSVec3* pTangent, uint32* pSegID)
{
    IW_CALLSTACK("CIwPathNurbs::CalcPointAtDist")
    IwAssertMsgN(358,ENGINE, IsInitialised(), ("Path %s is not initialised", DebugGetName()));

    // Note: this can be slow
    Segment*    pSeg    = m_Segments;
    int32       s       = m_NumSegments;
    int32       prevID  = 0;
    CIwVec3*    pPoint0 = m_Points;
    CIwVec3*    pPoint1 = m_Points + 1;
    int32       f;

    if (!(m_Flags & CLOSED_F))
    {
        // Open path
        if (d >= (int32)m_Length)
        {
            if (pPoint)     GetNurbsPos(pPoint, m_NumPoints - 2, IW_GEOM_ONE);
            if (pTangent)   GetNurbsTangent(pTangent, m_NumPoints - 2, IW_GEOM_ONE);
            if (pSegID)     *pSegID = m_NumSegments - 1;
            return;
        }
        else
        if (d <= 0)
        {
            if (pPoint)     GetNurbsPos(pPoint, 0, 0);
            if (pTangent)   GetNurbsTangent(pTangent, 0, 0);
            if (pSegID)     *pSegID = 0;
            return;
        }
    }
    else
    {
        // Closed path... get d in range 0..m_Length
        if (d >= (int32)m_Length)
        {
            while (d >= (int32)m_Length)
                d -= m_Length;
        }
        else
        if (d < 0)
        {
            while (d < 0)
                d += m_Length;
        }
    }

    while (s--)
    {
        if (d <= (pSeg->m_PathLength + pSeg->m_SegLength))
        {
            // Calculate position on this segment
            f   = ((d - pSeg->m_PathLength) << IW_GEOM_POINT) / pSeg->m_SegLength;
            if (pPoint)     GetNurbsPos(pPoint, prevID, f);
            if (pTangent)   GetNurbsTangent(pTangent, prevID, f);
            if (pSegID)     *pSegID = m_NumSegments - s;
            return;
        }
        pSeg++;
        pPoint0++;
        pPoint1++;
        prevID++;
        if (pPoint1 >= &m_Points[m_NumPoints])
            pPoint1 = m_Points;
    }
    IwAssertMsgN(359,ENGINE, false, ("d within path length but not within a segment"));
}
//-----------------------------------------------------------------------------
bool    CIwPathNurbs::IsPointInside(const CIwVec3& p)
{
    IW_CALLSTACK("CIwPathNurbs::IsPointInside")

    // NOTE: WE CHECK THE SHAPE ENCLOSED BY THE LINES JOINING THE CONTROL POINTS
    return CIwPathLine::IsPointInside(p);
}

//-----------------------------------------------------------------------------
void    CIwPathNurbs::Serialise()
{
    IW_CALLSTACK("CIwPathNurbs::Serialise")

    // Base class
    CIwPathLine::Serialise();
}
//-----------------------------------------------------------------------------
bool    CIwPathNurbs::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
    IW_CALLSTACK("CIwPathNurbs::ParseAttribute")

    return CIwPathLine::ParseAttribute(pParser, pAttrName);
}

//
// Nurbs Helpers
//

// Find polynomial basis functions
void rbasispoly(int c,int s,int npts,int* x,CIwPolynomial* r, CIwPolynomial& sum)
{
    IW_CALLSTACK("rbasispoly")

    int nplusc;
    int i,k;

    CIwPolynomial dp, ep;

    nplusc = npts + c;

    CIwPolynomial* poly = new CIwPolynomial[nplusc+1];

    iwfixed t_offset( (c-1+s)<<IW_GEOM_POINT );

/* calculate the first order nonrational basis functions n[i]   */

    for (i = 1; i<= nplusc-1; i++)
    {
        if (( t_offset+IW_GEOM_HALF >= (x[i]<<IW_GEOM_POINT)) && (t_offset+IW_GEOM_HALF < (x[i+1]<<IW_GEOM_POINT)))
        {
            poly[i] = IW_GEOM_ONE;
        }
        else
        {
            poly[i] = 0;
        }
    }

/* calculate the higher order nonrational basis functions */

    for (k = 2; k <= c; k++)
    {
        for (i = 1; i <= nplusc-k; i++)
        {
            if (poly[i] != 0)    /* if the lower order basis function is zero skip the calculation */
                dp = (CIwPolynomial(IW_GEOM_ONE,t_offset-(x[i]<<IW_GEOM_POINT))*poly[i])/((x[i+k-1]-x[i])<<IW_GEOM_POINT);
            else
                dp = 0;

            if (poly[i+1] != 0)     /* if the lower order basis function is zero skip the calculation */
                ep = (CIwPolynomial(-IW_GEOM_ONE,-t_offset+(x[i+k]<<IW_GEOM_POINT))*poly[i+1])/((x[i+k]-x[i+1])<<IW_GEOM_POINT);
            else
                ep = 0;

            poly[i] = dp + ep;
        }
    }

/* calculate sum for denominator of rational basis functions */

    sum = 0;
    for (i = 1; i <= npts; i++){
            sum = sum + poly[i];
    }

/* form rational basis functions and put in r vector */

    for (i = 1; i <= npts; i++){
        if (sum != 0)
        {
            r[i] = poly[i];
        }
        else
        {
            r[i] = 0;
        }
    }

    delete [] poly;
}

// Generate a B-spline uniform (periodic) knot vector.
void knotu(int n,int c,int* x)
{
    IW_CALLSTACK("knotu")

    int nplusc, i;

    nplusc = n + c;

    x[1] = 0;
    for (i = 2; i <= nplusc; i++)
    {
        x[i] = i-1;
    }
}

void    CIwPathNurbs::ComputeSegPolynomials()
{
    IW_CALLSTACK("CIwPathNurbs::ComputeSegPolynomials")

    int i;
//  iwfixed st;
    int npts = m_NumPoints;
    int k = m_Order;
    int nplusc = npts + k;
    int num_samples;
    int* x = new int[nplusc+k+k+3];

    //  zero the knot vector
    for (i = 0; i <= nplusc; i++)
    {
        x[i] = 0;
    }

    // Generate the uniform knot vector
    num_samples = npts;
    knotu(num_samples,k,x);

    CIwPolynomial* nbasis = new CIwPolynomial[num_samples+1];

    for (uint32 s = 0; s < m_NumSegments; s++)
    {
        // Clear polinomial
        m_SegPolynomials[0][s] = 0;
        m_SegPolynomials[1][s] = 0;
        m_SegPolynomials[2][s] = 0;

        // Zero the basis array
        for (i = 0; i <= npts; i++)
        {
             nbasis[i] = 0;
        }

        // calculate the polynomials of the rational B-spline curve
        rbasispoly(k,s,num_samples,x,nbasis,m_SegSumPolynomials[s]);      /* generate the basis function for this value of t */

        for (i = 1; i <= num_samples; i++)
        {
            m_SegPolynomials[0][s] += nbasis[i]*m_Points[x[i]].x;
            m_SegPolynomials[1][s] += nbasis[i]*m_Points[x[i]].y;
            m_SegPolynomials[2][s] += nbasis[i]*m_Points[x[i]].z;
        }

    }

    delete [] x;
    delete [] nbasis;
}
//-----------------------------------------------------------------------------
void    CIwPathNurbs::GetNurbsPos(CIwVec3* pPos, int32 prevID, int32 _t)
{
    IW_CALLSTACK("CIwPathNurbs::GetNurbsPos")

    if (!pPos)
        return;

//  iwfixed st;
    //int k = m_Order;

    // calculate the points on the rational B-spline curve
    //st = ((k+prevID-1)<<IW_GEOM_POINT) + _t;

    iwfixed sum( m_SegSumPolynomials[prevID].Evaluate(_t) );

    pPos->x = IW_FIXED_DIV(m_SegPolynomials[0][prevID].Evaluate(_t),sum);
    pPos->y = IW_FIXED_DIV(m_SegPolynomials[1][prevID].Evaluate(_t),sum);
    pPos->z = IW_FIXED_DIV(m_SegPolynomials[2][prevID].Evaluate(_t),sum);
}
//-----------------------------------------------------------------------------
void    CIwPathNurbs::GetNurbsTangent(CIwSVec3* pVec, int32 prevID, int32 _t)
{
    IW_CALLSTACK("CIwPathNurbs::GetNurbsTangent")

    if (!pVec)
        return;

    /*
        Differential of
            a(x) = f(x)/g(x)
            a'(x) = (f'(x)g(x) - f(x)g'(x)) / g(x)^2
    */

    CIwVec3 f, f1;
    int g, g1;

    g = m_SegSumPolynomials[prevID].Evaluate(_t);

    g1 = m_SegSumPolynomials[prevID].EvaluateDifferential(_t);

    f.x = m_SegPolynomials[0][prevID].Evaluate(_t);
    f.y = m_SegPolynomials[1][prevID].Evaluate(_t);
    f.z = m_SegPolynomials[2][prevID].Evaluate(_t);

    f1.x = m_SegPolynomials[0][prevID].EvaluateDifferential(_t);
    f1.y = m_SegPolynomials[1][prevID].EvaluateDifferential(_t);
    f1.z = m_SegPolynomials[2][prevID].EvaluateDifferential(_t);

    CIwVec3 a;

    int gg = IW_FIXED_MUL(g,g);
    a.x = IW_FIXED_DIV((IW_FIXED_MUL(f1.x, g) - IW_FIXED_MUL(f.x, g1)), gg);
    a.y = IW_FIXED_DIV((IW_FIXED_MUL(f1.y, g) - IW_FIXED_MUL(f.y, g1)), gg);
    a.z = IW_FIXED_DIV((IW_FIXED_MUL(f1.z, g) - IW_FIXED_MUL(f.z, g1)), gg);

    // currently, a will sometimes be zero (?!)
    if (a.IsZero () == false)
        a.NormaliseSafe();

    *pVec = a;
}

//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifdef IW_DEBUG
void    CIwPathNurbs::DebugRender()
{
    IW_CALLSTACK("CIwPathNurbs::DebugRender")

#define PATH_DEBUG_LINES_PER_SEG    10

    if (m_Flags & DEBUG_RENDER_F)
    {
        // Render segments as curves (multiple lines)
        int32       numP    = PATH_DEBUG_LINES_PER_SEG * (m_NumSegments);
        CIwVec3*    pts     = new CIwVec3[numP];
        CIwVec3*    pVec    = pts;
        for (uint32 s = 0; s < m_NumSegments; s++)
        {
            for (uint32 n = 0; n < PATH_DEBUG_LINES_PER_SEG; n++)
            {
                GetNurbsPos(pVec, s, (n << IW_GEOM_POINT) / PATH_DEBUG_LINES_PER_SEG);
                pVec++;
            }
        }
        IwGxDebugPrimLineStrip(pts, numP, m_DebugColour, (m_Flags & CLOSED_F) != 0);
        delete[] pts;
    }

    // Base class
//  CIwPath::DebugRender();
    if (m_Flags & DEBUG_RENDER_POINT_IDS_F)
        DebugRenderPointIDs();
}
#endif  // IW_DEBUG
