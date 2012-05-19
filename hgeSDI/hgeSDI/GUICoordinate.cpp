#include "StdAfx.h"
#include "GUICoordinate.h"

#include "Main.h"
#include "RenderTargetManager.h"

#include "RenderHelper.h"

#define _GUICG_MGID_SUB		0
#define _GUICG_MGID_MAIN	1
#define _GUICG_MGID_AXIS	2


#define _GUIC_SCALEMAX	40.0f
#define _GUIC_SCALEMIN	1.0f
#define _GUIC_DEFAULTSCALE	4.0f

GUICoordinate * pGUICoordinateSingleton = NULL;

GUICoordinate::GUICoordinate()
{
	assert(pGUICoordinateSingleton==NULL);
	SetGridColors(0xffa0a0a0, 0xffd0d0d0, 0xffff0000, 0xff00ff00, 0xff000000);

	hge = hgeCreate(HGE_VERSION);
	targrid = NULL;
	scale = _GUIC_DEFAULTSCALE;
}
GUICoordinate::~GUICoordinate()
{

}

GUICoordinate * GUICoordinate::getInstance()
{
	if (!pGUICoordinateSingleton)
	{
		pGUICoordinateSingleton = new GUICoordinate();
	}
	return pGUICoordinateSingleton;
}

void GUICoordinate::RenderGridReDraw()
{
	for (vector<GridInfo>::iterator it=scrgridxs.begin(); it!=scrgridxs.end(); ++it)
	{
		for (vector<GridInfo>::iterator jt=scrgridys.begin(); jt!=scrgridys.end(); ++jt)
		{
			DWORD col;
			switch (it->mgridID)
			{
			case _GUICG_MGID_AXIS:
				col = xaxiscol;
				break;
			case _GUICG_MGID_MAIN:
				col = gridcol;
				break;
			case _GUICG_MGID_SUB:
				col = subgridcol;
				break;
			}
			RenderHelper::RenderLineB(it->scrvalue, 0, scrh, col);
			switch (jt->mgridID)
			{
			case _GUICG_MGID_AXIS:
				col = yaxiscol;
				break;
			case _GUICG_MGID_MAIN:
				col = gridcol;
				break;
			case _GUICG_MGID_SUB:
				col = subgridcol;
				break;
			}
			RenderHelper::RenderLineR(0, jt->scrvalue, scrw, col);
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
		HTEXTURE tex = hge->Target_GetTexture(targrid);
		hgeQuad quad;
		quad.blend = BLEND_DEFAULT;
		quad.tex = tex;
		quad.v[0].x = 0; quad.v[0].y = 0; quad.v[0].tx = 0; quad.v[0].ty = 0, quad.v[0].z = 0, quad.v[0].col = 0xffffffff;
		quad.v[1].x = scrw; quad.v[1].y = 0; quad.v[1].tx = 1; quad.v[1].ty = 0, quad.v[1].z = 0, quad.v[1].col = 0xffffffff;
		quad.v[2].x = scrw; quad.v[2].y = scrh; quad.v[2].tx = 1; quad.v[2].ty = 1, quad.v[2].z = 0, quad.v[2].col = 0xffffffff;
		quad.v[3].x = 0; quad.v[3].y = scrh; quad.v[3].tx = 0; quad.v[3].ty = 1, quad.v[3].z = 0, quad.v[3].col = 0xffffffff;
		hge->Gfx_RenderQuad(&quad);
	}
}

void GUICoordinate::RenderCoordinate()
{
#define _GUICC_LENGTH		48
#define _GUICC_BOXSIZE		3
#define _GUICC_LETTERSSIZE		8
#define _GUICC_LETTERMARGIN		2
#define _GUICC_LETTERBEGIN		(_GUICC_LETTERMARGIN+_GUICC_LENGTH)
	if (scroriginx >= 0 && scroriginx <= scrw && scroriginy >= 0 && scroriginy <= scrh)
	{
		RenderHelper::RenderLineR(scroriginx, scroriginy, _GUICC_LENGTH, coordcol);
		RenderHelper::RenderLineB(scroriginx, scroriginy, _GUICC_LENGTH, coordcol);

		RenderHelper::RenderSquare(scroriginx-_GUICC_BOXSIZE, scroriginy-_GUICC_BOXSIZE, _GUICC_BOXSIZE*2, coordcol);

		// X
		RenderHelper::RenderLine(scroriginx+_GUICC_LETTERBEGIN, scroriginy+_GUICC_LETTERMARGIN, scroriginx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, scroriginy+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
		RenderHelper::RenderLine(scroriginx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, scroriginy+_GUICC_LETTERMARGIN, scroriginx+_GUICC_LETTERBEGIN, scroriginy+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
		// Y
		RenderHelper::RenderArrowB(scroriginx+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE/2, scroriginy+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE/2, 0, _GUICC_LETTERSSIZE/2, coordcol);
		RenderHelper::RenderLineB(scroriginx+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE/2, scroriginy+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE/2, _GUICC_LETTERSSIZE/2, coordcol);

		/*
		// Arrow
		hge->Gfx_RenderLine(scroriginx, scroriginy, scroriginx+_GUICC_SIZE, scroriginy, coordcol);
		hge->Gfx_RenderLine(scroriginx, scroriginy, scroriginx, scroriginy+_GUICC_SIZE, coordcol);

		// Box
		hge->Gfx_RenderLine(scroriginx-_GUICC_BOXSIZE, scroriginy-_GUICC_BOXSIZE, scroriginx+_GUICC_BOXSIZE, scroriginy-_GUICC_BOXSIZE, coordcol);
		hge->Gfx_RenderLine(scroriginx+_GUICC_BOXSIZE, scroriginy-_GUICC_BOXSIZE, scroriginx+_GUICC_BOXSIZE, scroriginy+_GUICC_BOXSIZE, coordcol);
		hge->Gfx_RenderLine(scroriginx+_GUICC_BOXSIZE, scroriginy+_GUICC_BOXSIZE, scroriginx-_GUICC_BOXSIZE, scroriginy+_GUICC_BOXSIZE, coordcol);
		hge->Gfx_RenderLine(scroriginx-_GUICC_BOXSIZE, scroriginy+_GUICC_BOXSIZE, scroriginx-_GUICC_BOXSIZE, scroriginy-_GUICC_BOXSIZE, coordcol);

		// Letter
		hge->Gfx_RenderLine(scroriginx+_GUICC_LETTERBEGIN, scroriginy+_GUICC_LETTERMARGIN, scroriginx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, scroriginy+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
		hge->Gfx_RenderLine(scroriginx+_GUICC_LETTERBEGIN+_GUICC_LETTERSSIZE, scroriginy+_GUICC_LETTERMARGIN, scroriginx+_GUICC_LETTERBEGIN, scroriginy+_GUICC_LETTERMARGIN+_GUICC_LETTERSSIZE, coordcol);
		*/
	}
}

void GUICoordinate::SetGrid( int _measuretype, float _originxpos, float _originypos, float _scale/*=1.0f*/ )
{
	measuretype = _measuretype;
	scroriginx = _originxpos;
	scroriginy = _originypos;
	switch (measuretype)
	{
	case GUICG_METRIC:
		nsubgrid = 5;
		break;
	case GUICG_IMPERIAL:
		nsubgrid = 4;
		break;
	}
	DoScale(_scale);
}

void GUICoordinate::DoPan( float xoffset, float yoffset )
{
//	ClientToCoordinate(&xoffset, &yoffset);
	scroriginx += xoffset;
	scroriginy += yoffset;
	UpdateScreenMeasure();
}

void GUICoordinate::DoScale( float _scale )
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
	scroriginx = cursorx_s*(1-invchangescale)+scroriginx*invchangescale;
	scroriginy = cursory_s*(1-invchangescale)+scroriginy*invchangescale;
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
	*x = ((*x)-scroriginx)/scale;
	*y = ((*y)-scroriginy)/scale;
}

void GUICoordinate::CoordinateToClient( float * x, float * y )
{
	if (!x || !y)
	{
		return;
	}
	CheckScale();
	*x = (*x)*scale+scroriginx;
	*y = (*y)*scale+scroriginy;
}

void GUICoordinate::UpdateScreenMeasure()
{
	CheckScale();

	float x = 0;
	float y = 0;
	ClientToCoordinate(&x, &y);
	scrlmeasure = x;
	scrtmeasure = y;
	
	scrw = x = hge->System_GetState(HGE_SCREENWIDTH);
	scrh = y = hge->System_GetState(HGE_SCREENHEIGHT);
	ClientToCoordinate(&x, &y);
	scrrmeasure = x;
	scrbmeasure = y;


#define _SCALEB_L1T 8.0f
#define _SCALESPACE	2.0f
#define _SCALESUBSCRMIN	12.0f


	if (scale > _SCALEB_L1T)
	{
		subgridspace = nsubgrid;
		subgridscrspace = subgridspace*scale;
	}
	else
	{
		subgridspace = _SCALESPACE*nsubgrid;
		subgridscrspace = subgridspace*scale;
		while (subgridscrspace < _SCALESUBSCRMIN)
		{
			subgridspace *= _SCALESPACE;
			subgridscrspace *= _SCALESPACE;
		}
	}

	scrgridxs.clear();
	scrgridys.clear();
	float coordscrl = scrlmeasure;
	float coordscrt = scrtmeasure;

	int nxbegin = (int)(coordscrl/subgridspace+0.5f);
	int nybegin = (int)(coordscrt/subgridspace+0.5f);
	float nearestxbegin = nxbegin*subgridspace;
	float nearestybegin = nybegin*subgridspace;


	CoordinateToClient(&nearestxbegin, &nearestybegin);
	if (nearestxbegin < 0)
	{
		nearestxbegin += subgridscrspace;
		nxbegin++;
	}
	if (nearestybegin < 0)
	{
		nearestybegin += subgridscrspace;
		nybegin++;
	}
	int nxnow = nxbegin;
	int nynow = nybegin;
	int xloopbeginval = nxbegin%nsubgrid;
	int yloopbeginval = nybegin%nsubgrid;

	for (float f=nearestxbegin; f<=scrw; f+=subgridscrspace)
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
		scrgridxs.push_back(_ginfo);
		nxnow++;
		xloopbeginval++;
		if (xloopbeginval == nsubgrid)
		{
			xloopbeginval = 0;
		}
	}
	for (float f=nearestybegin; f<=scrh; f+=subgridscrspace)
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
		scrgridys.push_back(_ginfo);
		yloopbeginval++;
		nynow++;
		if (yloopbeginval == nsubgrid)
		{
			yloopbeginval = 0;
		}
	}

	targrid = RenderTargetManager::getInstance()->UpdateTarget(RTID_GRID, scrw, scrh);
	if (targrid)
	{
		hge->Gfx_BeginScene(targrid);
		hge->Gfx_Clear(0x0);
		RenderGridReDraw();
		hge->Gfx_EndScene();
	}
}

void GUICoordinate::SetGridColors( DWORD _gridcol, DWORD _subgridcol, DWORD _xaxiscol, DWORD _yaxiscol, DWORD _coordcol )
{
	gridcol = _gridcol;
	subgridcol = _subgridcol;
	xaxiscol = _xaxiscol;
	yaxiscol = _yaxiscol;
	coordcol = _coordcol;
}

void GUICoordinate::SetCursorPosition( float x, float y )
{
	cursorx_s = cursorxcoord = x;
	cursory_s = cursorycoord = y;
	ClientToCoordinate(&cursorxcoord, &cursorycoord);
}