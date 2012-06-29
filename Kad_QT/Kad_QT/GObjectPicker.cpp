#include "StdAfx.h"
#include "GObjectPicker.h"
#include "MainInterface.h"
#include "MathHelper.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "GObjectManager.h"
#include "ColorManager.h"
#include "GLine.h"

GObjectPicker::GObjectPicker(void)
{
	state = PICKSTATE_NULL;
	bCheckMouseDown = false;
}

GObjectPicker::~GObjectPicker(void)
{
}

void GObjectPicker::Init()
{
	state = PICKSTATE_NULL;
	pfilterfunc = NULL;
	SetSnapTo(GOPSNAP_GEOMETRY|GOPSNAP_COORD|GOPSNAP_CONTINUITY);
	SetSnapRange(25.0f);
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
	if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN))
	{
		ClearSet();
	}


	
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	pickx_s = pguic->GetCursorX_S();
	picky_s = pguic->GetCursorY_S();
	pickx_c = pguic->GetCursorX_C();
	picky_c = pguic->GetCursorY_C();

	snaprange_c = pguic->StoCs(snaprange_s);

	bool bSnapped = false;
	GObjectManager * pgm = &GObjectManager::getInstance();
	GObject * pBaseNode = pgm->GetMainBaseNode();

    // Snap to Self
    if (!bSnapped && bCheckMouseDown)
    {
        bSnapped = CheckSnapPointAndCoord(mousedownx_c, mousedowny_c, GOPSNAP_SELF);
    }
	// Snap to Geometry
    if (!bSnapped && isSnapToGeometry())
	{
		bSnapped = CheckSnapGeometryPoint(pBaseNode);
		if (!bSnapped)
		{
			if (!pipinfo.empty())
			{
				for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
				{
					bSnapped = CheckSnapPointAndCoord(it->GetX(), it->GetY(), GOPSNAP_GEOMETRY);
					if (bSnapped)
					{
						break;
					}
				}
			}
		}
		if (!bSnapped)
		{
			bSnapped = CheckSnapGeometryLine(pBaseNode);
		}
	}
	// Snap to Continuity
    if (!bSnapped && isSnapToContinuity())
	{
		bSnapped = CheckSnapContinuity();
	}
	// Snap to Coord
    if (!bSnapped && isSnapToCoord())
	{
		bSnapped = CheckCoord_Obj(pBaseNode);
		if (snappedstate)
		{
			bSnapped = true;
		}
		if (!bSnapped)
		{
			bSnapped = CheckSnapPointAndCoord(0, 0, GOPSNAP_COORD);
		}
	}
	// Snap to Grid
    if (!bSnapped && isSnapToGrid())
	{
		bSnapped = CheckSnapGrid();
	}

	if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN))
	{
		mousedownx_c = pickx_c;
		mousedowny_c = picky_c;
		mousedownx_s = GetPickX_S();
		mousedowny_s = GetPickY_S();
		mousedownPickObj = pickObj;
		mousedownPickEntityObj = pickEntityObj;
		mousestate = GOPMOUSE_DOWN;
	}

	if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_UP))
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
		if (pickEntityObj)
		{
			pickEntityObj->CallRender(LINECOLOR_HIGHLIGHT);
		}
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
				snappedstate = GOPSNAPPED_POINT|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
				pickObj = pObj;
				pickEntityObj = pObj->getParent();
				if (pickEntityObj->isLayer())
				{
					pickEntityObj = pObj;
				}
				pickx_c = objx;
				picky_c = objy;
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
    return false;
}

bool GObjectPicker::CheckSnapPointAndCoord(float x, float y, int tostate)
{
    if (IsInSnapRangePoint_C(x, y))
    {
        snappedstate = tostate | GOPSNAPPED_VIRTUALPOINT;
        pickx_c = x;
        picky_c = y;
        return true;
    }
	else if (IsInSnapRangeXAxis_C(y))
	{
		snappedstate = tostate | GOPSNAPPED_XAXIS;
		picky_c = y;
		return true;
	}
	else if (IsInSnapRangeYAxis_C(x))
	{
		snappedstate = tostate | GOPSNAPPED_YAXIS;
		pickx_c = x;
		return true;
	}
    return false;
}

bool GObjectPicker::CheckCoord_Obj( GObject * pObj )
{
	if (pObj->isRepresentablePoint())
	{
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
				snappedstate |= GOPSNAPPED_XAXIS|GOPSNAP_COORD;
				picky_c = objy;
				if (snappedstate & GOPSNAPPED_YAXIS)
				{
					return true;
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
				snappedstate |= GOPSNAPPED_YAXIS|GOPSNAP_COORD;
				pickx_c = objx;
				if (snappedstate & GOPSNAPPED_XAXIS)
				{
					return true;
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
	return false;
}

bool GObjectPicker::CheckSnapGeometryLine( GObject * pObj )
{
	if (pObj->isRepresentableLine())
	{
		float neartox;
		float neartoy;
		if (((GLine *)pObj)->CheckNearTo(pickx_c, picky_c, snaprange_c, &neartox, &neartoy))
		{
			bool bret = true;
			if (pfilterfunc)
			{
				bret = pfilterfunc(pObj);
			}
			if (bret)
			{
				snappedstate = GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
				pickObj = pObj;
				pickEntityObj = pObj->getParent();
				if (pickEntityObj->isLayer())
				{
					pickEntityObj = pObj;
				}
				pickx_c = neartox;
				picky_c = neartoy;
				return true;
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
	return false;

}

bool GObjectPicker::CheckSnapGrid()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();

	float subspace = pguic->GetSubgirdSpace_C();
	int ixindex = (int)(pickx_c/subspace+0.5f);
	int iyindex = (int)(picky_c/subspace+0.5f);
	float fnx = ixindex*subspace;
	float fny = iyindex*subspace;
	if (IsInSnapRangePoint_C(fnx, fny))
	{
		snappedstate = GOPSNAPPED_POINT|GOPSNAP_GRID;
		pickx_c = fnx;
		picky_c = fny;
		return true;
	}
	return false;
}

bool GObjectPicker::CheckSnapContinuity()
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
					snappedstate = GOPSNAP_CONTINUITY;
					pickx_c = lx;
					picky_c = ly;
					return true;
				}
			}
		}
	}
	return false;
}

GObject * GObjectPicker::GetPickedObj()
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickObj;
	}
	return NULL;
}

GObject * GObjectPicker::GetPickedEntityObj()
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickEntityObj;
	}
	return NULL;
}

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
	pickObj = NULL;
	pickEntityObj = NULL;
	mousedownPickObj = NULL;
	mousedownPickEntityObj = NULL;
}

void GObjectPicker::OnDeleteNode( GObject * node )
{
#define _NULLIFYNODE_IF_DELETED(NODE, DELETEDNODE)	\
	if (NODE == DELETEDNODE)	\
	{	\
		NODE = NULL;	\
	}

	_NULLIFYNODE_IF_DELETED(pickObj, node);
	_NULLIFYNODE_IF_DELETED(pickEntityObj, node);
	_NULLIFYNODE_IF_DELETED(mousedownPickObj, node);
	_NULLIFYNODE_IF_DELETED(mousedownPickEntityObj, node);
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
	pipinfo.push_back(_info);
}