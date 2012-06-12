#include "StdAfx.h"
#include "IconManager.h"
#include "Resource.h"

IconManager::IconManager(void)
{
}


IconManager::~IconManager(void)
{
}

#define _ICM_BMPMAX_S	2
#define _ICM_BMPMAX_M	2
#define _ICM_BMPMAX_L	2

void IconManager::Init()
{
	imgList32.Create(ICMSIZE_LARGE, ICMSIZE_LARGE, ILC_COLORDDB|ILC_MASK, 0, 4);
	CBitmap bm32[_ICM_BMPMAX_L];
	for (int i=0; i<_ICM_BMPMAX_L; i++)
	{
		bm32[i].LoadBitmap(IDB_UICOMMANDICONS_32_00+i);
		imgList32.Add(&(bm32[i]), RGB(0, 0, 0));
	}

	imgList24.Create(ICMSIZE_MIDDLE, ICMSIZE_MIDDLE, ILC_COLORDDB|ILC_MASK, 0, 4);
	CBitmap bm24[_ICM_BMPMAX_M];
	for (int i=0; i<_ICM_BMPMAX_M; i++)
	{
		bm24[i].LoadBitmap(IDB_UICOMMANDICONS_24_00+i);
		imgList24.Add(&(bm24[i]), RGB(0, 0, 0));
	}

	imgList16.Create(ICMSIZE_SMALL, ICMSIZE_SMALL, ILC_COLORDDB|ILC_MASK, 0, 4);
	CBitmap bm16[_ICM_BMPMAX_S];
	for (int i=0; i<_ICM_BMPMAX_S; i++)
	{
		bm16[i].LoadBitmap(IDB_UICOMMANDICONS_16_00+i);
		imgList16.Add(&(bm16[i]), RGB(0, 0, 0));
	}
}

CImageList * IconManager::GetImageList( int icmsize )
{
	switch (icmsize)
	{
	case ICMSIZE_SMALL:
		return &imgList16;
	case ICMSIZE_MIDDLE:
		return &imgList24;
	case ICMSIZE_LARGE:
		return &imgList32;
	}
	DASSERT(true);
	return NULL;
}

int IconManager::GetImageListType( int icmsize )
{
	return LVSIL_SMALL;
}

int IconManager::GetCommandIconIndex( int comm )
{
	// ToDo!!
	return 0;
}

int IconManager::GetLBVisibleIcon( bool bVisible, bool bRecVisible )
{
	// ToDo!!
	return 0;
}

int IconManager::GetLBLockedIcon( bool bLock, bool bRecLock )
{
	// ToDo!!
	return 0;
}

int IconManager::GetLBFoldExpandIcon( bool bFolded )
{
	// ToDo!!
	return 0;
}

int IconManager::GetLBLineSelectIcon( bool toSelect )
{
	// ToDo!!
	return 0;
}

int IconManager::GetCMDDeleteItemIcon( int state )
{
	// ToDo!!
	return 0;
}

int IconManager::GetCMDDuplicateIcon( int state )
{
	// ToDo!!
	return 0;
}

int IconManager::GetCMDNewLayerIcon( int state )
{
	// ToDo!!
	return 0;
}

int IconManager::GetCMDNewSubLayerIcon( int state )
{
	// ToDo!!
	return 0;
}

int IconManager::GetCMDSnapShotIcon( int state )
{
	// ToDo!!
	return 0;
}

int IconManager::GetBlankIcon()
{
	// ToDo!!
	return 0;
}