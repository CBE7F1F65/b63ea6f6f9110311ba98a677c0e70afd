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

#define INIS_COMMANDLOG		"CommandLog"

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

class StringManager
{
public:
	static StringManager& getInstance()
	{
		static StringManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	StringManager();
	~StringManager();
	// Don't forget to declare these two.
	StringManager(StringManager const&);
	// Don't Implement
	void operator=(StringManager const&);
	// Don't implement

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
	const char * GetLogFileName();
	const char * GetIniFileName();

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
	}

	************************************************************************/

#define _FDEF(STRNAME, SEC, DEF)	\
	string str##STRNAME##Name;	\
	const char * Get##STRNAME##Name()	\
	{	\
		if (!str##STRNAME##Name.length())	\
		{	\
			str##STRNAME##Name = hge->Ini_GetString(SEC, #STRNAME, DEF);	\
		}	\
		return str##STRNAME##Name.c_str();	\
	}

#define _CLFDEF(STRNAME, DEF)	\
	_FDEF(CL##STRNAME, INIS_COMMANDLOG, DEF)

	_CLFDEF(	CreateCommand,	"Command"	);
	_CLFDEF(	FinishCommand,	"Done Command"	);
	_CLFDEF(	TerminalCommand,	"Terminal Command"	);
	_CLFDEF(	Error,	"Error"	);
	_CLFDEF(	SetParameter,	"Set Parameter"	);
	_CLFDEF(	NextPrompt,	"Please Input"	);
	_CLFDEF(	FinishSubCommand,	"Done"	);
	_CLFDEF(	UnDo,	"Processing UnDo"	);
	_CLFDEF(	ReDo,	"Processing ReDo"	);
	
#undef _CLFDEF
#undef _FDEF
};