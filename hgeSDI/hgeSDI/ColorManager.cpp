#include "StdAfx.h"
#include "ColorManager.h"


ColorManager::ColorManager(void)
{
	hge = hgeCreate(HGE_VERSION);
}


ColorManager::~ColorManager(void)
{
	if (hge)
	{
		hge->Release();
	}
}

DWORD ColorManager::GetLayerLineColor( int layer/*=-1*/ )
{
	return 0xffffffff;
}

DWORD ColorManager::GetTextColor( int type, int state )
{
	if (type == COLORMT_LIST)
	{
		switch (state)
		{
		case COLORMS_ACTIVE:
		case COLORMS_NONACTIVE:
		case COLORMS_EDITABLE:
			return 0xffffffff;
		case COLORMS_DISABLED:
			return 0xff717171;
		}
	}
	return 0xffffffff;
}

DWORD ColorManager::GetTextBkColor( int type, int state )
{
	if (type == COLORMT_LIST)
	{
		switch (state)
		{
		case COLORMS_ACTIVE:
			return 0xff596678;
		case COLORMS_NONACTIVE:
			return 0xff494949;
		case COLORMS_EDITABLE:
			return 0xff595959;
		case COLORMS_DISABLED:
			return 0xff535353;
		}
	}
	return 0xffffffff;
}

DWORD ColorManager::ARGBToABGR( DWORD col )
{
	return ARGB(GETA(col), GETB(col), GETG(col), GETR(col));
}

void ColorManager::Init()
{

}