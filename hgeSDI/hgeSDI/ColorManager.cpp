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
	return 0xff212830;
}

DWORD ColorManager::GetGridMainColor()
{
	return 0xff333949;
}

DWORD ColorManager::GetGridSubColor()
{
	return 0xff272d38;
}

DWORD ColorManager::GetGridXAxisColor()
{
	return 0xff6d2426;
}

DWORD ColorManager::GetGridYAxisColor()
{
	return 0xff236e26;
}

DWORD ColorManager::GetCoordColor()
{
	return 0xffffffff;
}

DWORD ColorManager::GetCursorColor()
{
	return 0xffffffff;
}

DWORD ColorManager::GetLayerLineColor( int layer/*=-1*/ )
{
	return 0xffffffff;
}