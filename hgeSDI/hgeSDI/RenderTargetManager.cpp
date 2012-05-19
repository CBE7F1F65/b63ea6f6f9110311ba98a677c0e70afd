#include "StdAfx.h"
#include "RenderTargetManager.h"

RenderTargetManager * pRenderTargetManagerSingleton = NULL;

RenderTargetManager::RenderTargetManager()
{
	assert(pRenderTargetManagerSingleton==NULL);
	hge = hgeCreate(HGE_VERSION);
}
RenderTargetManager::~RenderTargetManager()
{

}

RenderTargetManager * RenderTargetManager::getInstance()
{
	if (!pRenderTargetManagerSingleton)
	{
		pRenderTargetManagerSingleton = new RenderTargetManager();
	}
	return pRenderTargetManagerSingleton;
}

HTARGET RenderTargetManager::UpdateTarget( int id, int w, int h )
{
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