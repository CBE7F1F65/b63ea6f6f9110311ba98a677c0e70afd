#pragma once

#ifndef _MAINDEPENDENCY_H
#define _MAINDEPENDENCY_H

#include "../../hge/include/hge.h"
#include "../../hge/include/hgefont.h"
#include "../../hge/include/hgesprite.h"
#include "../../hge/include/hgeeffectsystem.h"

#ifdef __WIN32
#include <windows.h>
#endif
#include <list>
#include <vector>
#include <stdio.h>
#include <algorithm>
#ifdef __WIN32
#include <assert.h>
#endif

#include "Const.h"

using namespace std;


#pragma warning(disable:4244)
#pragma warning(disable:4800)
#ifdef __WIN32
#pragma comment(lib, "winmm.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib, "hgehelp_d.lib")
#else
#pragma comment(lib, "hgehelp.lib")
#endif // _DEBUG

#ifdef _DEBUG
#pragma comment(lib, "hge_d.lib")
#else
#pragma comment(lib, "hge.lib")
#endif // _DEBUG

#ifdef __WIN32
//#pragma comment(lib, "kailleraclient.lib")
//#pragma comment(lib, "bass.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#endif

#endif