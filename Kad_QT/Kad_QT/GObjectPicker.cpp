#include "StdAfx.h"
#include "GObjectPicker.h"
#include "MainInterface.h"
#include "MathHelper.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "GObjectManager.h"
#include "ColorManager.h"
#include "GLine.h"
#include "MarqueeSelect.h"


GObjectPicker::GObjectPicker(void)
{
	state = PICKSTATE_NULL;
	pfilterfunc = NULL;
	SetSnapTo(GOPSNAP_GEOMETRY|GOPSNAP_COORD|GOPSNAP_GEOMETRYCOORD|GOPSNAP_CONTINUITY);
	bCheckMouseDown = false;
	bRenderMouseDown = false;
	bFilledPos =false;
	bTestMode = false;
	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pFakeLine[i] = new GStraightLine(&(GObjectManager::getInstance().fakebasenode), PointF2D(0, 0), PointF2D(0, 0));
	}
}

GObjectPicker::~GObjectPicker(void)
{
}

void GObjectPicker::Init()
{
	state = PICKSTATE_NULL;
	SetSnapRange_S(25.0f);
}

int GObjectPicker::TestPickPoint( float x, float y, float *pfProportion/*=0*/, PickFilterCallback pfunc/*=NULL*/, float range_s/*=0*/ )
{
	state = PICKSTATE_NULL;

	PickPoint(pfunc);
	bFilledPos = true;
	bTestMode = true;
	filledX_C = x;
	filledY_C = y;

	this->snaptoflag = getInstance().snaptoflag;
	if (range_s >= 0)
	{
		SetSnapRange_S(range_s);
	}
	else
	{
		SetSnapRange_S(getInstance().snaprange_s);
	}

	ClearSet();
	UpdatePickPoint();
	int iret = PickPoint(pfunc);
	if (pfProportion)
	{
		*pfProportion = 0;
		if (IsPickReady(iret))
		{
			*pfProportion = CalculateProportion();
		}
	}
	return iret;
}


void GObjectPicker::FillInitialPosition()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();

	if (bFilledPos)
	{
		pickx_c = filledX_C;
		picky_c = filledY_C;
		pickx_s = pguic->CtoSx(pickx_c);
		picky_s = pguic->CtoSy(picky_c);
	}
	else
	{
		pickx_s = pguic->GetCursorX_S();
		picky_s = pguic->GetCursorY_S();
		pickx_c = pguic->GetCursorX_C();
		picky_c = pguic->GetCursorY_C();
	}
}

int GObjectPicker::PickPoint( PickFilterCallback pfunc/*=NULL*/ )
{
	pfilterfunc = pfunc;
	int retstate = 0;
	if (state == PICKSTATE_NULL)
	{
		mousestate = GOPMOUSE_NONE;
		state = PICKSTATE_REQUIREUPDATE;
		ClearSet();
	}
	else if (state == PICKSTATE_AFTERUPDATE)
	{
		state = PICKSTATE_REQUIREUPDATE;
	}
	else if (state == PICKSTATE_READY)
	{
		retstate = PICKSTATE_READY;
		state = PICKSTATE_NULL;
//		return true;
	}
//	return false;
	return mousestate|retstate;
}

int GObjectPicker::UpdatePickPoint()
{
	snappedstate = GOPSNAP_NONE;

	if (state != PICKSTATE_REQUIREUPDATE)
	{
		return mousestate|state;
	}
	state = PICKSTATE_AFTERUPDATE;

	MainInterface * pmain = &MainInterface::getInstance();
	if (!bTestMode)
	{
		if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN))
		{
			ClearSet();
		}
	}
	
	FillInitialPosition();

	GUICoordinate * pguic = &GUICoordinate::getInstance();
	snaprange_c = pguic->StoCs(snaprange_s);
	if (snaprange_c < M_FLOATEPS)
	{
		snaprange_c = M_FLOATEPS;
	}

	GObjectManager * pgm = &GObjectManager::getInstance();
	GObject * pBaseNode = pgm->GetMainBaseNode();

	nOnLine = 0;
	nPickObj = 0;
	nPickPIP = 0;

	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pickCoordEntityObj[i] = NULL;
	}

	int restoreSnapto = -1;
	if (!bTestMode)
	{
		if (!Command::getInstance().GetCurrentCommand() && !MarqueeSelect::getInstance().marqueestate)
		{
			restoreSnapto = snaptoflag;
			SetSnapTo(snaptoflag, false);
			SetSnapTo(GOPSNAP_GEOMETRY);
		}
	}

    // Snap to Self
    if (bCheckMouseDown)
    {
        CheckSnapPoint(mousedownx_c, mousedowny_c, GOPSNAP_SELF);
		bRenderMouseDown = true;
    }
	else
	{
		bRenderMouseDown = false;
	}
	// Snap to Geometry
    if (isSnapToGeometry())
	{
		CheckSnapGeometryPoint(pBaseNode);
		if (!pipinfo.empty())
		{
			for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
			{
				CheckSnapPoint(it->GetX(), it->GetY(), GOPSNAP_GEOMETRY);
			}
			if (isSnapToGeometryCoord())
			{
				for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
				{
					CheckSnapPointCoord(it->GetX(), it->GetY(), GOPSNAP_GEOMETRY);
				}
			}
		}
		CheckSnapGeometryLine(pBaseNode);

		if (isSnapToGeometryCoord())
		{
			CheckCoord_Obj(pBaseNode);
		}
	}
	// Snap to Continuity
    if (isSnapToContinuity())
	{
		CheckSnapContinuity();
	}
	// Snap to Coord
    if (isSnapToCoord())
	{
		CheckSnapPoint(0, 0, GOPSNAP_COORD);
		CheckSnapPointCoord(0, 0, GOPSNAP_COORD);
		if (bCheckMouseDown)
		{
			CheckSnapPointCoord(mousedownx_c, mousedowny_c, GOPSNAP_COORD);
		}
	}
	// Snap to Grid
    if (isSnapToGrid())
	{
		CheckSnapGrid();
	}

	// End of Snap
	if (restoreSnapto >= 0)
	{
		SetSnapTo(snaptoflag, false);
		SetSnapTo(restoreSnapto);
	}

	if (!bTestMode)
	{
		if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN))
		{
			mousedownx_c = pickx_c;
			mousedowny_c = picky_c;
			mousedownx_s = GetPickX_S();
			mousedowny_s = GetPickY_S();
			//		mousedownPickObj = pickObj;
			//		mousedownPickEntityObj = pickEntityObj;
			mousestate = GOPMOUSE_DOWN;
		}

		if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_UP))
		{
			mousestate = GOPMOUSE_UP;
			state = PICKSTATE_READY;
		}
	}
	else
	{
		mousestate = GOPMOUSE_UP;
		state = PICKSTATE_READY;
	}

	pipinfo.clear();
	bCheckMouseDown = false;

	return mousestate|state;
}

bool GObjectPicker::IsInSnapRangePoint_C( float x, float y )
{
	return MathHelper::getInstance().PointInCircle(pickx_c, picky_c, x, y, snaprange_c);
}

bool GObjectPicker::IsInSnapRangeXAxis_C( float y )
{
	return fabsf(picky_c-y) < snaprange_c;
}

bool GObjectPicker::IsInSnapRangeAngle_C( float x, float y, int angle, float *plx, float *ply )
{
	MathHelper * pmh = &MathHelper::getInstance();

	float lx, ly;
	float x1, y1, x2, y2;
	bool bIntersect = false;
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	bIntersect = pmh->GetLineSegmentInRect(x, y, angle, 0, 0, pguic->GetScreenWidth_C(), pguic->GetScreenWidth_C(), &x1, &y1, &x2, &y2);

	bool bret = pmh->PointNearToStraightLine(x, y, x1, y1, x2, y2, snaprange_c, &lx, &ly);
	if (plx)
	{
		*plx = lx;
	}
	if (ply)
	{
		*ply = ly;
	}
	return bret;
}

bool GObjectPicker::IsInSnapRangeYAxis_C( float x )
{
	return fabsf(pickx_c-x) < snaprange_c;
}

float GObjectPicker::GetPickX_S()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	return pguic->CtoSx(pickx_c);
}

float GObjectPicker::GetPickY_S()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	return pguic->CtoSy(picky_c);
}

#define _GOPRENDER_POINT_A		5
#define _GOPRENDER_POINT_CROSS	10

void GObjectPicker::Render()
{
#define _GOP_RENDER_ALPHA	0xA0
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	RenderHelper * prh = &RenderHelper::getInstance();
	ColorManager * pcm = &ColorManager::getInstance();

	float xs = GetPickX_S();
	float ys = GetPickY_S();

	if (snappedstate & GOPSNAPPED_XAXIS)
	{
		DWORD col = pcm->GetGridXAxisColor();
		col = SETA(col, _GOP_RENDER_ALPHA);
		prh->RenderLineR_S(0, ys, pguic->GetScreenWidth_S(), col);
	}
	if (snappedstate & GOPSNAPPED_YAXIS)
	{
		DWORD col = pcm->GetGridYAxisColor();
		col = SETA(col, _GOP_RENDER_ALPHA);
		prh->RenderLineB_S(xs, 0, pguic->GetScreenHeight_S(), col);
	}
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		for (int i=0; i<nPickObj; i++)
		{
			if (pickEntityObj[i])
			{
				pickEntityObj[i]->CallRender(LINECOLOR_HIGHLIGHT);
			}
		}
	}
	for (int i=0; i<nOnLine; i++)
	{
		if (pickCoordEntityObj[i])
		{
			pickCoordEntityObj[i]->CallRender(LINECOLOR_HIGHLIGHT);
		}
	}
	if (bRenderMouseDown)
	{
		prh->RenderSubstantivePoint(mousedownx_c, mousedowny_c, pcm->GetLayerLineHighlightColor());
	}
	if (snappedstate/* & GOPSNAPPED_POINT*/)
	{

		DWORD col = pcm->GetCursorColor();
		col = SETA(col, _GOP_RENDER_ALPHA);
		prh->RenderSquare_S(xs-_GOPRENDER_POINT_A, ys-_GOPRENDER_POINT_A, _GOPRENDER_POINT_A*2, col);
		prh->RenderLineR_S(xs-_GOPRENDER_POINT_CROSS, ys, _GOPRENDER_POINT_CROSS*2, col);
		prh->RenderLineB_S(xs, ys-_GOPRENDER_POINT_CROSS, _GOPRENDER_POINT_CROSS*2, col);
	}

}

bool GObjectPicker::CheckSnapGeometryPoint( GObject * pObj )
{
	if (!snappedstate)
	{
		if (pObj->isRepresentablePoint())
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
		if (pObj->isRepresentablePoint())
		{
			if (pObj->isSlaveToLine() || pObj->isSlaveToPiece() && !isSnapToVirtualCoord())
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
		if (pObj->isRepresentableLine())
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
					if (IsInSnapRangeAngle_C(it->GetX(), it->GetY(), it->GetAngle(), &lx, &ly))
					{
						if (nOnLine)
						{
							if (FindLineIntersectPIP(&(*it)))
							{
								snappedstate |= GOPSNAP_CONTINUITY;
								SetPickPIP((*it));
								nOnLine++;
							}
						}
						snappedstate |= GOPSNAP_CONTINUITY;
						SetPickPIP((*it));
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

GObject * GObjectPicker::GetPickedObj(int index/*=0*/)
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickObj[index];
	}
	return NULL;
}

GObject * GObjectPicker::GetPickedEntityObj(int index/*=0*/)
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickEntityObj[index];
	}
	return NULL;
}

/*
GObject * GObjectPicker::GetMDownPickedObj()
{
	if (mousestate != GOPMOUSE_NONE)
	{
		return mousedownPickObj;
	}
	return NULL;
}

GObject * GObjectPicker::GetMDownPickedEntityObj()
{
	if (mousestate != GOPMOUSE_NONE)
	{
		return mousedownPickEntityObj;
	}
    return NULL;
}
*/
void GObjectPicker::SetSnapTo(int snapto, bool bSet)
{
    if (bSet)
    {
        snaptoflag |= snapto;
    }
    else
    {
        snaptoflag &= ~snapto;
    }
}

void GObjectPicker::ClearSet()
{
	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pickObj[i] = NULL;
		pickEntityObj[i] = NULL;
		pickCoordEntityObj[i] = NULL;
	}

//	mousedownPickObj = NULL;
//	mousedownPickEntityObj = NULL;
}

void GObjectPicker::OnDeleteNode( GObject * node )
{
#define _NULLIFYNODE_IF_DELETED(NODE, DELETEDNODE)	\
	if (NODE == DELETEDNODE)	\
	{	\
		NODE = NULL;	\
	}

	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		_NULLIFYNODE_IF_DELETED(pickObj[i], node);
		_NULLIFYNODE_IF_DELETED(pickEntityObj[i], node);
		_NULLIFYNODE_IF_DELETED(pickCoordEntityObj[i], node);
	}
//	_NULLIFYNODE_IF_DELETED(mousedownPickObj, node);
//	_NULLIFYNODE_IF_DELETED(mousedownPickEntityObj, node);
}

bool GObjectPicker::IsPickReady( int iret/*=-1*/ )
{
	if (iret < 0)
	{
		return state & PICKSTATE_READY;
	}
	return iret & PICKSTATE_READY;
}

bool GObjectPicker::IsMouseDownReady()
{
	return mousestate & GOPMOUSE_DOWN;
}

void GObjectPicker::PushInterestPoint( float x, float y, bool bHasAngle/*=false*/, int angle/*=0*/ )
{
	PickerInterestPointInfo _info;
	_info.SetPosition(x, y);
	if (bHasAngle)
	{
		_info.SetAngle(angle);
	}
	for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
	{
		if ((*it).Equals(_info))
		{
			return;
		}
	}
	pipinfo.push_back(_info);
}

bool GObjectPicker::FindLineIntersectX( float y )
{
	// To set pickx_c and picky_c
	if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[0], 0, pickSection[0]);
		return SubFindLineX(pFakeLine[0], y, 1);
	}
	else if (snappedstate & GOPSNAPPED_YAXIS)
	{
		picky_c = y;
		return true;
	}
	else if (snappedstate & GOPSNAPPED_CONTINUITY)
	{
		return SubFindPIPX(&(pickPIP[0]), y);
	}
	return false;
}

bool GObjectPicker::FindLineIntersectY( float x )
{
	// To set pickx_c and picky_c
	if (snappedstate & GOPSNAPPED_LINE)
	{
		TraslateLineToStraightLine((GLine *)pickObj[0], 0, pickSection[0]);
		return SubFindLineY(pFakeLine[0], x, 1);
	}
	else if (snappedstate & GOPSNAPPED_XAXIS)
	{
		pickx_c = x;
		return true;
	}
	else if (snappedstate & GOPSNAPPED_CONTINUITY)
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

void GObjectPicker::SetPickObj( GObject * pObj )
{
	ASSERT(nPickObj < GOPONLINE_MAX);
	pickObj[nPickObj] = pObj;
	pickEntityObj[nPickObj] = pObj->getParent();
	if (pickEntityObj[nPickObj]->isLayer())
	{
		pickEntityObj[nPickObj] = pObj;
	}
	nPickObj++;
}

void GObjectPicker::SetPickPIP( PickerInterestPointInfo info )
{
	ASSERT(nPickPIP < GOPONLINE_MAX);
	pickPIP[nPickPIP] = info;
	nPickPIP++;
}

void GObjectPicker::SetPickObjCoord( GObject * pObj )
{
	ASSERT(nOnLine < GOPONLINE_MAX);
	pickCoordEntityObj[nOnLine] = pObj->getParent();
	if (pickCoordEntityObj[nOnLine]->isLayer())
	{
		pickCoordEntityObj[nOnLine] = pObj;
	}
	// No increment
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
	
	float arc = ARC(pPIP->GetAngle());
	float fcos = cosf(arc) * snaprange_c;
	float fsin = sinf(arc) * snaprange_c;
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

	float arc = ARC(pPIP->GetAngle());
	float fcos = cosf(arc) * snaprange_c;
	float fsin = sinf(arc) * snaprange_c;
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

	float arc1 = ARC(pPIP1->GetAngle());
	float fcos1 = cosf(arc1) * snaprange_c;
	float fsin1 = sinf(arc1) * snaprange_c;
	float xbase1 = pPIP1->GetX();
	float ybase1 = pPIP1->GetY();

	pFakeLine[0]->SetBeginEnd(xbase1-fcos1, ybase1-fsin1, xbase1+fcos1, ybase1+fsin1);

	float arc2 = ARC(pPIP2->GetAngle());
	float fcos2 = cosf(arc2) * snaprange_c;
	float fsin2 = sinf(arc2) * snaprange_c;
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

void GObjectPicker::TraslateLineToStraightLine( GLine * pLine, int index, int isec )
{
	if (pLine->isStraightLine())
	{
		pFakeLine[index]->SetBeginEnd(pLine->GetBeginPoint()->getX(), pLine->GetBeginPoint()->getY(), pLine->GetEndPoint()->getX(), pLine->GetEndPoint()->getY());
	}
	else
	{
		GBezierLine * pBLine = (GBezierLine *)pLine;
		pFakeLine[index]->SetBeginEnd(pBLine->bsinfo.GetX(isec), pBLine->bsinfo.GetY(isec), pBLine->bsinfo.GetX(isec+1), pBLine->bsinfo.GetY(isec+1));
	}
}

float GObjectPicker::CalculateProportion( int index/*=0 */ )
{
	GObject * pObj = GetPickedObj(index);
	if (!pObj)
	{
		return 0;
	}
	if (!pObj->isLine())
	{
		return 0;
	}
	GLine * pLine = (GLine *)pObj;
	return pLine->CalculateProportion(pickx_c, picky_c, pickSection[index]);
}

bool PickerInterestPointInfo::Equals( PickerInterestPointInfo & r )
{
	PointF2D p1(x, y);
	PointF2D p2(r.x, r.y);
	if (p1.Equals(p2))
	{
		if (bHasAngle)
		{
			if (r.bHasAngle && angle == r.angle)
			{
				return true;
			}
		}
		else
		{
			if (!r.bHasAngle)
			{
				return true;
			}
		}
	}
	return false;
}