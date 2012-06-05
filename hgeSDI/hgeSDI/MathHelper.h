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
		ASSERT(fval!=0);
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
	static MathHelper& getInstance()
	{
		static MathHelper instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	MathHelper();
	~MathHelper();
	// Don't forget to declare these two.
	MathHelper(MathHelper const&);
	// Don't Implement
	void operator=(MathHelper const&);
	// Don't implement

public:

	int AngleRestrict(int angle);

	float LineSegmentLength(PointF2D p1, PointF2D p2);
	float LineSegmentLengthPow2(PointF2D p1, PointF2D p2);

	bool LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx=0, float * inty=0);
	bool LinePartialInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb, bool nocalc=false);
	bool LineInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb);
	bool RectIntersectRect( float xl1, float yt1, float xr1, float yb1, float xl2, float yt2, float xr2, float yb2);

	bool CalculateCatmullRom(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, float s, PointF2D * pq);
	bool CalculateBezier(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, float s, PointF2D * pq);
};