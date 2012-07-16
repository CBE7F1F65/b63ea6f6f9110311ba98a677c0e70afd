#include "stdafx.h"
#include "GObjectPicker.h"
#include "MainInterface.h"
#include "GUICoordinate.h"
#include "GObjectManager.h"
#include "MarqueeSelect.h"
#include "RenderHelper.h"
#include "MarkingManager.h"

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

	nCurrentLockAngleIndex = 0;
	AdjustPositionToLocks();

	bSplitMarkingInUse = false;

	int restoreSnapto = -1;

	if (!bTestMode)
	{
		if (!Command::getInstance().GetCurrentCommand() && !MarqueeSelect::getInstance().marqueestate)
		{
			restoreSnapto = snaptoflag;
			SetSnapTo(snaptoflag, false);
			SetSnapTo(GOPSNAP_GEOMETRY|(restoreSnapto&GOPSNAP_HANDLEONLY));
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

	if (!bSplitMarkingInUse)
	{
		if (pSplitMarking)
		{
			MarkingManager::getInstance().DisableMarking(pSplitMarking);
			pSplitMarking = NULL;
		}
	}

	pipinfo.clear();
	bCheckMouseDown = false;

	return mousestate|state;
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