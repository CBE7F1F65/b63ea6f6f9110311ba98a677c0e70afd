#pragma once

#ifdef __RELEASE
#define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge.dll"
#else
#define DEFAULT_HGEDLLPATH	"../../hge/lib/vc/hge_d.dll"
#endif

#define M_STRMAX	256

#define ANGLEBASE_90	90000
#define ARC(X)		((X) * 0.00001745329251994329f)
#define ANGLE(X)	(int)((X) * 57295.77951308232f)