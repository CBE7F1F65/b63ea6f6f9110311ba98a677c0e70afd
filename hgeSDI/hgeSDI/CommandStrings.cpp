#include "stdafx.h"
#include "Command.h"

void Command::InitCommandStrInfo()
{
#define _LSET(COMM, LSTR)	\
	scinfo[COMM].str = LSTR
#define _SSET(COMM, SSTR)	\
	scinfo[COMM].shortstr =  SSTR
#define _BSET(COMM, LSTR, SSTR)	\
	_LSET(COMM, LSTR);	\
	_SSET(COMM, SSTR)

	_LSET(COMM_PAN, "PAN");
	_LSET(COMM_ZOOMIN, "ZOOMIN");
	_LSET(COMM_DOZOOM, "ZOOM");

	_BSET(COMM_LINE, "LINE", "L");
	_BSET(COMM_BEZIER, "BEZIER", "B");

#undef _LSET
#undef _SSET
#undef _BSET
}

void Command::InitWantPromptInfo()
{
#define _PSET(INDEX, STR)	\
	wpinfo[INDEX].str = STR

	_PSET(CWP_X, "X: ");
	_PSET(CWP_Y, "Y: ");
	_PSET(CWP_XY, "X, Y: ");
	_PSET(CWP_X_B, "BEGIN X: ");
	_PSET(CWP_Y_B, "BEGIN Y: ");
	_PSET(CWP_XY_B, "BEGIN X, Y: ");
	_PSET(CWP_X_N, "NEXT X: ");
	_PSET(CWP_Y_N, "NEXT Y: ");
	_PSET(CWP_XY_N, "NEXT X, Y: ");
	_PSET(CWP_X_E, "END X: ");
	_PSET(CWP_Y_E, "END Y: ");
	_PSET(CWP_XY_E, "END X, Y: ");
	_PSET(CWP_XOFFSET, "X OFFSET: ");
	_PSET(CWP_YOFFSET, "Y OFFSET: ");
	_PSET(CWP_SCALE, "SCALE: ");

#undef _PSET
}

void Command::InitSubCommandStrInfo()
{
	string tstr;
#define _SUBSET(INDEX, PROMPTSTR, STR)	\
	subcinfo[INDEX].str = STR;	\
	tstr = PROMPTSTR;	\
	tstr += " (";	\
	tstr += STR;	\
	tstr += ")";	\
	subcinfo[INDEX].promptstr = tstr


	_SUBSET(SSC_UNDO,			"UnDo", "U");
	_SUBSET(SSC_REDO,			"ReDo", "R");
	_SUBSET(SSC_FINISH,			"Finish", "F");
	_SUBSET(SSC_TERMINAL,		"Terminal", "T");
}