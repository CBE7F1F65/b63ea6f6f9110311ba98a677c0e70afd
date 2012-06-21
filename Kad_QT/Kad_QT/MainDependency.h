#pragma once

#ifndef _MAINDEPENDENCY_H
#define _MAINDEPENDENCY_H

#include "../../hge/include/hge.h"
#include "../../hge/include/hgefont.h"
#include "../../hge/include/hgesprite.h"
#include "../../hge/include/hgeeffectsystem.h"

#if IF_PLATFORM(HPLATFORM_WIN)
#include <windows.h>
#endif

#include <list>
#include <stdio.h>
#include <algorithm>

#if IF_PLATFORM(HPLATFORM_WIN)
#include <assert.h>
#endif

#include "Const.h"

using namespace std;

#pragma warning(disable:4244)
#pragma warning(disable:4800)
#if IF_PLATFORM(HPLATFORM_WIN)
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

#if IF_RENDERSYS(HRENDERSYS_DX)
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#endif

#endif