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
	bool StrictEquals(const PointF2D& point)
	{
		return x==point.x && y==point.y;
	}

	bool operator==(const PointF2D& point)
	{
		return StrictEquals(point);
	}

	float Dot(const PointF2D &point)
	{
		return x*point.x+y*point.y;
	};

	float Length2(){return x*x+y*y;};
	float Length(){return sqrtf(Length2());};

	PointF2D& Normalize()
	{
		float fLength = Length();
		if (Length())
		{
			(*this) = (*this)/Length();
		}
		else
		{
			x += 1.0f;
		}
		return *this;
	};

	int GetAngle()
	{
		return ANGLE(atan2f(y, x));
	};

public:

	float x;
	float y;
};

class GLine;
class GBezierLine;

class QuadBezierPointF2D
{
public:
	QuadBezierPointF2D(){};
	QuadBezierPointF2D(GBezierLine * pBezier);
	QuadBezierPointF2D(PointF2D &ptb, PointF2D &pte) {ptb=ptb; pte=pte;};
	QuadBezierPointF2D(PointF2D &ptb, PointF2D &ptbh, PointF2D &pteh, PointF2D &pte) {ptb=ptb; ptbh=ptbh; pteh=pteh; pte=pte;};
	virtual ~QuadBezierPointF2D(){};

public:
	PointF2D ptb;
	PointF2D ptbh;
	PointF2D pteh;
	PointF2D pte;
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
	bool CalculateBezierSubDivision(const QuadBezierPointF2D quadBezier, float s, QuadBezierPointF2D * pQuadHandles);
	bool SolveBezierExtendSubDivision(const QuadBezierPointF2D quadBezier, float s, PointF2D * pnewa, PointF2D * pnewbh, PointF2D * pneweh);
//	int SolveCubicEquationPointF2D(PointF2D ptA, PointF2D ptB, PointF2D ptC, PointF2D ptD, float s[3]);
	int SolveCubicEquation(float a, float b, float c, float d, float x[3]);

	bool PointInRect(float px, float py, float xl, float yt, float w, float h);
	bool PointInRectTwoPoint(float px, float py, float x1, float y1, float x2, float y2, float r=0);
	bool PointInCircle(float px, float py, float cx, float cy, float r);

	float NearestPointOnStraightLine(float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny);
	float NearestPointOnStraightLinePow2(float px, float py, float lx1, float ly1, float lx2, float ly2, float * nx, float * ny);
	bool PointNearToStraightLine( float px, float py, float x1, float y1, float x2, float y2, float r, float * plx, float * ply );
	void GetPerpendicularPointForLine( const PointF2D &pt1, const PointF2D &pt2, float s, float l, bool bUpward, PointF2D* ptp );
	int GetLineAngle( const PointF2D &pt1, const PointF2D &pt2 );
	void RestrictAngle( int* angle );
	bool GetLineSegmentInRect( float x, float y, int angle, float lx, float ty, float rx, float by, float* x1, float* y1, float* x2, float* y2 );
	float CalculateProportionOnStraightLine( float xb, float yb, float xe, float ye, float x, float y );
	bool FindPerpendicularPoint( float x, float y, float lx, float ly, int langle, float * px=NULL, float * py=NULL );
	int GetQuadrant( float x, float y, float xo=0, float yo=0 );

	bool FindNearestHandlePointForGivenBezierLength_TwoPoint(float fLength, const PointF2D &ptFirstAnchor, const PointF2D &ptFirstHandle, const PointF2D &ptSecondAnchor, PointF2D ptNear, PointF2D ptFar, float * px=NULL, float * py=NULL);
	bool FindNearestHandlePointForGivenBezierLength(float fLength, const PointF2D &ptFirstAnchor, const PointF2D &ptFirstHandle, const PointF2D &ptSecondAnchor, float cx, float cy, float * px=NULL, float * py=NULL, bool bCheckLengthAvailableOnly=false);
	bool FindSubNHPFGBL( GBezierLine * pBezier, float fTargetLength, PointF2D ptNear, PointF2D ptFar, float * px, float * py, float fNearLength=-1, float fFarLength=-1 );
	bool CalculateExtendBezierToAimLength( const QuadBezierPointF2D &quadBezier, float fAimLength, float fMinS, float fMaxS, float fLLength, float fRLength, QuadBezierPointF2D quadLeft, QuadBezierPointF2D quadRight, PointF2D * pptPos, QuadBezierPointF2D * pQuadOutBezier );
	bool AreTwoAngleClose( int nAngle1, int nAngle2 );

	bool IsPointInLeftOfLine(const PointF2D &ptA, const PointF2D &ptB, const PointF2D &ptC);
	PointF2D GetNormal(const PointF2D &ptA, const PointF2D &ptB, float fMul=0.0f);
};