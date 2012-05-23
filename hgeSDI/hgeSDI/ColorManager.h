#pragma once

#include "MainDependency.h"

class ColorManager
{
public:
	ColorManager(void);
	~ColorManager(void);

	static DWORD GetBGColor();
	static DWORD GetGridMainColor();
	static DWORD GetGridSubColor();
	static DWORD GetGridXAxisColor();
	static DWORD GetGridYAxisColor();
	static DWORD GetCoordColor();
	static DWORD GetCursorColor();

	static DWORD GetLayerLineColor(int layer=-1);
};

