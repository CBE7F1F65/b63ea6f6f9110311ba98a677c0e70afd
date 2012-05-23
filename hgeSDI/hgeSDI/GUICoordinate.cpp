#include "StdAfx.h"
#include "GUICoordinate.h"

#include "Main.h"
#include "RenderTargetManager.h"

#include "RenderHelper.h"
#include "ColorManager.h"

#include "GUICursor.h"

#include "Command.h"

#define _GUICG_MGID_SUB		0
#define _GUICG_MGID_MAIN	1
#define _GUICG_MGID_AXIS	2


#define _GUIC_SCALEMAX	40.0f
#define _GUIC_SCALEMIN	1.0f
#define _GUIC_DEFAULTSCALE	4.0f

GUICoordinate::GUICoordinate()
{
	hge = hgeCreate(HGE_VERSION);
	targrid = NULL;
	scale = _GUIC_DEFAULTSCALE;
}

GUICoordinate::~GUICoordinate()
{
	if (hge)
	{
		hge->Release();
	}
}

void GUICoordinate::RenderGridReDraw()
{
	for (vector<GridInfo>::iterator it=gridxs_s.begin(); it!=gridxs_s.end(); ++it)
	{
		for (vector<GridInfo>::iterator jt=gridys_s.begin(); jt!=gridys_s.end(); ++jt)
		{
			DWORD col;
			switch (it->mgridID)
			{
			case _GUICG_MGID_AXIS:
				col = ColorManager::GetGridXAxisColor();
				break;
			case _GUICG_MGID_MAIN:
				col = ColorManager::GetGridMainColor();
				break;
			case _GUICG_MGID_SUB:
				col = ColorManager::GetGridSubColor();
				break;
			}
			RenderHelper::RenderLineB(it->scrvalue, 0, scrh_s, col);
			switch (jt->mgridID)
			{
			case _GUICG_MGID_AXIS:
				col = ColorManager::GetGridYAxisColor();
				break;
			case _GUICG_MGID_MAIN:
				col = ColorManager::GetGridMainColor();
				break;
			case _GUICG_MGID_SUB:
				col = ColorManager::GetGridSubColor();
				break;
			}
			RenderHelper::RenderLineR(0, jt->scrvalue, scrw_s, col);
		}
	}
}

void GUICoordinate::RenderGrid()
{
	if (!targrid)
	{
		RenderGridReDraw();
	}
	else
	{
		RenderHelper::TargetQuadRender(targrid, 0, 0, 0xffffffff);
		/*
		HTEXTURE tex = hge->Target_GetTexture(targrid);
		hgeQuad quad;
		quad.blend = BLEND_DEFAULT;
		quad.tex = tex;
		quad.v[0].x = 0; quad.v[0].y = 0; quad.v[0].tx = 0; quad.v[0].ty = 0, quad.v[0].z = 0, quad.v[0].col = 0xffffffff;
		quad.v[1].x = scrw_s; quad.v[1].y = 0; quad.v[1].tx = 1; quad.v[1].ty = 0, quad.v[1].z = 0, quad.v[1].col = 0xffffffff;
		quad.v[2].x = scrw_s; quad.v[2].y = scrh_s; quad.v[2].tx = 1; quad.v[2].ty = 1, quad.v[2].z = 0, quad.v[2].col = 0xffffffff;
		quad.v[3].x = 0; quad.v[3].y = scrh_s; quad.v[3].tx = 0; quad.v[3].ty = 1, quad.v[3].z = 0, quad.v[3].col = 0xffffffff;
		hge->Gfx_RenderQuad(&quad);
		*/
	}
}

void GUICoordinate::DoRenderCoordinate( float renderatx, float renderaty )
{
#define _GUICC_LENGTH		48
#define _GUICC_BOXSIZE		3
#define _GUICC_LETTERSSIZE		8
#define _GUICC_LETTERMARGIN		2
#define _GUICC_LETTERBEGIN		(_GUICC_LETTERMARGIN+_GUICC_LENGTH)

	DWORD coordcol = ColorManager::GetCoordColor();

	RenderHelper::RenderLineR(renderatx, renderaty, _GUICC_LENGTH, coordcol);
	RenderHelper::RenderLineB(renderatx, renderaty, _GUICC_LENGTH, coordcol);

	RenderHelper::RenderSquare(renderatx-_GUICC_BOXSIZE, renderaty-_GUICC_BOXSIZE, _GUICC_BOXSIZE*2, coordcol);

	// X
	RenderHelper::RenderLine(renderatx+_GUICC_LETTERBEGIN, renderaty+_GUICC_LETTERMARGIN, renderatx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, renderaty+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
	RenderHelper::RenderLine(renderatx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, renderaty+_GUICC_LETTERMARGIN, renderatx+_GUICC_LETTERBEGIN, renderaty+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
	// Y
	RenderHelper::RenderArrowB(renderatx+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE/2, renderaty+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE/2, 0, _GUICC_LETTERSSIZE/2, coordcol);
	RenderHelper::RenderLineB(renderatx+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE/2, renderaty+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE/2, _GUICC_LETTERSSIZE/2, coordcol);
}

void GUICoordinate::RenderCoordinate()
{

#define _GUICC_RENDERAT	15

	if (originx_s < 0 || originx_s > scrw_s || originy_s < 0 || originy_s > scrh_s)
	{
	}
	else
	{
		DoRenderCoordinate(originx_s, originy_s);
	}
	DoRenderCoordinate(_GUICC_RENDERAT, _GUICC_RENDERAT);

}

void GUICoordinate::SetGrid( int _measuretype, float _originxpos, float _originypos, float _scale/*=1.0f*/ )
{
	measuretype = _measuretype;
	originx_s = _originxpos;
	originy_s = _originypos;
	switch (measuretype)
	{
	case GUICG_METRIC:
		nsubgrid = 5;
		break;
	case GUICG_IMPERIAL:
		nsubgrid = 4;
		break;
	}
	DoZoom(0, 0, _scale);
}

int GUICoordinate::DoZoomCommand()
{
	Command * pcommand = &(Command::getInstance());
	float x, y, _scale;
	pcommand->GetParamXY(CSP_DOZOOM_C_XY_SCALE, &x, &y);
	_scale = pcommand->GetParamF(CSP_DOZOOM_C_XY_SCALE);
	DoZoom(x, y, _scale);
	return pcommand->FinishCommand();
}

void GUICoordinate::DoPan( float xoffset_s, float yoffset_s )
{
//	ClientToCoordinate(&xoffset, &yoffset);
	originx_s += xoffset_s;
	originy_s += yoffset_s;
	if (xoffset_s || yoffset_s)
	{
		UpdateScreenMeasure();
	}
}

int GUICoordinate::DoPanCommand()
{
	Command * pcommand = &Command::getInstance();
	MainInterface * pmain = &MainInterface::getInstance();
	switch (pcommand->GetStep())
	{
	case CSI_INIT:
		panwillterminate = false;
		GUICursor::getInstance().ChangeCursor(GUIC_HAND);
		pcommand->StepTo(CSI_PAN_READY);
		break;
	case CSI_PAN_READY:
		if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_UP))
		{
			if (hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex))
			{
				panwillterminate = true;
			}
			else
			{
				pcommand->StepTo(CSI_FINISH);
			}
		}
		if (hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex) && !hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN))
		{
			DoPan(pmain->mousex-pmain->lastmousex, pmain->mousey-pmain->lastmousey);
		}
		else if (panwillterminate && !hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex))
		{
			pcommand->StepTo(CSI_FINISH);
		}
		break;
	case CSI_PAUSE:
		GUICursor::getInstance().ChangeCursor();
		break;
	case CSI_RESUME:
		GUICursor::getInstance().ChangeCursor(GUIC_HAND);
		break;
	case CSI_FINISH:
		GUICursor::getInstance().ChangeCursor();
		pcommand->FinishCommand();
		break;
	case CSI_TERMINAL:
		GUICursor::getInstance().ChangeCursor();
		pcommand->TerminalCommand();
		break;
	}
	return 0;
}

void GUICoordinate::DoZoom(float cx_s, float cy_s, float _scale )
{
	float oldscale = scale;
	scale *= _scale;
	if (scale > _GUIC_SCALEMAX)
	{
		scale = _GUIC_SCALEMAX;
	}
	else if (scale < _GUIC_SCALEMIN)
	{
		scale = _GUIC_SCALEMIN;
	}
	float invchangescale = scale/oldscale;
	originx_s = cx_s*(1-invchangescale)+originx_s*invchangescale;
	originy_s = cy_s*(1-invchangescale)+originy_s*invchangescale;
//	scroriginx *= invchangescale;
//	scroriginy *= invchangescale;
	UpdateScreenMeasure();
}

void GUICoordinate::ClientToCoordinate( float * x, float * y )
{
	if (!x || !y)
	{
		return;
	}
	CheckScale();
	*x = ((*x)-originx_s)/scale;
	*y = ((*y)-originy_s)/scale;
}

void GUICoordinate::CoordinateToClient( float * x, float * y )
{
	if (!x || !y)
	{
		return;
	}
	CheckScale();
	*x = (*x)*scale+originx_s;
	*y = (*y)*scale+originy_s;
}

void GUICoordinate::UpdateScreenMeasure()
{
	CheckScale();

	float x = 0;
	float y = 0;
	ClientToCoordinate(&x, &y);
	lboundary_c = x;
	tboundary_c = y;
	
	scrw_s = x = hge->System_GetState(HGE_SCREENWIDTH);
	scrh_s = y = hge->System_GetState(HGE_SCREENHEIGHT);
	ClientToCoordinate(&x, &y);
	rboundary_c = x;
	bboundary_c = y;


#define _SCALEB_L1T 8.0f
#define _SCALESPACE	2.0f
#define _SCALESUBSCRMIN	12.0f


	if (scale > _SCALEB_L1T)
	{
		subgridspace_c = nsubgrid;
		subgridspace_s = subgridspace_c*scale;
	}
	else
	{
		subgridspace_c = _SCALESPACE*nsubgrid;
		subgridspace_s = subgridspace_c*scale;
		while (subgridspace_s < _SCALESUBSCRMIN)
		{
			subgridspace_c *= _SCALESPACE;
			subgridspace_s *= _SCALESPACE;
		}
	}

	gridxs_s.clear();
	gridys_s.clear();
	float coordscrl = lboundary_c;
	float coordscrt = tboundary_c;

	int nxbegin = (int)(coordscrl/subgridspace_c+0.5f);
	int nybegin = (int)(coordscrt/subgridspace_c+0.5f);
	float nearestxbegin = nxbegin*subgridspace_c;
	float nearestybegin = nybegin*subgridspace_c;


	CoordinateToClient(&nearestxbegin, &nearestybegin);
	if (nearestxbegin < 0)
	{
		nearestxbegin += subgridspace_s;
		nxbegin++;
	}
	if (nearestybegin < 0)
	{
		nearestybegin += subgridspace_s;
		nybegin++;
	}
	int nxnow = nxbegin;
	int nynow = nybegin;
	int xloopbeginval = nxbegin%nsubgrid;
	int yloopbeginval = nybegin%nsubgrid;

	for (float f=nearestxbegin; f<=scrw_s; f+=subgridspace_s)
	{
		GridInfo _ginfo;
		_ginfo.scrvalue = f;
		if (!xloopbeginval)
		{
			if (!nxnow)
			{
				_ginfo.mgridID = _GUICG_MGID_AXIS;
			}
			else
			{
				_ginfo.mgridID = _GUICG_MGID_MAIN;
			}
		}
		else
		{
			_ginfo.mgridID = _GUICG_MGID_SUB;
		}
		gridxs_s.push_back(_ginfo);
		nxnow++;
		xloopbeginval++;
		if (xloopbeginval == nsubgrid)
		{
			xloopbeginval = 0;
		}
	}
	for (float f=nearestybegin; f<=scrh_s; f+=subgridspace_s)
	{
		GridInfo _ginfo;
		_ginfo.scrvalue = f;
		if (!yloopbeginval)
		{
			if (!nynow)
			{
				_ginfo.mgridID = _GUICG_MGID_AXIS;
			}
			else
			{
				_ginfo.mgridID = _GUICG_MGID_MAIN;
			}
		}
		else
		{
			_ginfo.mgridID = _GUICG_MGID_SUB;
		}
		gridys_s.push_back(_ginfo);
		yloopbeginval++;
		nynow++;
		if (yloopbeginval == nsubgrid)
		{
			yloopbeginval = 0;
		}
	}

	targrid = RenderTargetManager::getInstance().UpdateTarget(RTID_GRID);
	if (targrid)
	{
		RenderHelper::BeginRenderTar(targrid);
		RenderGridReDraw();
		RenderHelper::EndRenderTar();
	}
}

void GUICoordinate::SetCursorPosition( float x_s, float y_s )
{
	cursorx_s = cursorx_c = x_s;
	cursory_s = cursory_c = y_s;
	ClientToCoordinate(&cursorx_c, &cursory_c);
}