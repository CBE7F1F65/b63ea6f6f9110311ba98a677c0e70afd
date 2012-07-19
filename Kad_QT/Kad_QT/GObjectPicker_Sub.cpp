#include "stdafx.h"
#include "GObjectPicker.h"


bool GObjectPicker::FindLineIntersectX( float y )
{
	// To set pickx_c and picky_c
	if (snappedstate & GOPSNAPPED_LENGTHLOCK)
	{
		return SubFindLengthLockX(y);
	}
	if (snappedstate & GOPSNAPPED_ANGLESLOCK)
	{
		return SubFindAnglesLockX(y);
	}
	if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[0], 0, pickSection[0]);
		return SubFindLineX(pFakeLine[0], y, 1);
	}
	if (snappedstate & GOPSNAPPED_YAXIS)
	{
		picky_c = y;
		return true;
	}
	if (snappedstate & GOPSNAPPED_CONTINUITY)
	{
		return SubFindPIPX(&(pickPIP[0]), y);
	}
	return false;
}

bool GObjectPicker::FindLineIntersectY( float x )
{
	// To set pickx_c and picky_c
	if (snappedstate & GOPSNAPPED_LENGTHLOCK)
	{
		return SubFindLengthLockY(x);
	}
	if (snappedstate & GOPSNAPPED_ANGLESLOCK)
	{
		return SubFindAnglesLockY(x);
	}
	if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[0], 0, pickSection[0]);
		return SubFindLineY(pFakeLine[0], x, 1);
	}
	if (snappedstate & GOPSNAPPED_XAXIS)
	{
		pickx_c = x;
		return true;
	}
	if (snappedstate & GOPSNAPPED_CONTINUITY)
	{
		return SubFindPIPY(&(pickPIP[0]), x);
	}
	return false;
}

bool GObjectPicker::FindLineIntersectLine( GLine * pLine )
{
	// To set pickx_c and picky_c
	int nObjIndex = nOnLine;
	int nAnotherIndex = GOPONLINE_MAX-1-nOnLine;
	TraslateLineToStraightLine(pLine, nOnLine, pickSection[nOnLine]);

	if (snappedstate & GOPSNAPPED_LENGTHLOCK)
	{
		return SubFindLineLengthLock(pFakeLine[nOnLine], nAnotherIndex);
	}
	if (snappedstate & GOPSNAPPED_ANGLESLOCK)
	{
		return SubFindLineAnglesLock(pFakeLine[nOnLine], nAnotherIndex);
	}
	if (snappedstate & GOPSNAPPED_XAXIS)
	{
		return SubFindLineX(pFakeLine[nOnLine], picky_c, nAnotherIndex);
	}
	else if (snappedstate & GOPSNAPPED_YAXIS)
	{
		return SubFindLineY(pFakeLine[nOnLine], pickx_c, nAnotherIndex);
	}
	else if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[nAnotherIndex], nAnotherIndex, pickSection[nAnotherIndex]);
		return SubFindLineLine(pFakeLine[nAnotherIndex], pFakeLine[nOnLine]);
	}
	else if (snappedstate & GOPSNAPPED_CONTINUITY)
	{
		return SubFindLinePIP(pFakeLine[nOnLine], &(pickPIP[0]), nAnotherIndex);
	}
	return false;
}

bool GObjectPicker::FindLineIntersectPIP( PickerInterestPointInfo * pPIP )
{
	// To set pickx_c and picky_c
	if (snappedstate & GOPSNAPPED_LENGTHLOCK)
	{
		return SubFindPIPLengthLock(pPIP);
	}
	if (snappedstate & GOPSNAPPED_ANGLESLOCK)
	{
		return SubFindPIPAnglesLock(pPIP);
	}
	if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[0], 0, pickSection[0]);
		return SubFindLinePIP(pFakeLine[0], pPIP, 1);
	}
	else if (snappedstate & GOPSNAPPED_XAXIS)
	{
		return SubFindPIPX(pPIP, picky_c);
	}
	else if (snappedstate & GOPSNAPPED_YAXIS)
	{
		return SubFindPIPY(pPIP, pickx_c);
	}
	else if (snappedstate & GOPSNAPPED_CONTINUITY)
	{
		return SubFindPIPPIP(pPIP, &(pickPIP[0]));
	}
	return false;
}

bool GObjectPicker::SubFindLineX( GLine * pLine, float y, int iIndex )
{
	list<PointF2D> pts;
	pFakeLine[iIndex]->SetBeginEnd(pickx_c-snaprange_c, y, pickx_c+snaprange_c, y);
	if (pLine->CheckIntersectWithLineObj(pFakeLine[iIndex], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLineY( GLine * pLine, float x, int iIndex )
{
	list<PointF2D> pts;
	pFakeLine[iIndex]->SetBeginEnd(x, picky_c-snaprange_c, x, picky_c+snaprange_c);
	if (pLine->CheckIntersectWithLineObj(pFakeLine[iIndex], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindPIPX( PickerInterestPointInfo * pPIP, float y )
{
	list<PointF2D> pts;

	TraslatePIPToStraightLine(pPIP, 0);
	pFakeLine[1]->SetBeginEnd(pickx_c-snaprange_c, y, pickx_c+snaprange_c, y);
	if (pFakeLine[1]->CheckIntersectWithLineObj(pFakeLine[0], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindPIPY( PickerInterestPointInfo * pPIP, float x )
{
	list<PointF2D> pts;

	TraslatePIPToStraightLine(pPIP, 0);
	pFakeLine[1]->SetBeginEnd(x, picky_c-snaprange_c, x, picky_c+snaprange_c);
	if (pFakeLine[1]->CheckIntersectWithLineObj(pFakeLine[0], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLinePIP( GLine * pLine, PickerInterestPointInfo * pPIP, int iIndex )
{
	list<PointF2D> pts;

	TraslatePIPToStraightLine(pPIP, iIndex);

	if (pLine->CheckIntersectWithLineObj(pFakeLine[iIndex], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLineLine( GLine * pLine1, GLine * pLine2 )
{
	list<PointF2D> pts;

	if (pLine1->CheckIntersectWithLineObj(pLine2, &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindPIPPIP( PickerInterestPointInfo * pPIP1, PickerInterestPointInfo * pPIP2 )
{
	list<PointF2D> pts;

	TraslatePIPToStraightLine(pPIP1, 0);
	TraslatePIPToStraightLine(pPIP2, 1);

	if (pFakeLine[1]->CheckIntersectWithLineObj(pFakeLine[0], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}

	return false;
}

bool GObjectPicker::SubFindLengthLockX( float y )
{
	if (fabsf(y-fLockOriginY_c) > fLockLength)
	{
		return false;
	}
	if (pLockLengthHandle)
	{
		float tx, ty;
		bool bret = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength_TwoPoint(
			fLockLength, pLockLengthAnotherHandle->GetAnchor()->GetPointF2D(), pLockLengthAnotherHandle->GetPointF2D(),
			pLockLengthHandle->GetPointF2D(),
			PointF2D(pickx_c, y-snaprange_c), PointF2D(pickx_c, y+snaprange_c),
			&tx, &ty);

		if (bret)
		{
			pickx_c = tx;
			picky_c = ty;
			return true;
		}
		return false;
	}
	PointF2D ptIntersection[2];
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	PointF2D ptNow(pickx_c, picky_c);
	MathHelper * pmh = &MathHelper::getInstance();
	bool bIntersect = pmh->LineIntersectCircle(PointF2D(0, y), PointF2D(1, 0), ptC, fLockLength, ptIntersection);
	if (bIntersect)
	{
		if (pmh->LineSegmentLengthPow2(ptNow, ptIntersection[0]) <= pmh->LineSegmentLengthPow2(ptNow, ptIntersection[1]))
		{
			pickx_c = ptIntersection[0].x;
		}
		else
		{
			pickx_c = ptIntersection[1].x;
		}
		picky_c = y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindAnglesLockX( float y )
{
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptIntersection;
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	bool bIntersect = pmh->LineIntersectLine(PointF2D(0, y), PointF2D(1, 0), ptC, ptCurrentLockAngleDir, &ptIntersection);
	if (bIntersect)
	{
		pickx_c = ptIntersection.x;
		picky_c = y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLengthLockY( float x )
{
	if (fabsf(x-fLockOriginX_c) > fLockLength)
	{
		return false;
	}
	if (pLockLengthHandle)
	{
		float tx, ty;
		bool bret = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength_TwoPoint(
			fLockLength, pLockLengthAnotherHandle->GetAnchor()->GetPointF2D(), pLockLengthAnotherHandle->GetPointF2D(),
			pLockLengthHandle->GetPointF2D(),
			PointF2D(x-snaprange_c, picky_c), PointF2D(x+snaprange_c, picky_c),
			&tx, &ty);

		if (bret)
		{
			pickx_c = tx;
			picky_c = ty;
			return true;
		}
		return false;
	}
	PointF2D ptIntersection[2];
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	PointF2D ptNow(pickx_c, picky_c);
	MathHelper * pmh = &MathHelper::getInstance();
	bool bIntersect = pmh->LineIntersectCircle(PointF2D(x, 0), PointF2D(0, 1), ptC, fLockLength, ptIntersection);
	if (bIntersect)
	{
		if (pmh->LineSegmentLengthPow2(ptNow, ptIntersection[0]) <= pmh->LineSegmentLengthPow2(ptNow, ptIntersection[1]))
		{
			picky_c = ptIntersection[0].y;
		}
		else
		{
			picky_c = ptIntersection[1].y;
		}
		pickx_c = x;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindAnglesLockY( float x )
{
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptIntersection;
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	bool bIntersect = pmh->LineIntersectLine(PointF2D(x, 0), PointF2D(0, 1), ptC, ptCurrentLockAngleDir, &ptIntersection);
	if (bIntersect)
	{
		pickx_c = x;
		picky_c = ptIntersection.y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLineLengthLock( GLine * pLine, int iIndex )
{
	if (pLockLengthHandle)
	{
		return false;
	}

	if (pLockLengthHandle)
	{
		float tx, ty;
		bool bret = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength_TwoPoint(
			fLockLength, pLockLengthAnotherHandle->GetAnchor()->GetPointF2D(), pLockLengthAnotherHandle->GetPointF2D(),
			pLockLengthHandle->GetPointF2D(),
			pLine->GetBeginPoint()->GetPointF2D(), pLine->GetEndPoint()->GetPointF2D(),		// Can be wrong
			&tx, &ty);

		if (bret)
		{
			pickx_c = tx;
			picky_c = ty;
			return true;
		}
		return false;
	}

	list<PointF2D>pts;
	MathHelper * pmh = &MathHelper::getInstance();
	int quadrant = pmh->GetQuadrant(pickx_c, picky_c, fLockOriginX_c, fLockOriginY_c);
	pFakeLine[iIndex]->SetPosByQuarterCircle(fLockOriginX_c, fLockOriginY_c, fLockLength, quadrant);

	if (pLine->CheckIntersectWithLineObj(pFakeLine[iIndex], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindLineAnglesLock( GLine * pLine, int iIndex )
{
	if (pLockLengthHandle)
	{
		return false;
	}
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptIntersection;
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	float s;
	bool bIntersect = pmh->LineIntersectLine(pLine->GetBeginPoint()->GetPointF2D(), pLine->GetTangentPointF2D(0), ptC, ptCurrentLockAngleDir, &ptIntersection, &s);
	if (bIntersect && s >= 0.0f && s <= 1.0f)
	{
		pickx_c = ptIntersection.x;
		picky_c = ptIntersection.y;
		return true;
	}
	
	return false;
}

bool GObjectPicker::SubFindPIPLengthLock( PickerInterestPointInfo * pPIP )
{
	TraslatePIPToStraightLine(pPIP, 0);

	list<PointF2D>pts;
	MathHelper * pmh = &MathHelper::getInstance();

	if (pLockLengthHandle)
	{
		float tx, ty;
		bool bret = pmh->FindNearestHandlePointForGivenBezierLength_TwoPoint(
			fLockLength, pLockLengthAnotherHandle->GetAnchor()->GetPointF2D(), pLockLengthAnotherHandle->GetPointF2D(),
			pLockLengthHandle->GetPointF2D(),
			pFakeLine[0]->GetBeginPoint()->GetPointF2D(), pFakeLine[0]->GetEndPoint()->GetPointF2D(),
			&tx, &ty);
		if (bret)
		{
			pickx_c = tx;
			picky_c = ty;
			return true;
		}
		return false;
	}

	int quadrant = pmh->GetQuadrant(pickx_c, picky_c, fLockOriginX_c, fLockOriginY_c);
	pFakeLine[1]->SetPosByQuarterCircle(fLockOriginX_c, fLockOriginY_c, fLockLength, quadrant);

	if (pFakeLine[0]->CheckIntersectWithLineObj(pFakeLine[1], &pts))
	{
		pickx_c = pts.front().x;
		picky_c = pts.front().y;
		return true;
	}
	return false;
}

bool GObjectPicker::SubFindPIPAnglesLock( PickerInterestPointInfo * pPIP )
{
	TraslatePIPToStraightLine(pPIP, 0);

	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptIntersection;
	PointF2D ptC(fLockOriginX_c, fLockOriginY_c);
	float s;
	bool bIntersect = pmh->LineIntersectLine(pFakeLine[0]->GetBeginPoint()->GetPointF2D(), pFakeLine[0]->GetTangentPointF2D(0), ptC, ptCurrentLockAngleDir, &ptIntersection, &s);
	if (bIntersect && s >= 0.0f && s <= 1.0f)
	{
		pickx_c = ptIntersection.x;
		picky_c = ptIntersection.y;
		return true;
	}
	return false;
}
