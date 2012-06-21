#ifndef _HGE_PLATFORM_MACRO
#define _HGE_PLATFORM_MACRO

#define HPLATFORM_WIN32		0x00000001
#define HPLATFORM_WIN64		0x00000002
#define HPLATFORM_WIN		0x0000000f

#define HPLATFORM_IPHONE	0x00000010
#define HPLATFORM_IPAD		0x00000020
#define HPLATFORM_IOS		0x000000f0

#define HPLATFORM_MAC32		0x00000100
#define HPLATFORM_MAC64		0x00000200
#define HPLATFORM_MAC		0x00000f00

#define HPLATFORM_PSP		0x00001000

#define HRENDERSYS_DX	0x01000000
#define HRENDERSYS_GL	0x02000000
#define HRENDERSYS_GLES	0x04000000
#define HRENDERSYS_GLS	(HRENDERSYS_GL|HRENDERSYS_GLES)
#define HRENDERSYS_PSP	0x08000000

#define HFRAMEWORK_MFC	0x00100000
#define HFRAMEWORK_QT	0x00200000

/************************************************************************/
/* ToModify                                                             */
/************************************************************************/
#define HPLATFORM_DEFINE	HPLATFORM_WIN32
#define HRENDERYSY_DEFINE	HRENDERSYS_GL
#define HFRAMEWORK_DEFINE	HFRAMEWORK_QT
/************************************************************************/
/* End                                                                  */
/************************************************************************/

#define IF_PLATFORM(PLAT)		((PLAT)&(HPLATFORM_DEFINE))
#define IF_RENDERSYS(REND)		((REND)&(HRENDERYSY_DEFINE))
#define IF_FRAMWORK(FRAM)		((FRAM)&(HFRAMEWORK_DEFINE))

#define IFNOT_PLATFORM(PLAT)		(!IF_PLATFORM(PLAT))
#define IFNOT_RENDERSYS(REND)		(!IF_RENDERSYS(REND))
#define IFNOT_FRAMWORK(FRAM)		(!IF_FRAMWORK(FRAM))

#endif