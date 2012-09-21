#pragma once

#ifdef __RELEASE
 #define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge.dll"
#else
 #define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge_d.dll"
#endif

#ifndef __RELEASE
 #define DEBUGTEST
#endif

#define M_VERSION		0x0010
#define M_VERSIONSTR	"0.1"
#define M_SIGNATURE		"h5nc"

#define M_STRMAX	256

#define M_FLOATMIN	1.175494351e-38F
#define M_FLOATMAX	3.402823466e+38F

#define M_FLOATEPS	0.0001f
#define M_FLOATEXTREMEEPS	0.000001f

#define ANGLEBASE_90	90000
#define ANGLEBASE_180	180000
#define ANGLEBASE_270	270000
#define ANGLEBASE_360	360000
#define ARC(X)		((X) * 0.00001745329251994329f)
#define ANGLE(X)	(int)((X) * 57295.77951308232f)

#define M_ANGLEEPS	9

#define cost(X)	cosf(ARC(X))
#define sint(X)	sinf(ARC(X))

enum{
	QUADRANT_1,
	QUADRANT_2,
	QUADRANT_3,
	QUADRANT_4,

	QUADRANTMAX,
};

#define M_BEZIERCIRCLE_KAPPA	0.5522847498307933984022516322796f	// 4*(sqrt(2)-1)/3