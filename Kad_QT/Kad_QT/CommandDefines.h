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
	COMM_UNDO,		// UnDo
	COMM_REDO,		// ReDo

	_COMM_INSTANTCOMMANDEND,
	// Common Command

	// Node
	COMM_DELETEITEM,
	COMM_REPARENT,
	COMM_CLING,
	COMM_SETNODENAME,
	COMM_DUPLICATE,
	COMM_MOVENODE,
	COMM_MOVENODEBYOFFSET,
	COMM_MOVENODEBYOFFSET_BATCH,

	// Layers
	COMM_NEWLAYER,		// new layer
	COMM_NEWSUBLAYER,	// new sub layer
	COMM_SETWORKINGLAYER,	// set a working layer

	// Draw
	COMM_LINE,		// line
	COMM_BEZIER,	// bezier curve

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
	CSP_UNDO_I_STEP=0,
};

enum{
	CSI_UNDO_DUMMY=0,
	CSI_UNDO_WANTSTEP,
};

/************************************************************************/
/* PAN                                                                  */
/************************************************************************/

enum{
	CSP_PAN_XY_OFFSET = 0,
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
	CSP_ZOOMIN_XY_B = 0,
	CSP_ZOOMIN_XY_E,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSP_DOZOOM_XY_F_C_SCALE = 0,
};

/************************************************************************/
/* REPARENT                                                             */
/************************************************************************/

enum{
	CSP_REPARENT_I_NEWPARENTINDEX=0,
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
	CSP_CLING_I_FROMINDEX=0,
	CSP_CLING_I_TOINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_CLING_DUMMY=0,
	CSI_CLING_WANTFROMINDEX,
	CSI_CLING_WANTTOINDEX,
};

/************************************************************************/
/* DELETEITEM                                                           */
/************************************************************************/

enum{
	CSP_DELETEITEM_I_INDEXES=0,
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
	CSP_DUPLICATE_I_INDEXES=0,
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
	CSP_NEWLAYER_S_I_NAME_INDEX=0,
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
/* SETWORKINGLAYER                                                      */
/************************************************************************/

enum{
	CSP_SETWORKINGLAYER_I_NEWINDEX=0,
	CSP_SETWORKINGLAYER_I_LASTINDEX,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSPUNDO_SETWORKINGLAYER_I_LASTINDEX=0,
};
//////////////////////////////////////////////////////////////////////////
enum{
	CSI_SETWORKINGLAYER_DUMMY=0,
	CSI_SETWORKINGLAYER_WANTNEWINDEX,
	CSI_SETWORKINGLAYER_WANTLASTINDEX,
};

/************************************************************************/
/* LINE                                                                 */
/************************************************************************/

enum{
	CSP_LINE_XY_B = 0,
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
	CSP_BEZIER_XY_BA=0,
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
	CWP_XOFFSET,
	CWP_YOFFSET,
	CWP_HANDLEX,
	CWP_HANDLEY,
	CWP_SCALE,
	CWP_STEP,
	CWP_INDEX,
	CWP_NAME,

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

	_SSC_ENDINDEX,
};
/************************************************************************/
/* Index                                                                */
/************************************************************************/
#define COMMANDINDEXMAX			_COMM_ENDINDEX
#define COMMANDWANTPROMPTMAX	_CWP_ENDINDEX
#define COMMANDSUBINDEXMAX		_SSC_ENDINDEX