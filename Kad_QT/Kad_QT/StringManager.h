#pragma once

#include "MainDependency.h"
#include "CommandDefines.h"

class CommandStrInfo
{
public:
	CommandStrInfo()
	{
		ClearSet();
	};
	~CommandStrInfo(){};

	void ClearSet()
	{
		str = "";
		shortstr = "";
		description = "";
		comment = "";
	};

	string str;
	string shortstr;
	string description;
	string comment;
};

class SubCommandStrInfo{
public:
	SubCommandStrInfo()
	{
		ClearSet();
	};
	~SubCommandStrInfo(){};

	void ClearSet()
	{
		promptstr = "";
		str = "";
		description = "";
	};

	string promptstr;
	string str;
	string description;
};

class WantPromptInfo
{
public:
	WantPromptInfo()
	{
		ClearSet();
	};
	~WantPromptInfo(){};

	void ClearSet()
	{
		str = "";
	};

	string str;
};

#define INIS_NODENAME		"NodeName"
#define INIS_COMMANDLOG		"CommandLog"

#define INIS_MESSAGEBOX		"MessageBox"

#define INIS_TOOLTIP		"ToolTip"
#define INIS_DEFAULTSTR		"DefaultString"
#define INIS_NODEINFO		"NodeInfo"

#define INIS_CWP			"WPrompt"
#define INIS_COMMAND_PREFIX		"Command_"
#define INIS_SSC_PREFIX			"SubCommand_"

#define ININ_STR		"Name"
#define ININ_SSTR		"Short"
#define ININ_DES		"Description"
#define ININ_PROMPT		"Prompt"
#define ININ_COMMENT	"Comment"

#define INIS_PERFORMANCE	"Performance"
#define ININ_UNDOMAX	"UnDoMax"
#define INID_UNDOMAX	100

#define INIS_COLOR	"Color"

class StringManager
{
public:
	static StringManager& getInstance() { static StringManager instance; return instance; }

private:
	StringManager();
	~StringManager();
	StringManager(StringManager const&);
	void operator=(StringManager const&);

public:

	void Init();

	const char * GetCommandPanelTabName(int ntab);
	const char * GetCommandPanelName();
	const char * GetHistoryPanelName();
	const char * GetLayerPanelName();
	const char * GetStatusPaneLeftName();
	const char * GetStatusPaneRightName();
	const char * GetAppIDName();
	const char * GetRegistryKeyName();
	const char * GetRegistryGeometryName();
	const char * GetRegistryWindowStateName();
	const char * GetLogFileName();
	const char * GetIniFileName();

	const char * GetNodeInfoClingTypeName(int nType);
	const char * GetNodeInfoChangeClingTypeName(int nType);

	HGE * hge;

	CommandStrInfo scinfo[COMMANDINDEXMAX];
	WantPromptInfo wpinfo[COMMANDWANTPROMPTMAX];
	SubCommandStrInfo subcinfo[COMMANDSUBINDEXMAX];

	void FillSCInfo();
	void FillWPInfo();
	void FillSubSCInfo();

	const char * GetCommandStrName(int command)
	{
		return scinfo[command].str.c_str();
	};
	const char * GetCommandShortStrName(int command)
	{
		return scinfo[command].shortstr.c_str();
	};
	const char * GetCommandDescriptionName(int command)
	{
		return scinfo[command].description.c_str();
	};
	const char * GetCommandCommentName(int command)
	{
		return scinfo[command].comment.c_str();
	};
	const char * GetWantPromptName(int cwp)
	{
		return wpinfo[cwp].str.c_str();
	};
	const char * GetSubCommandStrName(int ssc)
	{
		return subcinfo[ssc].str.c_str();
	};
	const char * GetSubCommandPromptStrName(int ssc)
	{
		return subcinfo[ssc].promptstr.c_str();
	};
	const char * GetSubCommandDescriptionName(int ssc)
	{
		return subcinfo[ssc].description.c_str();
	};

	/************************************************************************

	string strNName;
	const char * GetNName()
	{
		if (!strNName.length())
		{
			strNName = hge->Ini_GetString(Sec, N, Def);
		}
		return strNName.c_str();
	};
	void RestoreNName(bool bWrite)
	{
		strNName = Def;
		hge->Ini_SetString(Sec, N, "");
	}
	************************************************************************/

#define _SMFDEF(STRNAME, SEC, DEF)	\
	string str##STRNAME##Name;	\
	const char * Get##STRNAME##Name()	\
	{	\
		if (!str##STRNAME##Name.length())	\
		{	\
			str##STRNAME##Name = hge->Ini_GetString(SEC, #STRNAME, DEF);	\
		}	\
		return str##STRNAME##Name.c_str();	\
	};	\
	void Restore##STRNAME##Name(bool bWrite)	\
	{	\
		str##STRNAME##Name = DEF;	\
		hge->Ini_SetString(SEC, #STRNAME, "");	\
        if (bWrite) \
        {   \
        }   \
	}

	_SMFDEF(	SnapshotDefault,	INIS_DEFAULTSTR,	"Snapshot"	);
	_SMFDEF(	NodeCloneAppend,	INIS_DEFAULTSTR,	" Copy"	);

#define _SMCLFDEF(STRNAME, DEF)	\
	_SMFDEF(CL##STRNAME, INIS_COMMANDLOG, DEF)

	_SMCLFDEF(	CreateCommand,	"Command"	);
	_SMCLFDEF(	FinishCommand,	"Done Command"	);
	_SMCLFDEF(	TerminalCommand,	"Terminal Command"	);
	_SMCLFDEF(	Error,	"Error"	);
	_SMCLFDEF(	SetParameter,	"Set Parameter"	);
	_SMCLFDEF(	NextPrompt,	"Please Input"	);
	_SMCLFDEF(	FinishSubCommand,	"Done"	);
	_SMCLFDEF(	UnDo,	"Processing UnDo"	);
	_SMCLFDEF(	ReDo,	"Processing ReDo"	);

#undef _SMCLFDEF

#define _SMNNFDEF(STRNAME, DEF)	\
	_SMFDEF(NN##STRNAME, INIS_NODENAME, DEF)

	_SMNNFDEF(	Object,	"Object"	);
	_SMNNFDEF(	Layer,	"Layer"	);
	_SMNNFDEF(	Shape,	"Shape"	);
	_SMNNFDEF(	Piece,	"Piece"	);
	_SMNNFDEF(	Line,	"Line"	);
	_SMNNFDEF(	Point,	"Point"	);
	_SMNNFDEF(	AnchorPoint,	"Anchor"	);
    _SMNNFDEF(	MidPoint,	"MidPoint"	);
	_SMNNFDEF(	HandlePoint,	"Handle"	);

#undef _SMNNFDEF

#define _SMTOOLTIPDEF(STRNAME, DEF)	\
	_SMFDEF(ToolTip##STRNAME, INIS_TOOLTIP, DEF)

	_SMTOOLTIPDEF(	Snapshot,	"SnapShot"	);
	_SMTOOLTIPDEF(	Delete,	"Delete"	);

#undef _SMTOOLTIPDEF

#define _SMNODEINFODEF(STRNAME, DEF) \
    _SMFDEF(NodeInfo##STRNAME, INIS_NODEINFO, DEF)

    _SMNODEINFODEF(	Info,	"Node Information"	);
    _SMNODEINFODEF(	ID,	"ID"	);
	_SMNODEINFODEF(	Name,	"Name"	);
	_SMNODEINFODEF(	Position,	"Position"	);
	_SMNODEINFODEF(	Length,	"Length"	);
	_SMNODEINFODEF(	LengthCalculatePrompt,	"Press button to Calculate"	);
    _SMNODEINFODEF(	ClingTo,	"Cling To"	);
	_SMNODEINFODEF(	ClingBy,	"Cling By"	);
	_SMNODEINFODEF(	MergeWith,	"Merge With"	);
	_SMNODEINFODEF(	BindWith,	"Bind With"	);

	_SMNODEINFODEF(	Hierarchy,	"Hierarchy Information"	);
	_SMNODEINFODEF(	Parent,	"Parent"	);
	_SMNODEINFODEF(	Children,	"Children"	);

	_SMNODEINFODEF(	Transform,	"Transform"	);
	_SMNODEINFODEF(	ToBezier,	"To Bezier"	);
	_SMNODEINFODEF(	ToStrightLine,	"To Straight Line"	);
	_SMNODEINFODEF(	SwapBeginEnd,	"Swap Begin End Anchors"	);

#undef _SMNODEINFODEF

#define _SMMBDEF(STRNAME, DEF)	\
	_SMFDEF(MB##STRNAME, INIS_MESSAGEBOX, DEF)

	_SMMBDEF(Warning, "Warning");
	_SMMBDEF(WRecoverFromInvalidSnapshot, "This action will clear all UNDO steps and is inrevertable.\r\nDo you want to continue?");
	_SMMBDEF(Error, "Error");

#undef	_SMMBDEF

#undef _SMFDEF
};
