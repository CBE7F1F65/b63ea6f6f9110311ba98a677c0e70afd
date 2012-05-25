#include "stdafx.h"
#include "Command.h"

void Command::InitCommandStrInfo()
{
	ZeroMemory(scinfo, sizeof(scinfo));

#define _LSET(COMM, LSTR)	\
	strcpy_s(scinfo[COMM].str, COMMANDSTRINGMAX, LSTR)
#define _SSET(COMM, SSTR)	\
	strcpy_s(scinfo[COMM].shortstr, COMMANDSHORTSTRMAX, SSTR)
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
	ZeroMemory(wpinfo, sizeof(wpinfo));
#define _PSET(INDEX, STR)	\
	strcpy_s(wpinfo[INDEX].str, M_STRMAX, STR)

	_PSET(CWP_X, "X: ");
	_PSET(CWP_Y, "Y: ");
	_PSET(CWP_XY, "XY: ");
	_PSET(CWP_XOFFSET, "X offset: ");
	_PSET(CWP_YOFFSET, "Y offset: ");
	_PSET(CWP_SCALE, "Scale: ");

#undef _PSET
}