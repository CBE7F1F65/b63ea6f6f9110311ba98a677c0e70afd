#pragma once

enum{
	ICMSIZE_SMALL=0,
	ICMSIZE_MIDDLE,
	ICMSIZE_LARGE,
};

class IconManager
{
public:
	static IconManager& getInstance()
	{
		static IconManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	IconManager();
	~IconManager();
	// Don't forget to declare these two.
	IconManager(IconManager const&);
	// Don't Implement
	void operator=(IconManager const&);
	// Don't implement

public:

	void Init();

	CImageList * GetImageList(int icmsize);
	int GetImageListType(int icmsize);

	int GetCommandIconIndex(int comm);
	
	CImageList imgList16;
	CImageList imgList24;
	CImageList imgList32;
};

