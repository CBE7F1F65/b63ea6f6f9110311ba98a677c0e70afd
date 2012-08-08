#pragma once

#define COMMANDSTRINGMAX	0x20
#define COMMANDSHORTSTRMAX	0x10

#define CI_MAKESUBEX(PCOUNT, UCOUNT)	((PCOUNT)+((UCOUNT)<<8))
#define CI_GETPCOUNT(SUBEX)				((SUBEX)&0xff)
#define CI_GETUCOUNT(SUBEX)				((SUBEX)>>8)

enum{
	COMM_NULL = 0,

	_COMM_NORMALCOMMANDBEGIN,

	_COMM_PUSHCOMMANDBEGIN,

	// Push
	COMM_PAN,		// enter pan mode
	COMM_ZOOMIN,	// rect zoom

	_COMM_NOLOGCOMMANDBEGIN,
	// No log
	COMM_DOZOOM,	// mouse wheel

	_COMM_NOLOGCOMMANDEND,

	COMM_OUTPUT,	// output node structure

	_COMM_PUSHCOMMANDEND,

	_COMM_INSTANTCOMMANDBEGIN,

	// Instant Commands
	COMM_INITIAL,	// do nothing
	COMM_TERMINAL,	// do nothing
	COMM_UNDO,		// UnDo
	COMM_REDO,		// ReDo

	_COMM_INSTANTCOMMANDEND,
	// Common Command

	// Test
#ifdef DEBUGTEST
	COMM_TEST,
#endif // DEBUGTEST
	
	// Node
	COMM_DELETEITEM,
	COMM_REPARENT,
	COMM_CLING,
	COMM_BINDHANDLE,
	COMM_MERGE,
	COMM_DEMERGE,
	COMM_SETNODENAME,
	COMM_DUPLICATE,
	COMM_MOVENODE,
	COMM_MOVENODEBYOFFSET,
	COMM_MOVENODEBYOFFSET_BATCH,
	COMM_ROTATENODE_BATCH,
	COMM_SCALENODE_BATCH,
	COMM_FLIPNODE_BATCH,

	COMM_EXTEND,

	// Transform
	COMM_TOBEZIER,
	COMM_TOSTRAIGHTLINE,
	COMM_SWAPBEGINEND,

	COMM_CLIP,
	COMM_JOIN,

	// Layers
	COMM_NEWLAYER,		// new layer
	COMM_NEWSUBLAYER,	// new sub layer
	COMM_SETWORKINGLAYER,	// set a working layer

	// Draw
	COMM_LINE,		// line
	COMM_BEZIER,	// bezier curve
	COMM_CIRCLE,	// circle

	// Other
	COMM_RESTORETOSNAPSHOT,

	_COMM_NORMALCOMMANDEND,
	_COMM_INTERNALBEGIN,

	COMM_I_COMMAND,
	COMM_I_COMMAND_AUTO,
	COMM_I_COMMAND_ENDMARK,
	COMM_I_UNDO_COMMIT,
	COMM_I_UNDO_PARAM,
	COMM_I_UNDO_PARAMFROMCOMMAND,
	COMM_I_ADDNODE,
	COMM_I_DELETENODE,
	COMM_I_REPARENTNODE,
	COMM_I_COMM_WORKINGLAYER,

	_COMM_INTERNALEND,

	_COMM_ENDINDEX,
};

#define COMMPARAMFLAG_X		0x0001
#define COMMPARAMFLAG_Y		0x0002
//#define COMMPARAMFLAG_XY	0x0003
#define COMMPARAMFLAG_F		0x0010
#define COMMPARAMFLAG_I		0x0020
#define COMMPARAMFLAG_S		0x0040
#define COMMPARAMFLAG_G		0x0100

#define COMMPARAMMAX	16
#define CSI_INIT			0
#define CSI_TERMINAL		-1
#define CSI_FINISH			-2
#define CSI_FINISHCONTINUE	-3
#define CSI_PAUSE			-4
#define CSI_RESUME			-5

// CSP Param
// CSPUNDO UnDo Param
// CSI Step

/************************************************************************/
/* UnDo                                                                 */
/************************************************************************/
enum{
	CSP_UNDO_I_STEP,
};

enum{
	CSI_UNDO_DUMMY=0,
	CSI_UNDO_WANTSTEP,
};

/************************************************************************/
/* PAN                                                                  */
/************************************************************************/

enum{
	CSP_PAN_XY_OFFSET,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_PAN_DUMMY=0,
	CSI_PAN_READY,
};

/************************************************************************/
/* ZOOM                                                                 */
/************************************************************************/

enum{
	CSP_ZOOMIN_XY_B,
	CSP_ZOOMIN_XY_E,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSP_DOZOOM_XY_F_C_SCALE,
};

/************************************************************************/
/* REPARENT                                                             */
/************************************************************************/

enum{
	CSP_REPARENT_I_NEWPARENTINDEX,
	CSP_REPARENT_I_AFTERINDEX,
	CSP_REPARENT_I_OLDINDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_REPARENT_DUMMY=0,
	CSI_REPARENT_WANTNEWPARENTINDEX,
	CSI_REPARENT_WANTAFTERINDEX,
	CSI_REPARENT_WANTOLDINDEXES,
};

/************************************************************************/
/* CLING                                                                */
/************************************************************************/

enum{
	CSP_CLING_I_FROMINDEX,
	CSP_CLING_I_TOINDEX,
	CSP_CLING_I_F_TYPE_VALUE,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_CLING_DUMMY=0,
	CSI_CLING_WANTFROMINDEX,
	CSI_CLING_WANTTOINDEX,
	CSI_CLING_WANTCLINGTYPE,
	CSI_CLING_WANTVAL,
};

/************************************************************************/
/* BINDHANDLE                                                           */
/************************************************************************/

enum{
	CSP_BINDHANDLE_I_FROMINDEX,
	CSP_BINDHANDLE_I_TOINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_BINDHANDLE_DUMMY=0,
	CSI_BINDHANDLE_WANTFROMINDEX,
	CSI_BINDHANDLE_WANTTOINDEX,
};

/************************************************************************/
/* MERGE                                                                */
/************************************************************************/

enum{
	CSP_MERGE_I_FROMINDEX,
	CSP_MERGE_I_TOINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_MERGE_DUMMY=0,
	CSI_MERGE_WANTFROMINDEX,
	CSI_MERGE_WANTTOINDEX,
};

/************************************************************************/
/* DEMERGE                                                              */
/************************************************************************/

enum{
	CSP_DEMERGE_I_FROMINDEX,
	CSP_DEMERGE_I_TOINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_DEMERGE_DUMMY=0,
	CSI_DEMERGE_WANTFROMINDEX,
	CSI_DEMERGE_WANTTOINDEX,
};
/************************************************************************/
/* DELETEITEM                                                           */
/************************************************************************/

enum{
	CSP_DELETEITEM_I_INDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_DELETEITEM_DUMMY=0,
	CSI_DELETEITEM_WANTINDEXES,
};

/************************************************************************/
/* DUPLICATE                                                            */
/************************************************************************/

enum{
	CSP_DUPLICATE_I_INDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_DUPLICATE_DUMMY=0,
	CSI_DUPLICATE_WANTINDEXES,
};

/************************************************************************/
/* NEWLAYER                                                             */
/************************************************************************/

enum{
	CSP_NEWLAYER_S_I_NAME_INDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_NEWLAYER_DUMMY=0,
	CSI_NEWLAYER_WANTNAME,
	CSI_NEWLAYER_WANTINDEX,
};

/************************************************************************/
/* SETNODENAME                                                         */
/************************************************************************/

enum{
	CSP_SETNODENAME_I_S_INDEX_NEWNAME,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_SETNODENAME_DUMMY=0,
	CSI_SETNODENAME_WANTINDEX,
	CSI_SETNODENAME_WANTNEWNAME,
};

/************************************************************************/
/* MOVENODE                                                             */
/************************************************************************/

enum{
	CSP_MOVENODE_I_XY_INDEX_NEWPOS,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_MOVENODE_DUMMY=0,
	CSI_MOVENODE_WANTINDEX,
	CSI_MOVENODE_WANTX,
	CSI_MOVENODE_WANTY,
};

/************************************************************************/
/* MOVENODE_BATCH                                                       */
/************************************************************************/

enum{
	CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_MOVENODE_BATCH_DUMMY=0,
	CSI_MOVENODE_BATCH_WANTX,
	CSI_MOVENODE_BATCH_WANTY,
	CSI_MOVENODE_BATCH_WANTINDEXES,
};

/************************************************************************/
/* ROTATENODE_BATCH                                                     */
/************************************************************************/

enum{
	CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN,
	CSP_ROTATENODE_BATCH_I_INDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_ROTATENODE_BATCH_DUMMY=0,
	CSI_ROTATENODE_BATCH_WANTX,
	CSI_ROTATENODE_BATCH_WANTY,
	CSI_ROTATENODE_BATCH_WANTANGLE,
	CSI_ROTATENODE_BATCH_WANTINDEXES,
};

/************************************************************************/
/* SCALENODE_BATCH                                                      */
/************************************************************************/

enum{
	CSP_SCALENODE_BATCH_XY_ORIGIN,
	CSP_SCALENODE_BATCH_XY_SCALE,
	CSP_SCALENODE_BATCH_I_INDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_SCALENODE_BATCH_DUMMY=0,
	CSI_SCALENODE_BATCH_WANTX,
	CSI_SCALENODE_BATCH_WANTY,
	CSI_SCALENODE_BATCH_WANTXSCALE,
	CSI_SCALENODE_BATCH_WANTYSCALE,
	CSI_SCALENODE_BATCH_WANTINDEXES,
};

/************************************************************************/
/* FLIPNODE_BATCH                                                       */
/************************************************************************/

enum{
	CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN,
	CSP_FLIPNODE_BATCH_I_INDEXES,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_FLIPNODE_BATCH_DUMMY=0,
	CSI_FLIPNODE_BATCH_WANTX,
	CSI_FLIPNODE_BATCH_WANTY,
	CSI_FLIPNODE_BATCH_WANTANGLE,
	CSI_FLIPNODE_BATCH_WANTINDEXES,
};

/************************************************************************/
/* EXTEND                                                               */
/************************************************************************/

enum{
	CSP_EXTEND_I_F_INDEX_BEGINOFFSET,
	CSP_EXTEND_F_ENDOFFSET,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_EXTEND_DUMMY=0,
	CSI_EXTEND_WANTINDEX,
	CSI_EXTEND_WANTBEGINOFFSET,
	CSI_EXTEND_WANTENDOFFSET,
};

/************************************************************************/
/* SETWORKINGLAYER                                                      */
/************************************************************************/

enum{
	CSP_SETWORKINGLAYER_I_NEWINDEX,
	CSP_SETWORKINGLAYER_I_LASTINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSPUNDO_SETWORKINGLAYER_I_LASTINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_SETWORKINGLAYER_DUMMY=0,
	CSI_SETWORKINGLAYER_WANTNEWINDEX,
	CSI_SETWORKINGLAYER_WANTLASTINDEX,
};

/************************************************************************/
/* LINETRANSFORM                                                        */
/************************************************************************/

enum{
	CSP_LINETRANSFORM_I_INDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_LINETRANSFORM_DUMMY=0,
	CSI_LINETRANSFORM_WANTNEWINDEX,
};

/************************************************************************/
/* CLIP                                                                 */
/************************************************************************/

enum{
	CSP_CLIP_I_F_INDEX_PROPORTION,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_CLIP_DUMMY=0,
	CSI_CLIP_WANTINDEX,
	CSI_CLIP_WANTPROPORTION,
};

/************************************************************************/
/* JOIN                                                                 */
/************************************************************************/

enum{
	CSP_JOIN_I_INDEX,
	CSP_JOIN_I_DELETEINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_JOIN_DUMMY=0,
	CSI_JOIN_WANTINDEX,
	CSI_JOIN_WANTDELETEINDEX,
};

/************************************************************************/
/* LINE                                                                 */
/************************************************************************/

enum{
	CSP_LINE_XY_B,
	CSP_LINE_XY_N,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_LINE_DUMMY=0,
	CSI_LINE_WANTX1,
	CSI_LINE_WANTY1,
	CSI_LINE_WANTX2,
	CSI_LINE_WANTY2,
};

/************************************************************************/
/* BEZIER                                                               */
/************************************************************************/

enum{
	CSP_BEZIER_XY_BA,
	CSP_BEZIER_XY_BH,
	CSP_BEZIER_XY_NA,
	CSP_BEZIER_XY_NH,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_BEZIER_DUMMY=0,
	CSI_BEZIER_WANTBAX,
	CSI_BEZIER_WANTBAY,
	CSI_BEZIER_WANTBHX,
	CSI_BEZIER_WANTBHY,
	CSI_BEZIER_WANTNAX,
	CSI_BEZIER_WANTNAY,
	CSI_BEZIER_WANTNHX,
	CSI_BEZIER_WANTNHY,
};

/************************************************************************/
/* CIRCLE                                                               */
/************************************************************************/

enum{
	CSP_CIRCLE_XY_F_CENTER_R,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_CIRCLE_DUMMY=0,
	CSI_CIRCLE_WANTX,
	CSI_CIRCLE_WANTY,
	CSI_CIRCLE_WANTR,
};

/************************************************************************/
/* RESTORETOSNAPSHOT                                                    */
/************************************************************************/

enum{
	CSP_RESTORETOSNAPSHOT_I_INDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_RESTORETOSNAPSHOT_DUMMY,
	CSI_RESTORETOSNAPSHOT_WANTINDEX,
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
/************************************************************************/
/* Prompt                                                               */
/************************************************************************/

// CWP Prompt

enum{
	CWP_NULL=0,
	CWP_X,
	CWP_Y,
	CWP_XY,
	CWP_X_B,
	CWP_Y_B,
	CWP_XY_B,
	CWP_X_E,
	CWP_Y_E,
	CWP_XY_E,
	CWP_X_N,
	CWP_Y_N,
	CWP_XY_N,
	CWP_R,
	CWP_BEGINOFFSET,
	CWP_ENDOFFSET,
	CWP_XOFFSET,
	CWP_YOFFSET,
	CWP_XSCALE,
	CWP_YSCALE,
	CWP_HANDLEX,
	CWP_HANDLEY,
	CWP_SCALE,
	CWP_STEP,
	CWP_INDEX,
	CWP_NAME,
	CWP_TYPE,
	CWP_VALUE,
	CWP_PROPORTION,
	CWP_ANGLE,

	_CWP_ENDINDEX,
};

/************************************************************************/
/* Sub                                                                  */
/************************************************************************/

// SSC Sub

enum{
	SSC_NULL=0,
	SSC_UNDO,
	SSC_REDO,
	SSC_FINISH,
	SSC_TERMINAL,
	SSC_POINT,
	SSC_LINE,

	_SSC_ENDINDEX,
};
/************************************************************************/
/* Index                                                                */
/************************************************************************/
#define COMMANDINDEXMAX			_COMM_ENDINDEX
#define COMMANDWANTPROMPTMAX	_CWP_ENDINDEX
#define COMMANDSUBINDEXMAX		_SSC_ENDINDEX