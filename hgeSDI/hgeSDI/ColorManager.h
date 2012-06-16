#pragma once

// No include Objs!
#include "MainDependency.h"
#include "StringManager.h"

enum{
	COLORMT_LIST=0,

	_COLORMT_ENDINDEX,
};

enum{
	COLORMS_ACTIVE=0,
	COLORMS_NONACTIVE,
	COLORMS_BACKGROUND,
	COLORMS_FRAME,
	COLORMS_NONEEDITABLE,
	COLORMS_SUBSTANCE,
	COLORMS_DISABLED,

	_COLORMS_ENDINDEX,
};

enum{
	LINECOLOR_NORMAL=0,
	LINECOLOR_ACTIVE,			// Selected
	LINECOLOR_HIGHLIGHT,		// Picking

	_LINECOLOR_ENDINDEX
};

class LineColorSet
{
public:
	LineColorSet(){ClearSet();};
	~LineColorSet(){};

	void ClearSet()
	{
		for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
		{
			dwColors[i] = 0;
		}
		bColorSet = false;
	};
	bool IsColorSet()
	{
		return bColorSet;
	};
	void SetColor(int index, DWORD col)
	{
		dwColors[index] = col;
		bColorSet = true;
	};
	DWORD GetColor( int iHighlightLevel )
	{
		return dwColors[iHighlightLevel];
	};
	DWORD dwColors[_LINECOLOR_ENDINDEX];
	bool bColorSet;
};


class UIColor
{
public:
	UIColor()
	{
		ClearSet();
	};
	~UIColor(){};

	DWORD col;
	void ClearSet()
	{
		col = 0;
	};

	UIColor & operator=(DWORD _col)
	{
		col = _col;
		return *this;
	};
};

class ColorManager
{
public:
	static ColorManager& getInstance() { static ColorManager instance; return instance; }

private:
	ColorManager();
	~ColorManager();
	ColorManager(ColorManager const&);
	void operator=(ColorManager const&);

public:

	void Init();
	HGE * hge;
	/************************************************************************
	UIColor colName;
	DWORD GetNameColor()
	{
		if (!colName.col)
		{
			DWORD col;
			const char * colstr = hge->Ini_GetString(INIS_COLOR, Name, "");
			if (strlen(colstr))
			{
				sscanf_s(colstr, "%x", &col);
				colName = col;
			}
			else
			{
				colName = Def;
			}
		}
		return colName.col;
	};
	void RestoreNameColor(bool bWrite=true)
	{
		colName = Def;
		if (bWrite)
		{
			hge->Ini_SetString(INIS_COLOR, Name, "");
		}
	}
	************************************************************************/

#define _CMFDEF(COLNAME, DEF)	\
	UIColor col##COLNAME;	\
	DWORD Get##COLNAME##Color()	\
	{	\
		if (!col##COLNAME.col)	\
		{	\
			DWORD col;	\
			const char * colstr = hge->Ini_GetString(INIS_COLOR, #COLNAME, "");	\
			if (strlen(colstr))	\
			{	\
				sscanf_s(colstr, "%x", &col);	\
				col##COLNAME = col;	\
			}	\
			else	\
			{	\
				col##COLNAME = DEF;	\
			}	\
		}	\
		return col##COLNAME.col;	\
	};	\
	void Restore##COLNAME##Color(bool bWrite=true)	\
	{	\
		col##COLNAME = DEF;	\
		if (bWrite)	\
		{	\
			hge->Ini_SetString(INIS_COLOR, #COLNAME, "");	\
		}	\
	}

	_CMFDEF(	BG,	0xff212830	);
	_CMFDEF(	GridMain,	0xff333949	);
	_CMFDEF(	GridSub,	0xff272d38	);
	_CMFDEF(	GridXAxis,	0xff6d2426	);
	_CMFDEF(	GridYAxis,	0xff236e26	);
	_CMFDEF(	Coord,	0xffffffff	);
	_CMFDEF(	Cursor,	0xffffffff	);
	_CMFDEF(	MarqueeLine,	0xffffffff	);

	// ToDo!!
	void RestoreDefault();

	/*
	DWORD GetBGColor();
	DWORD GetGridMainColor();
	DWORD GetGridSubColor();
	DWORD GetGridXAxisColor();
	DWORD GetGridYAxisColor();
	DWORD GetCoordColor();
	DWORD GetCursorColor();
	*/

	DWORD GetTextColor(int type, int state);
	DWORD GetTextBkColor(int type, int state);

	DWORD GetLayerLineColor(int layer);
	DWORD GetLayerLineHighlightColor(int layer);
	DWORD GetLayerLineActiveColor(int layer);
	LineColorSet GetLayerLineColorSetByIndex( int layerID );

#undef _CMFDEF

	DWORD ARGBToABGR(DWORD col);
//	DWORD Highlight( DWORD col, int iHighlightLevel );

	void ARGBToAHSL(DWORD col, float *a, float *h, float *s, float *l);
	DWORD AHSLToARGB(float a, float h, float s, float l);
	float HueToRGB(float p, float q, float t);
};