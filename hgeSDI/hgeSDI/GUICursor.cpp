#include "stdafx.h"
#include "GUICursor.h"

#include "Main.h"
#include "RenderHelper.h"
#include "ColorManager.h"

GUICursor * pGUICursorSingleton = NULL;

GUICursor::GUICursor()
{
	assert(pGUICursorSingleton==NULL);
	guic = lastguic = GUIC_NORMAL;
}
GUICursor::~GUICursor()
{

}

GUICursor * GUICursor::getInstance()
{
	if (!pGUICursorSingleton)
	{
		pGUICursorSingleton = new GUICursor();
	}
	return pGUICursorSingleton;
}

void GUICursor::ChangeCursor( int _guic )
{
	if (_guic < 0)
	{
		guic = lastguic;
	}
	else
	{
		lastguic = guic;
		guic = _guic;
	}
}

void GUICursor::Render( float x, float y )
{
	HGE * hge = MainInterface::getInstance()->hge;
	if (hge)
	{
		DWORD col = ColorManager::GetCursorColor();
#define _GUICN_SIZE		48
#define _GUICN_BOXSIZE	3
#define _GUICN_SMALLSIZE		16
#define _GUICN_ARROWSIZE		6

		switch (guic)
		{
		case GUIC_NORMAL:
			/************************************************************************/
			/*              |                                                       */
			/*              |                                                       */
			/*           -------                                                    */
			/*           |  |  |                                                    */
			/*     --------------------                                             */
			/*           |  |  |                                                    */
			/*           -------                                                    */
			/*              |                                                       */
			/*              |                                                       */
			/************************************************************************/
			/*
			hge->Gfx_RenderLine(x-_GUICN_BOXSIZE, y-_GUICN_BOXSIZE, x+_GUICN_BOXSIZE, y-_GUICN_BOXSIZE, col);
			hge->Gfx_RenderLine(x+_GUICN_BOXSIZE, y-_GUICN_BOXSIZE, x+_GUICN_BOXSIZE, y+_GUICN_BOXSIZE, col);
			hge->Gfx_RenderLine(x+_GUICN_BOXSIZE, y+_GUICN_BOXSIZE, x-_GUICN_BOXSIZE, y+_GUICN_BOXSIZE, col);
			hge->Gfx_RenderLine(x-_GUICN_BOXSIZE, y+_GUICN_BOXSIZE, x-_GUICN_BOXSIZE, y-_GUICN_BOXSIZE, col);

			hge->Gfx_RenderLine(x-_GUICN_SIZE, y, x+_GUICN_SIZE, y, col);
			hge->Gfx_RenderLine(x, y-_GUICN_SIZE, x, y+_GUICN_SIZE, col);
			*/
			
			RenderHelper::RenderSquare(x-_GUICN_BOXSIZE, y-_GUICN_BOXSIZE, _GUICN_BOXSIZE*2, col);
			RenderHelper::RenderLineR(x-_GUICN_SIZE, y, _GUICN_SIZE*2, col);
			RenderHelper::RenderLineB(x, y-_GUICN_SIZE, _GUICN_SIZE*2, col);
			
			break;
		case GUIC_CREATEPOINT:
			/*
			hge->Gfx_RenderLine(x-_GUICN_SIZE, y, x+_GUICN_SIZE, y, col);
			hge->Gfx_RenderLine(x, y-_GUICN_SIZE, x, y+_GUICN_SIZE, col);
			*/
			RenderHelper::RenderLineR(x-_GUICN_SIZE, y, _GUICN_SIZE*2, col);
			RenderHelper::RenderLineB(x, y-_GUICN_SIZE, _GUICN_SIZE*2, col);
			break;
		case GUIC_HAND:
			/*
			hge->Gfx_RenderLine(x-_GUICN_SMALLSIZE, y, x+_GUICN_SMALLSIZE, y, col);
			hge->Gfx_RenderLine(x, y-_GUICN_SMALLSIZE, x, y+_GUICN_SMALLSIZE, col);

			hge->Gfx_RenderLine(x-_GUICN_SMALLSIZE, y, x-_GUICN_SMALLSIZE+_GUICN_ARROWSIZE, y-_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x-_GUICN_SMALLSIZE, y, x-_GUICN_SMALLSIZE+_GUICN_ARROWSIZE, y+_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x+_GUICN_SMALLSIZE, y, x+_GUICN_SMALLSIZE-_GUICN_ARROWSIZE, y-_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x+_GUICN_SMALLSIZE, y, x+_GUICN_SMALLSIZE-_GUICN_ARROWSIZE, y+_GUICN_ARROWSIZE, col);

			hge->Gfx_RenderLine(x, y-_GUICN_SMALLSIZE, x+_GUICN_ARROWSIZE, y-_GUICN_SMALLSIZE+_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x, y-_GUICN_SMALLSIZE, x-_GUICN_ARROWSIZE, y-_GUICN_SMALLSIZE+_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x, y+_GUICN_SMALLSIZE, x+_GUICN_ARROWSIZE, y+_GUICN_SMALLSIZE-_GUICN_ARROWSIZE, col);
			hge->Gfx_RenderLine(x, y+_GUICN_SMALLSIZE, x-_GUICN_ARROWSIZE, y+_GUICN_SMALLSIZE-_GUICN_ARROWSIZE, col);
			*/

			RenderHelper::RenderLineR(x-_GUICN_SMALLSIZE, y, _GUICN_SMALLSIZE*2, col);
			RenderHelper::RenderLineB(x, y-_GUICN_SMALLSIZE, _GUICN_SMALLSIZE*2, col);

			RenderHelper::RenderArrowR(x+_GUICN_SMALLSIZE, y, 0, _GUICN_ARROWSIZE, col);
			RenderHelper::RenderArrowB(x, y+_GUICN_SMALLSIZE, 0, _GUICN_ARROWSIZE, col);
			RenderHelper::RenderArrowL(x-_GUICN_SMALLSIZE, y, 0, _GUICN_ARROWSIZE, col);
			RenderHelper::RenderArrowT(x, y-_GUICN_SMALLSIZE, 0, _GUICN_ARROWSIZE, col);
			break;
		}
	}
}
