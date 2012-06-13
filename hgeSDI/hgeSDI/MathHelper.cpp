#include "StdAfx.h"
#include "MathHelper.h"

#include "Const.h"

MathHelper::MathHelper(void)
{
}

MathHelper::~MathHelper(void)
{
}

int MathHelper::AngleRestrict( int angle )
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
	float mua,mub;
	float denom,numera,numerb;

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

bool MathHelper::CalculateBezier( const PointF2D &p1, const PointF2D &p2, const PointF2D &p3, const PointF2D &p4, float s, PointF2D * pq )
{
	if (pq)
	{
		*pq = p1*powf(1-s, 3) + p2*(3*s*(powf(1-s, 2))) + p3*(3*powf(s, 2)*(1-s)) + p4*powf(s,3);
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

float MathHelper::NearestPointOnLine( float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny )
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
	return LineSegmentLength(proj, p);

}