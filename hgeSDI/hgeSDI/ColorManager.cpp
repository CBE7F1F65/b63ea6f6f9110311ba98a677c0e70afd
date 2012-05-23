#include "StdAfx.h"
#include "ColorManager.h"


ColorManager::ColorManager(void)
{
}


ColorManager::~ColorManager(void)
{
}

DWORD ColorManager::GetBGColor()
{
	return 0xfff0f0f0;
}

DWORD ColorManager::GetGridMainColor()
{
	return 0xffa0a0a0;
}

DWORD ColorManager::GetGridSubColor()
{
	return 0xffd0d0d0;
}

DWORD ColorManager::GetGridXAxisColor()
{
	return 0xffff0000;
}

DWORD ColorManager::GetGridYAxisColor()
{
	return 0xff00ff00;
}

DWORD ColorManager::GetCoordColor()
{
	return 0xff000000;
}

DWORD ColorManager::GetCursorColor()
{
	return 0xff000000;
}

DWORD ColorManager::GetLayerLineColor( int layer/*=-1*/ )
{
	return 0xff000000;
}