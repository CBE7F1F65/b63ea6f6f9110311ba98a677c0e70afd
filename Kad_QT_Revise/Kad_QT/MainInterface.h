#pragma once

#include "MainDependency.h"

#define MVINACTIVEREASON_POPUP				1
#define MVINACTIVEREASON_FLOATINGCOMMAND	2

#define MI_AUTOSAVE_POSTFIX	"~"
#define MI_SAVEFILEEXTENSION	".xml"

// MFC
#include "Command.h"
#include "GLayer.h"

#include "qtui_glview.h"

class MainInterface
{
public:
	static MainInterface& getInstance() { static MainInterface instance; return instance; }

private:
	MainInterface();
	~MainInterface();
	MainInterface(MainInterface const&);
	void operator=(MainInterface const&);

public:

	bool IsMainViewActive();
	void SetMainViewActive(bool bActive, int reason);

	// From Frame
	bool OnPreInit();
	bool OnInit(void * parent, int w, int h);
	void OnResizeWindow(int x, int y);
	void OnUpdateTimer();
	void OnDoScroll(bool horz, int pos, int range);

	void OnChangeMouseCursor(HWND hwnd, int mousecursor=-1);

	// From Command
	enum{
		COMM_LINE,
	};
	int OnCommand(int comm);
	int OnCommandWithParam(int comm, int firsttype, ...);

	int OnCommandSingleParamI(int ival);
	int OnCommandSingleParamF(float fval);
	int OnCommandSingleParamS(const char * sval);
	int OnCommandSingleParamSubCommand(int ssc);

	int OnCommitCommand(const char * str);

	void OnPushRevertible(const char * desc, const char * commandstr, int command);
	void OnUnDo(int step=1);
	void OnReDo(int step=1);
	void OnClearReDo(int ndelete);
	void OnClearUnDo(int ndelete);
	void OnClearPreviousHistory(int ndelete=1);
	void OnTreeLockChange( bool toLock );
	void OnRebuildLayerTree(GObject * changebase, GObject * activeitem);
    list<GObject *> * OnGetSelectedNodes();
    GObject * OnGetHoveringNode();
	bool OnGetDragDroppedNodes( GLayer ** pLayerNode, GObject ** pAfterNode );
	void OnSetActiveLayer_Internal( GLayer * pLayer );
	void OnSetActiveLayerFromUI();

	// To Frame
	void CallUpdateStatusBarText(int id, const char * text);
	void CallAppendCommandLogText(const char * text, bool bNewLine=true);
	void CallChangeNode( GObject * pObj );
    void CallShowNodeInfo(GObject * pObj=NULL, bool bContext=false);

	// To Command
	void CallUnDoReDo(int step);

	// To Nodes
	void CallSelectAllNodes(bool bOnlyThisLayer=false);

	// Do

	void DoResizeWindow();
	void DoUpdateFPS();
	void DoUpdateStatusInfo();

	// File

	bool OpenFile();
	bool OpenFile(const char * filename);
	bool SaveFile(bool bSaveAs=false);
	bool SaveFile(const char * filename, bool bAutosave=false);
	bool AutoSaveFile();

	bool HGEThreadFunc();



	bool Render();
	bool Frame();
	bool FocusLost();
	bool FocusGain();
	bool Exit();
	bool GfxRestore();

	void MBeep(int id=-1);

    inline float GetPrecision()
	{
        return nPrecision/100.0f;
	};
    inline int GetPrecisionInt()
    {
        return nPrecision;
    };
    void SetPrecisionInt(int nVal);

	int GetLeftKeyIndex(){return cursorleftkeyindex;};
	int GetRightKeyIndex(){return cursorrightkeyindex;};

	HGE * hge;
private:
	int resizewindow_w;
	int resizewindow_h;

	float mousex;
	float mousey;
	float lastmousex;
	float lastmousey;
public:

	void SetLeftDoubleClick(){bLeftDoubleClick=true;};
	bool CheckLeftDoubleClick(){bool bt = bLeftDoubleClick; bLeftDoubleClick = false; return bt;};
	void SetRightDoubleClick(){bRightDoubleClick=true;};
	bool CheckRightDoubleClick(){bool bt = bRightDoubleClick; bRightDoubleClick = false; return bt;};
	void SetMiddleDoubleClick(){bMiddleDoubleClick=true;};
	bool CheckMiddleDoubleClick(){bool bt = bMiddleDoubleClick; bMiddleDoubleClick = false; return bt;};

	float GetMouseXForCoord(){return mousex;};
	float GetMouseYForCoord(){return mousey;};
	float GetLastMouseXForCoord(){return lastmousex;};
	float GetLastMouseYForCoord(){return lastmousey;};

	float GetMouseX_S();
	float GetMouseY_S();
	float GetMouseX_C();
	float GetMouseY_C();

	float GetPrintMul();
	int GetPrintDPI(){return printdpi;};
	float GetDisplayMul(){return displaymul;};
	int GetScreenDPI();
	int GetPrintsize_W();
	int GetPrintsize_H();
	int GetPrintMargin();

	const char * GetSaveFileName() {return savefilename.c_str();};

private:

	int nFileStatus;
	string savefilename;

	bool bLeftDoubleClick;
	bool bRightDoubleClick;
	bool bMiddleDoubleClick;

	int lastmousevwheel;
	int mousevwheel;

	float wheelscalefactor;

	int cursorleftkeyindex;
	int cursorrightkeyindex;

	bool manageloop;

	float nPrecision;
	bool bActive;
	int toactivedelay;
	int inactivereason;

	int printdpi;
	float displaymul;
	float printsize_w;
	float printsize_h;
	float printmargin;

	bool bLockLeftMouseDown;

	QTUI_GLView * parentview;

	QTime autosavelasttime;
	int autosavesecond;
};
