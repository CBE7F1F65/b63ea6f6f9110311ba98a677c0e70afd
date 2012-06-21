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

HTARGET RenderTargetManager::UpdateTarget( int id, bool * updated/*=0*/, int w/*=0*/, int h/*=0*/ )
{
	if (!w && !h)
	{
		w = hge->System_GetState(HGE_SCREENWIDTH);
		h = hge->System_GetState(HGE_SCREENHEIGHT);
	}
	if (updated)
	{
		*updated = false;
	}
	for (list<RenderTargetInfo>::iterator it=tars.begin(); it!=tars.end(); ++it)
	{
		if (it->id == id)
		{
			if (it->w == w && it->h == h)
			{
				if (it->needupdate)
				{
					if (updated)
					{
						*updated = true;
					}
					it->needupdate = false;
				}
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
		if (updated)
		{
			*updated = true;
		}
		RenderTargetInfo _rti;
		_rti.id = id;
		_rti.tar = tar;
		_rti.w = w;
		_rti.h = h;
		_rti.needupdate = false;
		tars.push_back(_rti);
		return tar;
	}
	return NULL;
}

void RenderTargetManager::SetNeedUpdate()
{
	for (list<RenderTargetInfo>::iterator it=tars.begin(); it!=tars.end(); ++it)
	{
		it->needupdate = true;
	}
}