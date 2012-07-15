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

#define cost(X)	cosf(ARC(X))
#define sint(X)	sinf(ARC(X))

enum{
	QUADRANT_1,
	QUADRANT_2,
	QUADRANT_3,
	QUADRANT_4,
};

#define M_BEZIERCIRCLE_KAPPA	0.5522847498307933984022516322796f	// 4*(sqrt(2)-1)/3