#include "StdAfx.h"
#include "RenderTargetManager.h"


RenderTargetManager::RenderTargetManager()
{
	hge = hgeCreate(HGE_VERSION);
}
RenderTargetManager::~RenderTargetManager()
{
	if (hge)
	{
		hge->Release();
	}
}

HTARGET RenderTargetManager::UpdateTarget( int id, int w/*=0*/, int h/*=0*/ )
{
	if (!w && !h)
	{
		w = hge->System_GetState(HGE_SCREENWIDTH);
		h = hge->System_GetState(HGE_SCREENHEIGHT);
	}
	for (list<RenderTargetInfo>::iterator it=tars.begin(); it!=tars.end(); ++it)
	{
		if (it->id == id)
		{
			if (it->w == w && it->h == h)
			{
				return it->tar;
			}

			hge->Target_Free(it->tar);
			tars.erase(it);
			break;
		}
	}
	HTARGET tar = hge->Target_Create(w, h, false);
	if (tar)
	{
		RenderTargetInfo _rti;
		_rti.id = id;
		_rti.tar = tar;
		_rti.w = w;
		_rti.h = h;
		tars.push_back(_rti);
		return tar;
	}
	return NULL;
}