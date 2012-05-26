#include "stdafx.h"
#include "Command.h"

void Command::InitCommandStrInfo()
{
//	ZeroMemory(scinfo, sizeof(scinfo));

//	strcpy_s(scinfo[COMM].str, COMMANDSTRINGMAX, LSTR)
#define _LSET(COMM, LSTR)	\
	scinfo[COMM].str = LSTR
//	strcpy_s(scinfo[COMM].shortstr, COMMANDSHORTSTRMAX, SSTR)
#define _SSET(COMM, SSTR)	\
	scinfo[COMM].shortstr =  SSTR
#define _BSET(COMM, LSTR, SSTR)	\
	_LSET(COMM, LSTR);	\
	_SSET(COMM, SSTR)

	_LSET(COMM_PAN, "pan");
	_LSET(COMM_ZOOMIN, "zoomin");
	_LSET(COMM_DOZOOM, "zoom");

	_BSET(COMM_LINE, "line", "l");
	_BSET(COMM_BEZIER, "bezier", "b");

#undef _LSET
#undef _SSET
#undef _BSET
}

void Command::InitWantPromptInfo()
{
//	ZeroMemory(wpinfo, sizeof(wpinfo));

//	strcpy_s(wpinfo[INDEX].str, M_STRMAX, STR)
#define _PSET(INDEX, STR)	\
	wpinfo[INDEX].str = STR

	_PSET(CWP_X, "X: ");
	_PSET(CWP_Y, "Y: ");
	_PSET(CWP_XY, "X, Y: ");
	_PSET(CWP_X_B, "BEGIN X: ");
	_PSET(CWP_Y_B, "BEGIN Y: ");
	_PSET(CWP_XY_B, "BEGIN X, Y: ");
	_PSET(CWP_X_E, "END X: ");
	_PSET(CWP_Y_E, "END Y: ");
	_PSET(CWP_XY_E, "END X, Y: ");
	_PSET(CWP_XOFFSET, "X OFFSET: ");
	_PSET(CWP_YOFFSET, "Y OFFSET: ");
	_PSET(CWP_SCALE, "SCALE: ");

#undef _PSET
}