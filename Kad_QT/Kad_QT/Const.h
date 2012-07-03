#pragma once

#ifdef __RELEASE
 #define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge.dll"
#else
 #define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge_d.dll"
#endif

#ifndef __RELEASE
 #define DEBUGTEST
#endif

#define M_STRMAX	256

#define M_FLOATEPS	0.0001f

#define ANGLEBASE_90	90000
#define ANGLEBASE_180	180000
#define ANGLEBASE_270	270000
#define ANGLEBASE_360	360000
#define ARC(X)		((X) * 0.00001745329251994329f)
#define ANGLE(X)	(int)((X) * 57295.77951308232f)