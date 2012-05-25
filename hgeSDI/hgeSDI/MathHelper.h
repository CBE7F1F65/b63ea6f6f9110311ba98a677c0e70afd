#pragma once

class PointF2D
{
public:
	PointF2D()
	{
		x = y = 0.0f;
	}

	PointF2D(const PointF2D &point)
	{
		x = point.x;
		y = point.y;
	}

	PointF2D(float _x,
		float _y)
	{
		x = _x;
		y = _y;
	}

	PointF2D operator+(const PointF2D& point) const
	{
		return PointF2D(x + point.x,
			y + point.y);
	}

	PointF2D operator-(const PointF2D& point) const
	{
		return PointF2D(x - point.x,
			y - point.y);
	}

	PointF2D operator*(float fval) const
	{
		return PointF2D(x * fval,
			y * fval);
	}

	PointF2D operator/(float fval) const
	{
		assert(fval!=0);
		return PointF2D(x / fval,
			y / fval);
	}

	BOOL Equals(const PointF2D& point)
	{
		return (x == point.x) && (y == point.y);
	}

public:

	float x;
	float y;
};


class MathHelper
{
public:
	MathHelper(void);
	~MathHelper(void);

	static int AngleRestrict(int angle);

	static float LineSegmentLength(PointF2D p1, PointF2D p2);
	static float LineSegmentLengthPow2(PointF2D p1, PointF2D p2);

	static bool LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx=0, float * inty=0);
	static bool LinePartialInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb, bool nocalc=false);
	static bool LineInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb);
	static bool RectIntersectRect( float xl1, float yt1, float xr1, float yb1, float xl2, float yt2, float xr2, float yb2);

	static bool CalculateCatmullRom(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, float s, PointF2D * pq);
	static bool CalculateBezier(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, float s, PointF2D * pq);
};
