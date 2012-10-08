#include "StdAfx.h"
#include "MathHelper.h"

#include "Const.h"

#include "GBaseNode.h"
#include "GLine.h"

MathHelper::MathHelper(void)
{
}

MathHelper::~MathHelper(void)
{
}
int MathHelper::RestrictAngle( int angle )
{
	int anglebase_360 = ANGLEBASE_90*4;
	while (angle < 0)
	{
		angle += anglebase_360;
	}
	while (angle >= anglebase_360)
	{
		angle -= anglebase_360;
	}
	return angle;
}

int MathHelper::LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx/*=0*/, float * inty/*=0*/ )
{
	/*
	float s1_x, s1_y, s2_x, s2_y;
    s1_x = x2 - x1;     s1_y = y2 - y1;
    s2_x = x4 - x3;     s2_y = y4 - y3;
	*/

	double mua,mub;
	double denom,numera,numerb;
	/*
	denom = (-s2_x * s1_y + s1_x * s2_y);
	numera = (-s1_y * (x1 - x3) + s1_x * (y1 - y3));
	numerb = ( s2_x * (y1 - y3) - s2_y * (x1 - x3));
	*/
	
	denom  = (y4-y3) * (x2-x1) - (x4-x3) * (y2-y1);
	numera = (x4-x3) * (y1-y3) - (y4-y3) * (x1-x3);
	numerb = (x2-x1) * (y1-y3) - (y2-y1) * (x1-x3);
	
	/* Are the line coincident? */
	if (fabsf(numera) < FLT_MIN && fabsf(numerb) < FLT_MIN && fabsf(denom) < FLT_MIN)
	{
		float retx, rety;
		if ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1) < (x4-x3)*(x4-x3)+(y4-y3)*(y4-y3))
		{
			retx = (x1+x2)/2;
			rety = (y1+y2)/2;
		}
		else
		{
			retx = (x3+x4)/2;
			rety = (y3+y4)/2;
		}
		if (intx)
		{
			*intx = retx;
		}
		if (inty)
		{
			*inty = rety;
		}

		return MHLINEINTERSECT_COINCIDENT;
	}

	/* Are the line parallel */
	if (fabsf(denom) < FLT_MIN) {
		if (intx)
		{
			*intx = 0;
		}
		if (inty)
		{
			*inty = 0;
		}
		return MHLINEINTERSECT_NOINTERSECT;
	}

	/* Is the intersection along the the segments */
	mua = numera / denom;
	mub = numerb / denom;
	if (mua < -M_FLOATEXTREMEEPS || mua > 1+M_FLOATEXTREMEEPS || mub < -M_FLOATEXTREMEEPS || mub > 1+M_FLOATEXTREMEEPS) {
		if (intx)
		{
			*intx = 0;
		}
		if (inty)
		{
			*inty = 0;
		}
		return MHLINEINTERSECT_NOINTERSECT;
	}
	if (intx)
	{
		*intx = x1 + mua * (x2 - x1);
	}
	if (inty)
	{
		*inty = y1 + mua * (y2 - y1);
	}
	return MHLINEINTERSECT_INTERSECT;
}

bool MathHelper::LinePartialInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb, bool nocalc/*=false*/ )
{
	// Inside Rect
	if (LineInRect(x1, y1, x2, y2, xl, yt, xr, yb))
	{
		return true;
	}
	// Totally Outside Rect
	if (!RectIntersectRect(min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2), xl, yt, xr, yb))
	{
		return false;
	}

	if (nocalc)
	{
		return true;
	}

	if (LineSegmentIntersect(x1, y1, x2, y2, xl, yt, xr, yt))
	{
		return true;
	}
	if (LineSegmentIntersect(x1, y1, x2, y2, xr, yt, xr, yb))
	{
		return true;
	}
	if (LineSegmentIntersect(x1, y1, x2, y2, xr, yb, xl, yb))
	{
		return true;
	}
	if (LineSegmentIntersect(x1, y1, x2, y2, xl, yb, xl, yt))
	{
		return true;
	}
	return false;
}

bool MathHelper::LineInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb )
{
	if (x1 >= xl && x1 <= xr
		&& x2 >= xl && x2 <= xr
		&& y1 >= yt && y1 <= yb
		&& y2 >= yt && y2 <= yb)
	{
		return true;
	}
	return false;
}

bool MathHelper::RectIntersectRect( float xl1, float yt1, float xr1, float yb1, float xl2, float yt2, float xr2, float yb2 )
{
	if (xr1 < xl2 || xl1 > xr2 || yt1 > yb2 || yb1 < yt2)
	{
		return false;
	}
	return true;
}

bool MathHelper::CalculateCatmullRom( const PointF2D &p1, const PointF2D &p2, const PointF2D &p3, const PointF2D &p4, float s, PointF2D * pq )
{
	if (s < 0 || s > 1)
	{
		return false;
	}
	float s3 = powf(s, 3);
	float s2 = powf(s, 2);
	if (pq)
	{
		(*pq) = ((p1*(-s3+2*s2-s)+p2*(3*s3-5*s2+2)+p3*(-3*s3+4*s2+s)+p4*(s3-s2))/2.0f);
		return true;
	}
	return false;
}

bool MathHelper::CalculateBezier( const PointF2D &pb, const PointF2D &pbh, const PointF2D &peh, const PointF2D &pe, float s, PointF2D * pq )
{
	if (pq)
	{
		*pq = pb*powf(1-s, 3) + pbh*(3*s*(powf(1-s, 2))) + peh*(3*powf(s, 2)*(1-s)) + pe*powf(s,3);
		return true;
	}
	return false;
}

bool MathHelper::CalculateBezierSubDivision( const QuadBezierPointF2D quadBezier, float s, QuadBezierPointF2D * pQuadHandles )
{
	PointF2D p12 = (quadBezier.ptbh-quadBezier.ptb)*s+quadBezier.ptb;
	PointF2D p23 = (quadBezier.pteh-quadBezier.ptbh)*s+quadBezier.ptbh;
	PointF2D p34 = (quadBezier.pte-quadBezier.pteh)*s+quadBezier.pteh;

	PointF2D p123 = (p23-p12)*s+p12;
	PointF2D p234 = (p34-p23)*s+p23;

	if (pQuadHandles)
	{
		pQuadHandles->ptb = p12;
		pQuadHandles->pte = p34;
		pQuadHandles->ptbh = p123;
		pQuadHandles->pteh = p234;
	}
	return true;
}

bool MathHelper::SolveBezierExtendSubDivision( const QuadBezierPointF2D quadBezier, float s, PointF2D * pnewa, PointF2D * pnewbh, PointF2D * pneweh )
{
	if (s >=0 && s <= 1)
	{
		ASSERT(false);
		return false;
	}
	if (s < 0.0f)
	{
		s = (-s)/(1.0f-s);
		float msinv = 1/(1-s);

		// p1234 = pb
		// p234 = pbh
		// p4 = pe
		// p34 = peh
		// ?pnewa = p1
		// ?pnewh = p2
		PointF2D p123 = (quadBezier.ptb-quadBezier.ptbh*s)*msinv;
		PointF2D p23 = (quadBezier.ptbh-quadBezier.pteh*s)*msinv;
		PointF2D p12 = (p123-p23*s)*msinv;
		PointF2D p3 = (quadBezier.pteh-quadBezier.pte*s)*msinv;
		PointF2D p2 = (p23-p3*s)*msinv;
		PointF2D p1 = (p12-p2*s)*msinv;
		if (pnewa)
		{
			*pnewa = p1;
		}
		if (pnewbh)
		{
			*pnewbh = p2;
		}
		if (pneweh)
		{
			*pneweh = p3;
		}

		return true;
	}
	if (s > 1.0f)
	{
		s = (s-1.0f)/s;
		float msinv = 1/(1-s);

		// p1234 = pe
		// p123 = peh
		// p1 = pb
		// p12 = pbh
		// ?pnewa = p4
		// ?pnewh = p3
		PointF2D p234 = (quadBezier.pte-quadBezier.pteh*s)*msinv;
		PointF2D p23 = (quadBezier.pteh-quadBezier.ptbh*s)*msinv;
		PointF2D p34 = (p234-p23*s)*msinv;
		PointF2D p2 = (quadBezier.ptbh-quadBezier.ptb*s)*msinv;
		PointF2D p3 = (p23-p2*s)*msinv;
		PointF2D p4 = (p34-p3*s)*msinv;
		if (pnewa)
		{
			*pnewa = p4;
		}
		if (pnewbh)
		{
			*pnewbh = p2;
		}
		if (pneweh)
		{
			*pneweh = p3;
		}

		return true;
	}
	return false;
}

float MathHelper::LineSegmentLength( const PointF2D &p1, const PointF2D &p2 )
{
	float l = LineSegmentLengthPow2(p1, p2);
	if (l == 0.0f)
	{
		return 0.0f;
	}
	return sqrtf(l);
}

float MathHelper::LineSegmentLengthPow2( const PointF2D &p1, const PointF2D &p2 )
{
	return powf(p1.x-p2.x, 2)+powf(p1.y-p2.y, 2);
}

bool MathHelper::PointInRect( float px, float py, float xl, float yt, float w, float h )
{
	return px > xl && px < xl+w && py > yt && py < yt+h;
}

bool MathHelper::PointInCircle( float px, float py, float cx, float cy, float r )
{
	if (!PointInRect(px, py, cx-r, cy-r, 2*r, 2*r))
	{
		return false;
	}
	return powf(px-cx, 2)+powf(py-cy, 2) < r*r;
}

float MathHelper::NearestPointOnStraightLine( float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny )
{
	return sqrtf(NearestPointOnStraightLinePow2(px, py, lx1, ly1, lx2, ly2, nx, ny));
}

float MathHelper::NearestPointOnStraightLinePow2( float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny )
{
	PointF2D v(lx1, ly1);
	PointF2D w(lx2, ly2);
	PointF2D p(px, py);

	if (v.Equals(w))
	{
		if (nx) { *nx = lx1; }
		if (ny) { *ny = ly1; }
		return LineSegmentLength(v, p);
	}

	float length2 = LineSegmentLengthPow2(v, w);
	float t = (p-v).Dot(w-v)/length2;
	if (t < 0.0f)
	{
		if (nx) { *nx = lx1; }
		if (ny) { *ny = ly1; }
		return LineSegmentLength(v, p);
	}
	else if (t > 1.0f)
	{
		if (nx) { *nx = lx2; }
		if (ny) { *ny = ly2; }
		return LineSegmentLength(w, p);
	}

	PointF2D proj = v+(w-v)*t;
	if (nx) { *nx = proj.x; }
	if (ny) { *ny = proj.y; }
	return LineSegmentLengthPow2(proj, p);

}

bool MathHelper::PointNearToStraightLine( float px, float py, float x1, float y1, float x2, float y2, float r, float * plx, float * ply )
{
	if (!PointInRectTwoPoint(px, py, x1, y1, x2, y2, r))
	{
		return false;
	}
	float nx, ny;
	float distpow2 = NearestPointOnStraightLinePow2(px, py, x1, y1, x2, y2, &nx, &ny);
	if (plx)
	{
		*plx = nx;
	}
	if (ply)
	{
		*ply = ny;
	}
	if (distpow2 < r*r)
	{
		return true;
	}
	return false;
}

bool MathHelper::PointInRectTwoPoint( float px, float py, float x1, float y1, float x2, float y2, float r/*=0*/ )
{
	return PointInRect(px, py, min(x1, x2)-r, min(y1, y2)-r, fabsf(x1-x2)+2*r, fabsf(y1-y2)+2*r);
}

void MathHelper::GetPerpendicularPointForLine( const PointF2D &pt1, const PointF2D &pt2, float s, float l, bool bUpward, PointF2D* ptp )
{
	int angle = GetLineAngle(pt1, pt2);
	angle += ANGLEBASE_90;
	RestrictAngle(&angle);
	if (angle >= 0)
	{
		angle += ANGLEBASE_180;
	}
	float xdiff = l*cost(angle);
	float ydiff = l*sint(angle);
	float xbase = (pt2.x-pt1.x)*s+pt1.x;
	float ybase = (pt2.y-pt1.y)*s+pt1.y;
	if (ptp)
	{
		ptp->x = xbase+xdiff;
		ptp->y = ybase+ydiff;
	}
}

int MathHelper::GetLineAngle( const PointF2D &pt1, const PointF2D &pt2 )
{
	/*
	if (pt1.Equals(pt2))
	{
		return 0;
	}
	*/
	// Range: [-pi, pi]
	return ANGLE(atan2f(pt2.y-pt1.y, pt2.x-pt1.x));
}

void MathHelper::RestrictAngle( int* angle )
{
	if (angle)
	{
		while (*angle > ANGLEBASE_180)
		{
			*angle-=ANGLEBASE_360;
		}
		while (*angle <= -ANGLEBASE_180)
		{
			*angle+=ANGLEBASE_360;
		}
	}
}

bool MathHelper::GetLineSegmentInRect( float x, float y, int angle, float lx, float ty, float rx, float by, float* x1, float* y1, float* x2, float* y2 )
{
	RestrictAngle(&angle);
	if (angle < 0)
	{
		angle += ANGLEBASE_180;
	}
	if (angle == ANGLEBASE_180)
	{
		angle = 0;
	}

	float tpx[2];
	float tpy[2];
	bool bret = false;

	if (angle == 0)
	{
		if (x >= lx && x <= rx)
		{
			tpx[0] = tpx[1] = x;
			tpy[0] = ty;
			tpy[1] = by;
			bret = true;
		}
	}
	else if (angle == 90)
	{
		if (y >= ty && y <= by)
		{
			tpy[0] = tpy[1] = y;
			tpx[0] = lx;
			tpx[1] = rx;
			bret = true;
		}
	}
	else
	{
		float ftg = tanf(ARC(angle));

		int nowindex = 0;
		// Check Left
		tpy[nowindex] = y - (x-lx)*ftg;
		if (tpy[nowindex] >= ty && tpy[nowindex] <= by)
		{
			tpx[nowindex] = lx;
			nowindex++;
		}
		// Check Top
		tpx[nowindex] = x - (y-ty)/ftg;
		if (tpx[nowindex] >= lx && tpx[nowindex] <= rx)
		{
			tpy[nowindex] = ty;
			if (nowindex && tpx[nowindex]==tpx[nowindex-1] && tpy[nowindex]==tpy[nowindex-1])
			{
			}
			else
			{
				nowindex++;
			}
		}

		// Check Right
		if (nowindex < 2)
		{
			tpy[nowindex] = y - (x-rx)*ftg;
			if (tpy[nowindex] >= ty && tpy[nowindex] <= by)
			{
				tpx[nowindex] = rx;
				nowindex++;
			}
		}
		// Check Bottom
		if (nowindex < 2)
		{
			tpx[nowindex] = x - (y-by)/ftg;
			if (tpx[nowindex] >= lx && tpx[nowindex] <= rx)
			{
				tpy[nowindex] = by;
				if (nowindex && tpx[nowindex]==tpx[nowindex-1] && tpy[nowindex]==tpy[nowindex-1])
				{
				}
				else
				{
					nowindex++;
				}
			}
		}
		//
		if (nowindex == 2)
		{
			bret = true;
		}
	}
	if (bret)
	{
		if (x1) { *x1 = tpx[0]; }
		if (y1) { *y1 = tpy[0]; }
		if (x2) { *x2 = tpx[1]; }
		if (y2) { *y2 = tpy[1]; }
		return true;
	}
	return false;
}

float MathHelper::CalculateProportionOnStraightLine( float xb, float yb, float xe, float ye, float x, float y )
{
	float xdiff = xe-xb;
	float ydiff = ye-yb;
	if (fabsf(xdiff) > fabsf(ydiff))
	{
		return (x-xb)/xdiff;
	}
	else
	{
		if (!ydiff)
		{
			return 0;
		}
		else
		{
			return (y-yb)/ydiff;
		}
	}
	return 0;
}

bool MathHelper::FindPerpendicularPoint( float x, float y, float lx, float ly, int langle, float * px/*=NULL*/, float * py/*=NULL */ )
{
	PointF2D v(lx, ly);
	PointF2D w(lx+cost(langle), ly+sint(langle));
	PointF2D p(x, y);
	
	float length2 = 1.0f;
	float t = (p-v).Dot(w-v)/length2;

	PointF2D proj = v+(w-v)*t;
	if (px) { *px = proj.x; }
	if (py) { *py = proj.y; }
	return true;
}

bool MathHelper::LineIntersectCircle( PointF2D lO, PointF2D lD, PointF2D C, float r, PointF2D * ptIntersections/*=NULL*/, PointF2D * ptNormals/*=NULL*/ )
{
	PointF2D d = lO-C;
	float a = lD.Dot(lD);
	float b = d.Dot(lD);
	float c = d.Dot(d) - r*r;

	float disc = b*b-a*c;
	if (disc < 0.0f)
	{
		return false;
	}

	if (ptIntersections || ptNormals)
	{
		float t[2];
		float sqrtDisc = sqrtf(disc);
		t[0] = (-b-sqrtDisc) / a;
		t[1] = (-b+sqrtDisc) / a;

		for (int i=0; i<2; i++)
		{
			if (ptIntersections)
			{
				ptIntersections[i] = lO+lD*t[i];
			}
			if (ptNormals)
			{
				ptNormals[i] = (ptIntersections[i]-C)/r;
			}
		}
	}

	return true;
}

bool MathHelper::LineIntersectLine( PointF2D p, PointF2D u, PointF2D q, PointF2D v, PointF2D * ptIntersection/*=NULL*/, float * pfs/*=NULL*/ )
{
	float D = u.y*v.x-u.x*v.y;
	if (fabsf(D) < M_FLOATEPS)
	{
		return false;
	}
	float Ds = (q.y - p.y) * v.x - (q.x - p.x) * v.y;
	float s = Ds / D;
	/*
	if (s < 0.0f || s > 1.0f)
	{
		return false;
	}
	float Dt = (q.y - p.y) * u.x - (q.x - p.x) * u.y;
	float t = Dt / D;
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}
	*/
	if (ptIntersection)
	{
		*ptIntersection = p+u*s;
	}
	if (pfs)
	{
		*pfs = s;
	}
	return true;
}

int MathHelper::GetQuadrant( float x, float y, float xo/*=0*/, float yo/*=0 */ )
{
	if (x>=xo)
	{
		if (y>=yo)
		{
			return QUADRANT_1;
		}
		return QUADRANT_4;
	}
	if (y>=yo)
	{
		return QUADRANT_2;
	}
	return QUADRANT_3;
}

bool MathHelper::FindNearestHandlePointForGivenBezierLength_TwoPoint( float fLength, const PointF2D &ptFirstAnchor, const PointF2D &ptFirstHandle, const PointF2D &ptSecondAnchor, PointF2D ptNear, PointF2D ptFar, float * px/*=NULL*/, float * py/*=NULL*/ )
{
	// !!TODO??
	return false;


	GBaseNode tbase;

	GBezierLine * pBezier = new GBezierLine(&tbase, ptFirstAnchor, ptFirstHandle, ptSecondAnchor, ptSecondAnchor);
	float fMinimalLength = pBezier->getLength();
	if (fLength < fMinimalLength)
	{
		tbase.RemoveAllChildren(true);
		return false;
	}

	if (ptNear.Equals(ptFar))
	{
		tbase.RemoveAllChildren(true);
		return false;
	}

	bool bRet = false;
	if (FindSubNHPFGBL(pBezier, fLength, ptNear, ptFar, px, py))
	{
		bRet = true;
	}
	tbase.RemoveAllChildren(true);
	return bRet;

}

bool MathHelper::FindNearestHandlePointForGivenBezierLength( float fLength, const PointF2D &ptFirstAnchor, const PointF2D &ptFirstHandle, const PointF2D &ptSecondAnchor, float cx, float cy, float * px/*=NULL*/, float * py/*=NULL*/, bool bCheckLengthAvailableOnly/*=false*/ )
{
	GBaseNode tbase;

	GBezierLine * pBezier = new GBezierLine(&tbase, ptFirstAnchor, ptFirstHandle, ptSecondAnchor, ptSecondAnchor);
	float fMinimalLength = pBezier->getLength();
	if (fLength < fMinimalLength)
	{
		tbase.RemoveAllChildren(true);
		return false;
	}
	if (bCheckLengthAvailableOnly)
	{
		tbase.RemoveAllChildren(true);
		return true;
	}

	PointF2D ptNow(cx, cy);
	PointF2D ptDiff = ptNow-ptSecondAnchor;
	PointF2D ptFar(ptSecondAnchor.x+fLength*2, ptSecondAnchor.y);

	if (fabsf(ptDiff.x) > fabsf(ptDiff.y))
	{
		float fmul = fabsf(2*fLength / ptDiff.x);
		ptFar = ptDiff*fmul+ptSecondAnchor;
	}
	else if (ptDiff.y)
	{
		float fmul = fabsf(2*fLength / ptDiff.y);
		ptFar = ptDiff*fmul+ptSecondAnchor;
	}

	bool bRet = false;
	if (FindSubNHPFGBL(pBezier, fLength, ptSecondAnchor, ptFar, px, py, fMinimalLength))
	{
		bRet = true;
	}

	tbase.RemoveAllChildren(true);
	return bRet;
}

bool MathHelper::FindSubNHPFGBL( GBezierLine * pBezier, float fTargetLength, PointF2D ptNear, PointF2D ptFar, float * px, float * py, float fNearLength/*=-1*/, float fFarLength/*=-1 */ )
{
	if (ptFar.Equals(ptNear))
	{
//		DASSERT(false);
		if (px) { *px = ptNear.x; }
		if (py) { *py = ptNear.y; }
		return false;
	}
	if (fNearLength < 0)
	{
		pBezier->SetEndHandlePos(ptNear.x, ptNear.y);
		fNearLength = pBezier->getLength();
	}
	if (fFarLength < 0)
	{
		pBezier->SetEndHandlePos(ptFar.x, ptFar.y);
		fNearLength = pBezier->getLength();
	}

	PointF2D ptMid = (ptFar+ptNear)/2.0f;
	pBezier->SetEndHandlePos(ptMid.x, ptMid.y);
	float fMidLength = pBezier->getLength();
	if (fabsf(fMidLength-fTargetLength)<M_FLOATEPS)
	{
		if (px) { *px = ptMid.x; }
		if (py) { *py = ptMid.y; }
		return true;
	}

	if (fMidLength > fTargetLength)
	{
		ptFar = ptMid;
		fFarLength = fMidLength;
	}
	else
	{
		ptNear = ptMid;
		fNearLength = fMidLength;
	}

	if (FindSubNHPFGBL(pBezier, fTargetLength, ptNear, ptFar, px, py, fNearLength, fFarLength))
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Use Double                                                           */
/************************************************************************/
double cubeRoot(double x)
{
	if (x < 0)
		return -pow(-x, 0.333333333333333);
	else
		return pow(x, 0.333333333333333);
}
int MathHelper::SolveCubicEquation( float a, float b, float c, float d, float x[3] )
{
	// cubic equation solver example using Cardano's method
	
	// this function returns the cube root if x were a negative number as well

	// find the discriminant
	double f, g, h;
	f = (3 * c / a - pow(b, 2) / pow(a, 2)) / 3;
	g = (2 * pow(b, 3) / pow(a, 3) - 9 * b * c / pow(a, 2) + 27 * d / a) / 27;
	h = pow(g, 2) / 4 + pow(f, 3) / 27;
	// evaluate discriminant
	if (f == 0 && g == 0 && h == 0)
	{
		// 3 equal roots
		double outx;
		// when f, g, and h all equal 0 the roots can be found by the following line
		outx = -cubeRoot(d / a);
		x[0] = outx;
		return 1;
	}
	else if (h <= 0)
	{
		// 3 real roots
		double q, i, j, k, l, m, n, p;
		// complicated maths making use of the method
		i = pow(pow(g, 2) / 4 - h, 0.5);
		j = cubeRoot(i);
		k = acos(-(g / (2 * i)));
		m = cos(k / 3);
		n = 1.73205080756888 * sin(k / 3);
		p = -(b / (3 * a));
		// print solutions
		x[0] = 2 * j * m + p;
		x[1] = -j * (m + n) + p;
		x[2] = -j * (m - n) + p;
		return 3;
	}
	else if (h > 0)
	{
		// 1 real root and 2 complex roots
		double r, s, t, u, p;
		// complicated maths making use of the method
		r = -(g / 2) + pow(h, 0.5);
		s = cubeRoot(r);
		t = -(g / 2) - pow(h, 0.5);
		u = cubeRoot(t);
		p = -(b / (3 * a));
		// print solutions
		x[0] = (s + u) + p;
		return 1;
	}
	return 0;
}

bool MathHelper::CalculateExtendBezierToAimLength( const QuadBezierPointF2D &quadBezier, float fAimLength, float fMinS, float fMaxS, float fLLength, float fRLength, QuadBezierPointF2D quadLeft, QuadBezierPointF2D quadRight, PointF2D * pptPos, QuadBezierPointF2D * pQuadOutBezier )
{
	if (fabsf(fAimLength-fLLength) < M_FLOATEPS || fabsf(fMinS - fMaxS) < M_FLOATEXTREMEEPS)
	{
		if (pptPos)
		{
			if (fMinS < 0.0f)
			{
				*pptPos = quadLeft.ptb;
			}
			else
			{
				*pptPos = quadLeft.pte;
			}
		}
		if (pQuadOutBezier)
		{
			*pQuadOutBezier = quadLeft;
		}
		return true;
	}
	else if (fabsf(fAimLength-fRLength) < M_FLOATEPS)
	{
		if (pptPos)
		{
			if (fMinS < 0.0f)
			{
				*pptPos = quadRight.ptb;
			}
			else
			{
				*pptPos = quadRight.pte;
			}
		}
		if (pQuadOutBezier)
		{
			*pQuadOutBezier = quadRight;
		}
		return true;
	}
	
	GBaseNode tBase;
	GBezierLine * pTempLine = new GBezierLine(&tBase, PointF2D(), PointF2D());

	if (fLLength < 0)
	{
		PointF2D ptna;
		SolveBezierExtendSubDivision(quadBezier, fMinS, &ptna, &quadLeft.ptbh, &quadLeft.pteh);
		if (fMinS < 0.0f)
		{
			quadLeft.ptb = ptna;
			quadLeft.pte = quadBezier.pte;
		}
		else
		{
			quadLeft.pte = ptna;
			quadLeft.ptb = quadBezier.ptb;
		}
		pTempLine->SetBeginEnd(quadLeft.ptb.x, quadLeft.ptb.y, quadLeft.pte.x, quadLeft.pte.y);
		pTempLine->SetBeginHandlePos(quadLeft.ptbh.x, quadLeft.ptbh.y);
		pTempLine->SetEndHandlePos(quadLeft.pteh.x, quadLeft.pteh.y);
		fLLength = pTempLine->getLength();
	}
	if (fRLength < 0)
	{
		PointF2D ptna;
		SolveBezierExtendSubDivision(quadBezier, fMaxS, &ptna, &quadRight.ptbh, &quadRight.pteh);
		if (fMaxS < 0.0f)
		{
			quadRight.ptb = ptna;
			quadRight.pte = quadBezier.pte;
		}
		else
		{
			quadRight.pte = ptna;
			quadRight.ptb = quadBezier.ptb;
		}
		pTempLine->SetBeginEnd(quadRight.ptb.x, quadRight.ptb.y, quadRight.pte.x, quadRight.pte.y);
		pTempLine->SetBeginHandlePos(quadRight.ptbh.x, quadRight.ptbh.y);
		pTempLine->SetEndHandlePos(quadRight.pteh.x, quadRight.pteh.y);
		fRLength = pTempLine->getLength();
	}

	if (fAimLength < fLLength && fAimLength < fRLength || fAimLength > fLLength && fAimLength > fRLength)
	{
		if (fLLength > fRLength)
		{
			ASSERT(fMinS < 0.0f);
			fMinS*=2;
			if (CalculateExtendBezierToAimLength(
				quadBezier,
				fAimLength, fMinS, fMaxS, -1, fRLength,
				QuadBezierPointF2D(),
				quadRight,
				pptPos, pQuadOutBezier))
			{
				tBase.RemoveAllChildren(true);
				return true;
			}
		}
		else
		{
			ASSERT(fMaxS > 1.0f);
			fMaxS = (fMaxS-1.0f)*2+1.0f;
			if (CalculateExtendBezierToAimLength(
				quadBezier,
				fAimLength, fMinS, fMaxS, fLLength, -1,
				quadLeft,
				QuadBezierPointF2D(),
				pptPos, pQuadOutBezier))
			{
				tBase.RemoveAllChildren(true);
				return true;
			}

		}
	}

	QuadBezierPointF2D quadMiddle;
	float fMidS = (fMinS+fMaxS)/2;
	PointF2D ptma;
	SolveBezierExtendSubDivision(quadBezier, fMidS, &ptma, &quadMiddle.ptbh, &quadMiddle.pteh);
	if (fMidS < 0.0f)
	{
		quadMiddle.ptb = ptma;
		quadMiddle.pte = quadBezier.pte;
	}
	else
	{
		quadMiddle.pte = ptma;
		quadMiddle.ptb = quadBezier.ptb;
	}
	pTempLine->SetBeginEnd(quadMiddle.ptb.x, quadMiddle.ptb.y, quadMiddle.pte.x, quadMiddle.pte.y);
	pTempLine->SetBeginHandlePos(quadMiddle.ptbh.x, quadMiddle.ptbh.y);
	pTempLine->SetEndHandlePos(quadMiddle.pteh.x, quadMiddle.pteh.y);
	float fMidLength = pTempLine->getLength();


	if ((fAimLength < fMidLength) ^ (fLLength > fRLength))
	{
		if (CalculateExtendBezierToAimLength(
			quadBezier,
			fAimLength, fMinS, fMidS, fLLength, fMidLength,
			quadLeft,
			quadMiddle,
			pptPos, pQuadOutBezier))
		{
			tBase.RemoveAllChildren(true);
			return true;
		}
	}
	else
	{
		if (CalculateExtendBezierToAimLength(
			quadBezier,
			fAimLength, fMidS, fMaxS, fMidLength, fRLength,
			quadMiddle,
			quadRight,
			pptPos, pQuadOutBezier))
		{
			tBase.RemoveAllChildren(true);
			return true;
		}
	}
	tBase.RemoveAllChildren(true);
	return false;
}

bool MathHelper::AreTwoAngleClose( int nAngle1, int nAngle2 )
{
	RestrictAngle(&nAngle1);
	RestrictAngle(&nAngle2);
	if (nAngle1 < nAngle2)
	{
		swap(nAngle1, nAngle2);
	}
	if (nAngle1-nAngle2 < M_ANGLEEPS)
	{
		return true;
	}

	nAngle2 += ANGLEBASE_360;
	if (nAngle1 < nAngle2)
	{
		swap(nAngle1, nAngle2);
	}
	if (nAngle1-nAngle2 < M_ANGLEEPS)
	{
		return true;
	}

	return false;
}

bool MathHelper::IsPointInLeftOfLine( const PointF2D &ptA, const PointF2D &ptB, const PointF2D &ptC )
{
	return ((ptB.x-ptA.x)*(ptC.y-ptA.y)-(ptB.y-ptA.y)*(ptC.x-ptA.x)) > 0;
}

PointF2D MathHelper::GetNormal( const PointF2D &ptA, const PointF2D &ptB, float fMul/*=0.0f*/ )
{
	PointF2D ptNormal = PointF2D(-(ptB.y-ptA.y), (ptB.x-ptA.x));
	if (!fMul)
	{
		return ptNormal;
	}
	float fLength = LineSegmentLength(ptA, ptB);
	if (fLength)
	{
		ptNormal = ptNormal * (fMul/fLength);
	}
	return ptNormal;
}

/************************************************************************/
/* BezierSublinesInfo                                                   */
/************************************************************************/
BezierSublinesInfo::BezierSublinesInfo()
{
	ptPoints = NULL;
	fLengths = NULL;
    ClearSet();
}

BezierSublinesInfo::~BezierSublinesInfo()
{
	ClearSet();
}


BezierSublinesInfo& BezierSublinesInfo::operator=(const BezierSublinesInfo &that)
{
	ClearSet();

	if (that.nPoints)
	{
		ResetPoints(that.ptPoints[0], that.ptBeginHandle, that.ptEndHandle, that.ptPoints[that.nPoints-1], that.fSavedPrecision);
	}

	return *this;
}

void BezierSublinesInfo::ClearSet()
{
	nPoints = 0;
	if (ptPoints)
	{
		delete[] ptPoints;
		ptPoints = NULL;
	}
	if (fLengths)
	{
		delete[] fLengths;
		fLengths = NULL;
	}
}

int BezierSublinesInfo::ResetPoints( PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe, float fPrecision )
{
	if (nPoints > 1)
	{
		if (ptPoints[0].Equals(pb) && ptBeginHandle.Equals(pbh) && ptEndHandle.Equals(peh) && ptPoints[nPoints-1].Equals(pe) && fPrecision == fSavedPrecision)
		{
			return nPoints;
		}
	}
	ClearSet();

	fSavedPrecision = fPrecision;

	MathHelper * pmh = &MathHelper::getInstance();

	float l23 = pmh->LineSegmentLengthPow2(pbh, peh);
	float l12 = pmh->LineSegmentLengthPow2(pb, pbh);
	float l34 = pmh->LineSegmentLengthPow2(peh, pe);

	float l = l23;
	if (l12 + l34 > l23)
	{
		l = l12 + l34;
	}

	if (l < 1.0f)
	{
		l = 1.0f;
	}
	l = sqrtf(l);
	l *= fPrecision;
	int nseg = (int)(l+1.0f);

	nPoints = nseg+1;
	ptPoints = new PointF2D[nPoints];

	float s = 0;
	PointF2D pqlast(0, 0);
	PointF2D pq(0, 0);

	fBoundingLX = min(pb.x, pe.x);
	fBoundingTY = min(pb.y, pe.y);
	fBoundingRX = max(pb.x, pe.x);
	fBoundingBY = max(pb.y, pe.y);

	pmh->CalculateBezier(pb, pbh, peh, pe, s, &pq);
	ptPoints[0] = pq;
	for (int i=0; i<nseg; i++)
	{
		pqlast = pq;
		s = (float)(i+1)/(float)nseg;
		pmh->CalculateBezier(pb, pbh, peh, pe, s, &pq);
		ptPoints[i+1] = pq;

		if (pq.x < fBoundingLX)
		{
			fBoundingLX = pq.x;
		}
		else if (pq.x > fBoundingRX)
		{
			fBoundingRX = pq.x;
		}
		if (pq.y < fBoundingTY)
		{
			fBoundingTY = pq.y;
		}
		else if (pq.y > fBoundingBY)
		{
			fBoundingBY = pq.y;
		}
	}

	return nPoints;
}

int BezierSublinesInfo::CalculateLengths()
{
	if (fLengths)
	{
		delete[] fLengths;
	}

	if (!nPoints)
	{
		ASSERT(false);
		return 0;
	}

	fLengths = new float[nPoints-1];

	float fLength=0;
	MathHelper * pmh = &MathHelper::getInstance();
	
	for (int i=0; i<nPoints-1; i++)
	{
		fLength = pmh->LineSegmentLength(ptPoints[i], ptPoints[i+1]);
		fLengths[i] = fLength;
	}

	return nPoints;
}

int BezierSublinesInfo::GetSubPointsCount()
{
	return nPoints;
}

float BezierSublinesInfo::GetX( int i )
{
	if (i >= 0 && i < nPoints)
	{
		return ptPoints[i].x;
	}
	ASSERT(false);
	return 0;
}

float BezierSublinesInfo::GetY( int i )
{
	if (i >= 0 && i < nPoints)
	{
		return ptPoints[i].y;
	}
	ASSERT(false);
	return 0;
}

float BezierSublinesInfo::GetLength( int i/*=-1*/ )
{
	if (!fLengths)
	{
		CalculateLengths();
	}
	if (i >= 0 && i < nPoints-1)
	{
		return fLengths[i];
	}
	else
	{
		return GetLength(0, nPoints-2);
	}
	ASSERT(false);
	return 0;
}

float BezierSublinesInfo::GetLength( int ibegin, int iend )
{
	if (ibegin >= 0 && ibegin < nPoints-1 && iend >= 0 && iend < nPoints-1)
	{
		float fRet = 0;
		for (int i=ibegin; i<=iend; i++)
		{
			fRet += fLengths[i];
		}
		return fRet;
	}
	ASSERT(false);
	return 0;

}
const PointF2D & BezierSublinesInfo::GetPoint( int i )
{
	if (i >= 0 && i < nPoints)
	{
		return ptPoints[i];
	}
	ASSERT(false);
	return ptPoints[0];
}

bool BezierSublinesInfo::GetBoundingBox( float * lx, float * ty, float * rx, float * by )
{
	if (!ptPoints)
	{
		return false;
	}

	if (lx)
	{
		*lx = fBoundingLX;
	}
	if (ty)
	{
		*ty = fBoundingTY;
	}
	if (rx)
	{
		*rx = fBoundingRX;
	}
	if (by)
	{
		*by = fBoundingBY;
	}

	return true;
}

bool BezierSublinesInfo::isLengthCalculated()
{
	return fLengths!=NULL;
}

QuadBezierPointF2D::QuadBezierPointF2D( GBezierLine * pBezier )
{
	if (pBezier)
	{
		ptb = pBezier->GetBeginPoint()->GetPointF2D();
		ptbh = pBezier->GetBeginPoint()->GetHandle()->GetPointF2D();
		pteh = pBezier->GetEndPoint()->GetHandle()->GetPointF2D();
		pte = pBezier->GetEndPoint()->GetPointF2D();
	}
	else
	{
		DASSERT(false);
	}
}