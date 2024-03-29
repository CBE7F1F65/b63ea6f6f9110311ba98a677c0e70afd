#include "stdafx.h"
#include "GObjectPicker.h"
#include "GUICoordinate.h"

bool GObjectPicker::CheckSnapGeometryPoint( GObject * pObj )
{
	if (!snappedstate)
	{
		if (pObj->isRepresentablePoint() && pObj->isDisplayVisible())
		{
			float objx = pObj->getX();
			float objy = pObj->getY();
			if (IsInSnapRangePoint_C(objx, objy))
			{
				bool bret = true;
				if (pfilterfunc)
				{
					bret = pfilterfunc(pObj);
				}
				if (isSnapToHandleOnly())
				{
					if (!pObj->isHandlePoint())
					{
						bret = false;
					}
				}
				if (bret)
				{
					snappedstate |= GOPSNAPPED_POINT|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
					SetPickObj(pObj);
					pickx_c = objx;
					picky_c = objy;
					nOnLine = GOPONLINE_MAX;
					return true;
				}
			}
		}
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			if (CheckSnapGeometryPoint(*it))
			{
				return true;
			}
		}
	}
	return false;
}

bool GObjectPicker::CheckSnapPoint(float x, float y, int tostate)
{
	if (!snappedstate)
	{
		if (IsInSnapRangePoint_C(x, y))
		{
			snappedstate |= tostate | GOPSNAPPED_VIRTUALPOINT;
			pickx_c = x;
			picky_c = y;
			nOnLine = GOPONLINE_MAX;
			return true;
		}
	}
	return false;
}

bool GObjectPicker::CheckSnapPointCoord(float x, float y, int tostate)
{
	if (nOnLine < GOPONLINE_MAX)
	{
		if (!(snappedstate&GOPSNAPPED_XAXIS))
		{
			if (IsInSnapRangeXAxis_C(y))
			{
				if (nOnLine)
				{
					if (FindLineIntersectX(y))
					{
						snappedstate |= tostate | GOPSNAPPED_XAXIS;
						nOnLine++;
						return true;
					}
				}
				else
				{
					snappedstate |= tostate | GOPSNAPPED_XAXIS;
					picky_c = y;
					nOnLine++;
				}
			}
		}
		if (!(snappedstate&GOPSNAPPED_YAXIS))
		{
			if (IsInSnapRangeYAxis_C(x))
			{
				if (nOnLine)
				{
					if (FindLineIntersectY(x))
					{
						snappedstate |= tostate | GOPSNAPPED_YAXIS;
						nOnLine++;
						return true;
					}
				}
				else
				{
					snappedstate |= tostate | GOPSNAPPED_YAXIS;
					pickx_c = x;
					nOnLine++;
				}
			}
		}
	}
	return false;
}

bool GObjectPicker::CheckCoord_Obj( GObject * pObj )
{
	if (nOnLine >= GOPONLINE_MAX)
	{
		return false;
	}
	if (!(snappedstate&GOPSNAPPED_XAXIS) || !(snappedstate&GOPSNAPPED_YAXIS))
	{
		if (pObj->isRepresentablePoint() && pObj->isDisplayVisible())
		{
			if ((pObj->isSlaveToLine() || pObj->isSlaveToPiece() || pObj->isHandlePoint()) && !isSnapToVirtualCoord())
			{
				return false;
			}
			float objx = pObj->getX();
			float objy = pObj->getY();
			if (!(snappedstate & GOPSNAPPED_XAXIS) && IsInSnapRangeXAxis_C(objy))
			{
				bool bret = true;
				if (pfilterfunc)
				{
					bret = pfilterfunc(pObj);
				}
				if (bret)
				{
					if (nOnLine)
					{
						if (FindLineIntersectX(objy))
						{
							snappedstate |= GOPSNAPPED_XAXIS|GOPSNAP_COORD;
							SetPickObjCoord(pObj);
							nOnLine++;
							return true;
						}
					}
					else
					{
						snappedstate |= GOPSNAPPED_XAXIS|GOPSNAP_COORD;
						picky_c = objy;
						SetPickObjCoord(pObj);
						nOnLine++;
					}
				}
			}
			if (!(snappedstate & GOPSNAPPED_YAXIS) && IsInSnapRangeYAxis_C(objx))
			{
				bool bret = true;
				if (pfilterfunc)
				{
					bret = pfilterfunc(pObj);
				}
				if (bret)
				{
					if (nOnLine)
					{
						if (FindLineIntersectY(objx))
						{
							snappedstate |= GOPSNAPPED_YAXIS|GOPSNAP_COORD;
							SetPickObjCoord(pObj);
							nOnLine++;
							return true;
						}
					}
					else
					{
						snappedstate |= GOPSNAPPED_YAXIS|GOPSNAP_COORD;
						pickx_c = objx;
						SetPickObjCoord(pObj);
						nOnLine++;
					}
				}
			}
		}
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			if (CheckCoord_Obj(*it))
			{
				return true;
			}
		}
	}
	return false;
}

bool GObjectPicker::CheckSnapGeometryLine( GObject * pObj )
{
	if (nOnLine < GOPONLINE_MAX)
	{
		if (pObj->isRepresentableLine() && pObj != pLockedLine && pObj->isDisplayVisible())
		{
			float neartox;
			float neartoy;
			if (((GLine *)pObj)->CheckNearTo(pickx_c, picky_c, snaprange_c, &neartox, &neartoy, &pickSection[nOnLine]))
			{
				bool bret = true;
				if (pfilterfunc)
				{
					bret = pfilterfunc(pObj);
				}
				if (bret)
				{
					if (nOnLine)
					{
						if (FindLineIntersectLine((GLine *)pObj))
						{
							snappedstate |= GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
							SetPickObj(pObj);
							nOnLine++;
						}
					}
					else
					{
						snappedstate |= GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
						SetPickObj(pObj);
						pickx_c = neartox;
						picky_c = neartoy;
						nOnLine++;
						AddSplitUI(pObj);
						CheckSnapGeometryLine_ClingBy((GLine *)pObj);
					}
				}
			}
		}
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			if (CheckSnapGeometryLine(*it))
			{
				return true;
			}
		}
	}
	return false;

}

bool GObjectPicker::CheckSnapGeometryLine_ClingBy( GLine * pLine )
{
	if (nOnLine == 1)
	{
		list<GPoint *> * plstClingBy = pLine->getClingBy();
		if (!plstClingBy->empty())
		{
			for (list<GPoint *>::iterator it=plstClingBy->begin(); it!=plstClingBy->end(); ++it)
			{
				GPoint * pPoint = *it;
				float objx = pPoint->getX();
				float objy = pPoint->getY();
				if (IsInSnapRangePoint_C(objx, objy))
				{
					bool bret = true;
					if (pfilterfunc)
					{
						bret = pfilterfunc(pPoint);
					}
					if (bret)
					{
						snappedstate |= GOPSNAPPED_POINT|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
						SetPickObj(pPoint);
						PointF2D ptPick;
						pPoint->getClingInfo()->GetClingPosition(&ptPick, &pickSection[0]);
						pickx_c = ptPick.x;
						picky_c = ptPick.y;
						nOnLine = GOPONLINE_MAX;
						return true;
					}
				}

			}
		}
	}
	return false;
}

bool GObjectPicker::CheckSnapGrid()
{
	if (!snappedstate)
	{
		GUICoordinate * pguic = &GUICoordinate::getInstance();

		float subspace = pguic->GetSubgirdSpace_C();
		int ixindex = (int)(pickx_c/subspace+0.5f);
		int iyindex = (int)(picky_c/subspace+0.5f);
		float fnx = ixindex*subspace;
		float fny = iyindex*subspace;
		if (IsInSnapRangePoint_C(fnx, fny))
		{
			snappedstate |= GOPSNAPPED_POINT|GOPSNAP_GRID;
			pickx_c = fnx;
			picky_c = fny;
			nOnLine = GOPONLINE_MAX;
			return true;
		}
	}
	return false;
}

bool GObjectPicker::CheckSnapContinuity()
{
	if (nOnLine < GOPONLINE_MAX)
	{
		if (!pipinfo.empty())
		{
			for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
			{
				if (it->HasAngle())
				{
					float lx, ly;
					bool bPicked = false;
					bool bPerp = false;

					float itx = it->GetX();
					float ity = it->GetY();
					float itangle = it->GetAngle();

					if (IsInSnapRangeAngle_C(itx, ity, itangle, &lx, &ly))
					{
						bPicked = true;
					}
					else if (IsInSnapRangeAngle_C(itx, ity, itangle+ANGLEBASE_90, &lx, &ly))
					{
						bPicked = true;
						bPerp = true;
					}
					if (bPicked)
					{
						if (nOnLine)
						{
							if (FindLineIntersectPIP(&(*it)))
							{
								snappedstate |= GOPSNAPPED_CONTINUITY;
								SetPickPIP((*it), bPerp);
								nOnLine++;
							}
						}
						snappedstate |= GOPSNAPPED_CONTINUITY;
						SetPickPIP((*it), bPerp);
						pickx_c = lx;
						picky_c = ly;
						nOnLine++;
						return true;
					}
				}
			}
		}
	}
	return false;
}