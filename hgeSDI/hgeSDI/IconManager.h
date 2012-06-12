#pragma once


#define ICMSIZE_SMALL	16
#define ICMSIZE_MIDDLE	24
#define ICMSIZE_LARGE	32


enum{
	ICMSTATE_NORMAL=0,
	ICMSTATE_DISABLED,
};

class IconManager
{
public:
	static IconManager& getInstance() { static IconManager instance; return instance; }

private:
	IconManager();
	~IconManager();
	IconManager(IconManager const&);
	void operator=(IconManager const&);

public:

	void Init();

	CImageList * GetImageList(int icmsize);
	int GetImageListType(int icmsize);

	int GetCommandIconIndex(int comm);
	int GetLBVisibleIcon( bool bVisible, bool bRecVisible );
	int GetLBLockedIcon( bool bLock, bool bRecLock );
	int GetLBFoldExpandIcon( bool bFolded );
	int GetLBLineSelectIcon( bool toSelect );
	int GetCMDDeleteItemIcon( int state );
	int GetCMDSnapShotIcon( int state );
	int GetCMDDuplicateIcon( int state );
	int GetCMDNewLayerIcon( int state );
	int GetCMDNewSubLayerIcon( int state );
	int GetBlankIcon();
	CImageList imgList16;
	CImageList imgList24;
	CImageList imgList32;
};

