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

	int angle = pPIP->GetAngle();
	float fcos = cost(angle) * snaprange_c;
	float fsin = sint(angle) * snaprange_c;
	float xbase = pPIP->GetX();
	float ybase = pPIP->GetY();

	pFakeLine[0]->SetBeginEnd(xbase-fcos, ybase-fsin, xbase+fcos, ybase+fsin);
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

	float arc = ARC(pPIP->GetAngle());
	float fcos = cosf(arc) * snaprange_c;
	float fsin = sinf(arc) * snaprange_c;
	float xbase = pPIP->GetX();
	float ybase = pPIP->GetY();

	pFakeLine[0]->SetBeginEnd(xbase-fcos, ybase-fsin, xbase+fcos, ybase+fsin);
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

	int angle = pPIP->GetAngle();
	float fcos = cost(angle) * snaprange_c;
	float fsin = sint(angle) * snaprange_c;
	float xbase = pPIP->GetX();
	float ybase = pPIP->GetY();

	pFakeLine[iIndex]->SetBeginEnd(xbase-fcos, ybase-fsin, xbase+fcos, ybase+fsin);

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

	int angle1 = pPIP1->GetAngle();
	float fcos1 = cost(angle1) * snaprange_c;
	float fsin1 = sint(angle1) * snaprange_c;
	float xbase1 = pPIP1->GetX();
	float ybase1 = pPIP1->GetY();

	pFakeLine[0]->SetBeginEnd(xbase1-fcos1, ybase1-fsin1, xbase1+fcos1, ybase1+fsin1);

	int angle2 = pPIP2->GetAngle();
	float fcos2 = cost(angle2) * snaprange_c;
	float fsin2 = sint(angle2) * snaprange_c;
	float xbase2 = pPIP2->GetX();
	float ybase2 = pPIP2->GetY();

	pFakeLine[1]->SetBeginEnd(xbase2-fcos2, ybase2-fsin2, xbase2+fcos2, ybase2+fsin2);

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
	return false;
}

bool GObjectPicker::SubFindAnglesLockX( float y )
{
	return false;
}

bool GObjectPicker::SubFindLengthLockY( float x )
{
	return false;
}

bool GObjectPicker::SubFindAnglesLockY( float x )
{
	return false;
}

bool GObjectPicker::SubFindLineLengthLock( GLine * pLine, int iIndex )
{
	return false;
}

bool GObjectPicker::SubFindLineAnglesLock( GLine * pLine, int iIndex )
{
	return false;
}

bool GObjectPicker::SubFindPIPLengthLock( PickerInterestPointInfo * pPIP )
{
	return false;
}

bool GObjectPicker::SubFindPIPAnglesLock( PickerInterestPointInfo * pPIP )
{
	return false;
}
