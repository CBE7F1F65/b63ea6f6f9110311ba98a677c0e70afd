#pragma once

#include "MainDependency.h"

enum{
	COLORMT_LIST,
};

enum{
	COLORMS_ACTIVE,
	COLORMS_NONACTIVE,
	COLORMS_DISABLED,
};

class ColorManager
{
public:
	static ColorManager& getInstance()
	{
		static ColorManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	ColorManager();
	~ColorManager();
	// Don't forget to declare these two.
	ColorManager(ColorManager const&);
	// Don't Implement
	void operator=(ColorManager const&);
	// Don't implement

public:
	
	DWORD GetBGColor();
	DWORD GetGridMainColor();
	DWORD GetGridSubColor();
	DWORD GetGridXAxisColor();
	DWORD GetGridYAxisColor();
	DWORD GetCoordColor();
	DWORD GetCursorColor();

	DWORD GetTextColor(int type, int state);
	DWORD GetTextBkColor(int type, int state);
	DWORD ARGBToABGR(DWORD col);

	DWORD GetLayerLineColor(int layer=-1);
};

