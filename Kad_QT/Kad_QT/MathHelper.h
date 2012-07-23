#pragma once

class PointF2D
{
public:
	PointF2D() { x = y = 0.0f; };
	PointF2D(const PointF2D &point) { x = point.x; y = point.y; };
	PointF2D(float _x, float _y) { x = _x; y = _y; };
	PointF2D(int angle){ x = cost(angle); y = sint(angle); };
	virtual ~PointF2D(){};

	PointF2D operator+(const PointF2D& point) const { return PointF2D(x + point.x, y + point.y); };
	PointF2D operator-(const PointF2D& point) const { return PointF2D(x - point.x, y - point.y); };
	PointF2D operator*(float fval) const { return PointF2D(x * fval, y * fval); };
	PointF2D operator/(float fval) const { ASSERT(fval!=0); return PointF2D(x / fval, y / fval); };

	bool Equals(const PointF2D& point)
	{
		return (fabsf(x-point.x) < M_FLOATEPS) && (fabsf(y-point.y) < M_FLOATEPS);
	}

	float Dot(const PointF2D &point)
	{
		return x*point.x+y*point.y;
	};

public:

	float x;
	float y;
};


class BezierSublinesInfo
{
public:
	BezierSublinesInfo();
	~BezierSublinesInfo();

private:
	BezierSublinesInfo(const BezierSublinesInfo& that);
public:
	BezierSublinesInfo& operator=(const BezierSublinesInfo& that);

	void ClearSet();
	int ResetPoints(PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe, float fPrecision);

	bool isLengthCalculated();
	int CalculateLengths();
	
	int GetSubPointsCount();
	float GetX(int i);
	float GetY(int i);
	const PointF2D &GetPoint(int i);
	float GetLength(int i=-1);
	float GetLength(int ibegin, int iend);

	bool GetBoundingBox(float * lx, float * ty, float * rx, float * by);


private:
	int nPoints;
	PointF2D * ptPoints;
	float * fLengths;

	PointF2D ptBeginHandle;
	PointF2D ptEndHandle;
	float fSavedPrecision;

	float fBoundingLX;
	float fBoundingTY;
	float fBoundingRX;
	float fBoundingBY;
};

class GLine;
class GBezierLine;

enum{
	MHLINEINTERSECT_NOINTERSECT,
	MHLINEINTERSECT_INTERSECT,
	MHLINEINTERSECT_COINCIDENT,
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

	int RestrictAngle(int angle);

	float LineSegmentLength(const PointF2D &p1, const PointF2D &p2);
	float LineSegmentLengthPow2(const PointF2D &p1, const PointF2D &p2);

	int LineSegmentIntersect( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float * intx=0, float * inty=0);
	bool LinePartialInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb, bool nocalc=false);
	bool LineInRect( float x1, float y1, float x2, float y2, float xl, float yt, float xr, float yb);
	bool RectIntersectRect( float xl1, float yt1, float xr1, float yb1, float xl2, float yt2, float xr2, float yb2);

	bool LineIntersectCircle(PointF2D lO, PointF2D lD, PointF2D C, float r, PointF2D * ptIntersections=NULL, PointF2D * ptNormals=NULL);
	bool LineIntersectLine(PointF2D p, PointF2D u, PointF2D q, PointF2D v, PointF2D * ptIntersection=NULL, float * pfs=NULL);

	bool CalculateCatmullRom(const PointF2D &p1, const PointF2D &p2, const PointF2D &p3, const PointF2D &p4, float s, PointF2D * pq);
	bool CalculateBezier(const PointF2D &pb, const PointF2D &pbh, const PointF2D &peh, const PointF2D &pe, float s, PointF2D * pq);
	bool CalculateBezierSubDivision(const PointF2D &pb, const PointF2D &pbh, const PointF2D &peh, const PointF2D &pe, float s, PointF2D * p1bh, PointF2D * p1eh, PointF2D * p2eh, PointF2D * p2bh);

	bool PointInRect(float px, float py, float xl, float yt, float w, float h);
	bool PointInRectTwoPoint(float px, float py, float x1, float y1, float x2, float y2, float r=0);
	bool PointInCircle(float px, float py, float cx, float cy, float r);

	float NearestPointOnStraightLine(float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny);
	float NearestPointOnStraightLinePow2(float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny);
	bool PointNearToStraightLine( float px, float py, float x1, float y1, float x2, float y2, float r, float * plx, float * ply );
	void GetPerpendicularPointForLine( PointF2D pt1, PointF2D pt2, float s, float l, bool bUpward, PointF2D* ptp );
	int GetLineAngle( PointF2D pt1, PointF2D pt2 );
	void RestrictAngle( int* angle );
	bool GetLineSegmentInRect( float x, float y, int angle, float lx, float ty, float rx, float by, float* x1, float* y1, float* x2, float* y2 );
	float CalculateProportionOnStraightLine( float xb, float yb, float xe, float ye, float x, float y );
	bool FindPerpendicularPoint( float x, float y, float lx, float ly, int langle, float * px=NULL, float * py=NULL );
	int GetQuadrant( float x, float y, float xo=0, float yo=0 );

	bool FindNearestHandlePointForGivenBezierLength_TwoPoint(float fLength, PointF2D ptFirstAnchor, PointF2D ptFirstHandle, PointF2D ptSecondAnchor, PointF2D ptNear, PointF2D ptFar, float * px=NULL, float * py=NULL);
	bool FindNearestHandlePointForGivenBezierLength(float fLength, PointF2D ptFirstAnchor, PointF2D ptFirstHandle, PointF2D ptSecondAnchor, float cx, float cy, float * px=NULL, float * py=NULL, bool bCheckLengthAvailableOnly=false);
	bool FindSubNHPFGBL( GBezierLine * pBezier, float fTargetLength, PointF2D ptNear, PointF2D ptFar, float * px, float * py, float fNearLength=-1, float fFarLength=-1 );
};