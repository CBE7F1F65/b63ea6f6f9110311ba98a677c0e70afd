#pragma once

#include "MainDependency.h"

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

	DWORD GetLayerLineColor(int layer=-1);
};

