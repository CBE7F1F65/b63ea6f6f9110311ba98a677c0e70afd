#include "StdAfx.h"
#include "StringManager.h"

StringManager::StringManager(void)
{
	hge = hgeCreate(HGE_VERSION);
}

StringManager::~StringManager(void)
{
	if (hge)
	{
		hge->Release();
	}
}

void StringManager::Init()
{
	FillSCInfo();
	FillWPInfo();
	FillSubSCInfo();
}

const char * StringManager::GetCommandPanelTabName( int ntab )
{
	string str = "Tab ";
	str += ntab;
	return str.c_str();
}

const char * StringManager::GetCommandPanelName()
{
	return "Command";
}

const char * StringManager::GetHistoryPanelName()
{
	return "History";
}

const char * StringManager::GetLayerPanelName()
{
	return "Layer";
}

const char * StringManager::GetStatusPaneLeftName()
{
	return "Status";
}

const char * StringManager::GetStatusPaneRightName()
{
	return "FPS";
}

const char * StringManager::GetAppIDName()
{
	return "h5nc.AppID.CurrentVersion";
}

const char * StringManager::GetRegistryKeyName()
{
	return "h5nc.Application";
}

const char * StringManager::GetLogFileName()
{
	return "Log.log";
}

const char * StringManager::GetIniFileName()
{
	return "Config.ini";
}

void StringManager::FillSCInfo()
{
#define _LSET(COMM, LSTR, DES, CMT)	\
	scinfo[COMM].str = LSTR;	\
	scinfo[COMM].description = hge->Ini_GetString(INIS_COMMAND_PREFIX LSTR, ININ_DES, DES);	\
	scinfo[COMM].comment = hge->Ini_GetString(INIS_COMMAND_PREFIX LSTR, ININ_COMMENT, CMT)
#define _SSET(COMM, SSTR)	\
	scinfo[COMM].shortstr =  hge->Ini_GetString(INIS_COMMAND_PREFIX SSTR, ININ_SSTR, SSTR)
#define _BSET(COMM, LSTR, SSTR, DES, CMT)	\
	_LSET(COMM, LSTR, DES, CMT);	\
	_SSET(COMM, SSTR)

	// Do not use _SSET, _LSET directly
	// Keep format for Excel Editing
	_BSET(	COMM_PAN,	"PAN",	"",	"Pan",	""	);
	_BSET(	COMM_ZOOMIN,	"ZOOMIN",	"",	"Zone Zoom",	""	);
	_BSET(	COMM_DOZOOM,	"ZOOM",	"",	"Instant Zoom",	""	);
	_BSET(	COMM_OUTPUT,	"OUTPUT",	"O",	"Output",	""	);

	_BSET(	COMM_INITIAL,	"INITIAL",	"",	"Initialization",	""	);
	_BSET(	COMM_UNDO,	"UNDO", "U",	"UnDo",	""	);
	_BSET(	COMM_REDO,	"REDO", "R",	"UnDo",	""	);

	_BSET(	COMM_REPARENT,	"REPARENT",	"",	"Reparent Item",	""	);
	_BSET(	COMM_DELETEITEM,	"DELETE",	"DEL",	"Delete Item",	""	);
	_BSET(	COMM_SETNODENAME,	"SETNODENAME",	"",	"Set Display Name",	""	);
	_BSET(	COMM_DUPLICATE,	"DUPLICATE",	"DUP",	"Duplicate Item",	""	);
	_BSET(	COMM_MOVENODE,	"MOVE",	"M",	"Move Node",	""	);
	_BSET(	COMM_MOVENODEBYOFFSET,	"MOVEBYOFFSET",	"MOFF",	"Move Node Offset",	""	);
	_BSET(	COMM_MOVENODEBYOFFSET_BATCH,	"MOVEBYOFFSET_BATCH",	"MOFF_BATCH",	"Move Node Offset Batch",	""	);

	_BSET(	COMM_SETWORKINGLAYER,	"SETWORKINGLAYER",	"",	"Set Working Layer",	""	);
	_BSET(	COMM_NEWLAYER,	"NEWLAYER", "NL",	"New Layer",	""	);
	_BSET(	COMM_NEWSUBLAYER,	"NEWSUBLAYER",	"NSL",	"New Sub Layer", ""	);

	_BSET(	COMM_LINE,	"LINE",	"L",	"Draw Line",	""	);
	_BSET(	COMM_BEZIER,	"BEZIER",	"B",	"Draw Bezier Curve",	""	);

#undef _SSET
#undef _LSET
#undef _BSET
}

void StringManager::FillWPInfo()
{
#define _PSET(INDEX, STR)	\
	wpinfo[INDEX].str = hge->Ini_GetString(INIS_CWP, STR, STR)

	// Keep format for Excel Editing
	_PSET(	CWP_X,	"X"	);
	_PSET(	CWP_Y,	"Y"	);
	_PSET(	CWP_XY,	"X, Y"	);
	_PSET(	CWP_X_B,	"BEGIN X"	);
	_PSET(	CWP_Y_B,	"BEGIN Y"	);
	_PSET(	CWP_XY_B,	"BEGIN X, Y"	);
	_PSET(	CWP_X_N,	"NEXT X"	);
	_PSET(	CWP_Y_N,	"NEXT Y"	);
	_PSET(	CWP_XY_N,	"NEXT X, Y"	);
	_PSET(	CWP_X_E,	"END X"	);
	_PSET(	CWP_Y_E,	"END Y"	);
	_PSET(	CWP_XY_E,	"END X, Y"	);
	_PSET(	CWP_XOFFSET,	"X OFFSET"	);
	_PSET(	CWP_YOFFSET,	"Y OFFSET"	);
	_PSET(	CWP_HANDLEX,	"HANDLE X"	);
	_PSET(	CWP_HANDLEY,	"HANDLE Y"	);
	_PSET(	CWP_SCALE,	"SCALE"	);
	_PSET(	CWP_STEP,	"STEP"	);
	_PSET(	CWP_INDEX,	"INDEX"	);
	_PSET(	CWP_NAME,	"NAME"	);

#undef _PSET
}

void StringManager::FillSubSCInfo()
{
	string tstr;
#define _SUBSET(INDEX, PROMPTSTR, STR, DES)	\
	subcinfo[INDEX].str = STR;	\
	subcinfo[INDEX].description = hge->Ini_GetString(INIS_SSC_PREFIX PROMPTSTR, ININ_DES, DES);	\
	tstr = hge->Ini_GetString(INIS_SSC_PREFIX PROMPTSTR, ININ_PROMPT, PROMPTSTR);	\
	tstr += " (";	\
	tstr += STR;	\
	tstr += ")";	\
	subcinfo[INDEX].promptstr = tstr

	// Keep format for Excel Editing
	_SUBSET(	SSC_UNDO,	"UnDo",	"U",	""	);
	_SUBSET(	SSC_REDO,	"ReDo",	"R",	""	);
	_SUBSET(	SSC_FINISH,	"Finish",	"F",	""	);
	_SUBSET(	SSC_TERMINAL,	"Terminal",	"T",	""	);

#undef _SUBSET
}