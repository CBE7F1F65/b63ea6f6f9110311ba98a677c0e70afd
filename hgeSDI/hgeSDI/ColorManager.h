#pragma once

#include "MainDependency.h"
#include "StringManager.h"

enum{
	COLORMT_LIST=0,

	_COLORMT_ENDINDEX,
};

enum{
	COLORMS_ACTIVE=0,
	COLORMS_NONACTIVE,
	COLORMS_EDITABLE,
	COLORMS_DISABLED,

	_COLORMS_ENDINDEX,
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
	
#define _FDEF(COLNAME, DEF)	\
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

	_FDEF(	BG,	0xff212830	);
	_FDEF(	GridMain,	0xff333949	);
	_FDEF(	GridSub,	0xff272d38	);
	_FDEF(	GridXAxis,	0xff6d2426	);
	_FDEF(	GridYAxis,	0xff236e26	);
	_FDEF(	Coord,	0xffffffff	);
	_FDEF(	Cursor,	0xffffffff	);

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

	DWORD GetLayerLineColor(int layer=-1);

#undef _FDEF

	DWORD ARGBToABGR(DWORD col);

};

