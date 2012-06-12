#pragma once

#define COMMANDSTRINGMAX	0x20
#define COMMANDSHORTSTRMAX	0x10

#define CI_MAKESUBEX(PCOUNT, UCOUNT)	((PCOUNT)+((UCOUNT)<<8))
#define CI_GETPCOUNT(SUBEX)				((SUBEX)&0xff)
#define CI_GETUCOUNT(SUBEX)				((SUBEX)>>8)

enum{
	COMM_NULL = 0,

	_COMM_PUSHCOMMANDBEGIN,

	// Push
	COMM_PAN,		// enter pan mode
	COMM_ZOOMIN,	// rect zoom

	_COMM_NOLOGCOMMANDBEGIN,
	// No log
	COMM_DOZOOM,	// mouse wheel

	_COMM_NOLOGCOMMANDEND,

	_COMM_PUSHCOMMANDEND,

	_COMM_INSTANTCOMMANDBEGIN,

	// Instant Commands
	COMM_INITIAL,	// do nothing

	_COMM_INSTANTCOMMANDEND,
	// Common Command
	// Layers
	COMM_NEWLAYER,		// new layer
	COMM_NEWSUBLAYER,	// new sub layer
	COMM_SETWORKINGLAYER,	// set a working layer

	// Draw
	COMM_LINE,		// line
	COMM_BEZIER,	// bezier curve

	_COMM_INTERNALBEGIN,

	COMM_I_COMMAND,
	COMM_I_UNDO_COMMIT,
	COMM_I_UNDO_PARAM,
	COMM_I_ADDNODE,
	COMM_I_DELETENODE,
	COMM_I_REPARENTNODE,

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
// CSI Step

enum{
	CSP_PAN_OFFSET_XY = 0,
};

enum{
	CSI_PAN_DUMMY=0,
	CSI_PAN_READY,
};

enum{
	CSP_ZOOMIN_B_XY = 0,
	CSP_ZOOMIN_E_XY,
};

enum{
	CSP_DOZOOM_C_XY_SCALE = 0,
};

enum{
	CSP_NEWLAYER_S_I_NAME_INDEX=0,
};

enum{
	CSI_NEWLAYER_DUMMY=0,
	CSI_NEWLAYER_WANTNAME,
	CSI_NEWLAYER_WANTINDEX,
};

enum{
	CSP_SETWORKINGLAYER_I_LASTINDEX=0,
	CSP_SETWORKINGLAYER_I_NEWINDEX,
};

enum{
	CSI_SETWORKINGLAYER_DUMMY=0,
	CSI_SETWORKINGLAYER_WANTLASTINDEX,
	CSI_SETWORKINGLAYER_WANTNEWINDEX,
};

enum{
	CSI_LINE_DUMMY=0,
	CSI_LINE_WANTX1,
	CSI_LINE_WANTY1,
	CSI_LINE_WANTX2,
	CSI_LINE_WANTY2,
};

enum{
	CSP_LINE_B_XY = 0,
	CSP_LINE_N_XY,
};

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
	CWP_SCALE,

	_CWP_ENDINDEX,
};

// SSC Sub

enum{
	SSC_NULL=0,
	SSC_UNDO,
	SSC_REDO,
	SSC_FINISH,
	SSC_TERMINAL,

	_SSC_ENDINDEX,
};

#define COMMANDINDEXMAX			_COMM_ENDINDEX
#define COMMANDWANTPROMPTMAX	_CWP_ENDINDEX
#define COMMANDSUBINDEXMAX		_SSC_ENDINDEX