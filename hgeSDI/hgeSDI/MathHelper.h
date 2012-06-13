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

	bool Equals(const PointF2D& point)
	{
		return (x == point.x) && (y == point.y);
	}

	float Dot(const PointF2D &point)
	{
		return x*point.x+y*point.y;
	};

public:

	float x;
	float y;
};
class MathHelper
{
public:
	static MathHelper& getInstance() { static MathHelper instance; return instance; }

private:
	MathHelper();
	~MathHelper();
	MathHelper(MathHelper const&);
	void operator=(MathHelper const&);

public:

	int AngleRestrict(int angle);

	float LineSegmentLength(const PointF2D &p1, const PointF2D &p2);
	float LineSegmentLengthPow2(const PointF2D &p1, const PointF2D &p2);

	bool LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx=0, float * inty=0);
	bool LinePartialInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb, bool nocalc=false);
	bool LineInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb);
	bool RectIntersectRect( float xl1, float yt1, float xr1, float yb1, float xl2, float yt2, float xr2, float yb2);

	bool CalculateCatmullRom(const PointF2D &p1, const PointF2D &p2, const PointF2D &p3, const PointF2D &p4, float s, PointF2D * pq);
	bool CalculateBezier(const PointF2D &p1, const PointF2D &p2, const PointF2D &p3, const PointF2D &p4, float s, PointF2D * pq);

	bool PointInRect(float px, float py, float xl, float yt, float w, float h);
	bool PointInCircle(float px, float py, float cx, float cy, float r);

	float NearestPointOnLine(float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny);
};