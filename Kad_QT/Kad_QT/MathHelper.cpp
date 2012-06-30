#include "StdAfx.h"
#include "MathHelper.h"

#include "Const.h"

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

bool MathHelper::LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx/*=0*/, float * inty/*=0*/ )
{
	/*
	float s1_x, s1_y, s2_x, s2_y;
    s1_x = x2 - x1;     s1_y = y2 - y1;
    s2_x = x4 - x3;     s2_y = y4 - y3;
	*/

	float mua,mub;
	float denom,numera,numerb;
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
		if (intx)
		{
			*intx = (x1 + x2) / 2;
		}
		if (inty)
		{
			*inty = (y1 + y2) / 2;
		}

		return true;
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
		return false;
	}

	/* Is the intersection along the the segments */
	mua = numera / denom;
	mub = numerb / denom;
	if (mua < 0 || mua > 1 || mub < 0 || mub > 1) {
		if (intx)
		{
			*intx = 0;
		}
		if (inty)
		{
			*inty = 0;
		}
		return false;
	}
	if (intx)
	{
		*intx = x1 + mua * (x2 - x1);
	}
	if (inty)
	{
		*inty = y1 + mua * (y2 - y1);
	}
	return true;
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

void MathHelper::GetPerpendicularPointForLine( PointF2D pt1, PointF2D pt2, float s, float l, bool bUpward, PointF2D* ptp )
{
	int angle = GetLineAngle(pt1, pt2);
	angle += ANGLEBASE_90;
	RestrictAngle(&angle);
	if (angle >= 0)
	{
		angle += ANGLEBASE_180;
	}
	float xdiff = l*cosf(ARC(angle));
	float ydiff = l*sinf(ARC(angle));
	float xbase = (pt2.x-pt1.x)*s+pt1.x;
	float ybase = (pt2.y-pt1.y)*s+pt1.y;
	if (ptp)
	{
		ptp->x = xbase+xdiff;
		ptp->y = ybase+ydiff;
	}
}

int MathHelper::GetLineAngle( PointF2D pt1, PointF2D pt2 )
{
	/*
	if (pt1.Equals(pt2))
	{
		return 0;
	}
	*/
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
		tpy[nowindex] = y + (x-lx)/ftg;
		if (tpy[nowindex] >= ty && tpy[nowindex] <= by)
		{
			tpx[nowindex] = lx;
			nowindex++;
		}
		// Check Top
		tpx[nowindex] = x + (y-ty)*ftg;
		if (tpx[nowindex] >= lx && tpx[nowindex] <= rx)
		{
			tpy[nowindex] = ty;
			nowindex++;
		}
		// Check Right
		if (nowindex < 2)
		{
			tpy[nowindex] = y + (x-rx)/ftg;
			if (tpy[nowindex] >= ty && tpy[nowindex] <= by)
			{
				tpx[nowindex] = rx;
				nowindex++;
			}
		}
		// Check Bottom
		if (nowindex < 2)
		{
			tpx[nowindex] = x + (y-by)*ftg;
			if (tpx[nowindex] >= lx && tpx[nowindex] <= rx)
			{
				tpy[nowindex] = by;
				nowindex++;
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
		ASSERT(true);
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
	ASSERT(true);
	return 0;
}

float BezierSublinesInfo::GetY( int i )
{
	if (i >= 0 && i < nPoints)
	{
		return ptPoints[i].y;
	}
	ASSERT(true);
	return 0;
}

float BezierSublinesInfo::GetLength( int i )
{
	if (!fLengths)
	{
		CalculateLengths();
	}
	if (i >= 0 && i < nPoints-1)
	{
		return fLengths[i];
	}
	ASSERT(true);
	return 0;
}

const PointF2D & BezierSublinesInfo::GetPoint( int i )
{
	if (i >= 0 && i < nPoints)
	{
		return ptPoints[i];
	}
	ASSERT(true);
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
