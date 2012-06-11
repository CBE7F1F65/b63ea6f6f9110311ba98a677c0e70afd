#pragma once

#include "MainDependency.h"

#define MVINACTIVEREASON_POPUP				1
#define MVINACTIVEREASON_FLOATINGCOMMAND	2

// MFC
#include "hgeSDIView.h"
#include "Command.h"
#include "GLayer.h"

class MainInterface
{
public:
	static MainInterface& getInstance()
	{
		static MainInterface instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	MainInterface();
	~MainInterface();
	// Don't forget to declare these two.
	MainInterface(MainInterface const&);
	// Don't Implement
	void operator=(MainInterface const&);
	// Don't implement

public:

	bool IsMainViewActive();
	void SetMainViewActive(bool bActive, int reason);

	// From Frame
	bool OnPreInit();
	bool OnInit(void * parent, int w, int h);
	void OnResizeWindow(int x, int y);
	void OnUpdateTimer();
	void OnMouseActivate();
	void OnDoScroll(bool horz, int pos, int range);

	// From Command
	enum{
		COMM_LINE,
	};
	int OnCommand(int comm);
	int OnCommitCommand(const char * str);

	void OnPushRevertable(const char * desc, const char * commandstr, int command);
	void OnUnDo(int step=1);
	void OnReDo(int step=1);
	void OnClearReDo(int ndelete);
	void OnClearPreviousHistory(int ndelete=1);
	void OnRebuildLayerTree(GObject * changebase, GObject * activeitem);
	GLayer * OnGetActiveLayer();
	GObject * OnGetSelectedNodes( int * pnextfromIndex );
	bool OnGetDragDroppedNodes( GLayer ** pLayerNode, GObject ** pAfterNode );
	void OnSetActiveLayer_Internal( GLayer * pLayer );
	void OnInternalActiveLayerSetDone();

	// To Frame
	void CallContextMenu(float x, float y);
	void CallUpdateStatusBarText(int id, const char * text);
	void CallAppendCommandLogText(const char * text, bool bNewLine=true);

	// To Command
	void CallUnDoReDo(int step);

	// Do

	void DoResizeWindow();
	void DoUpdateFPS();
	void DoUpdateStatusInfo();

	bool DoPickPoint(int restrict=0);
	bool UpdatePickPoint();

	void DoCheckFloatCommand();

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
		return precision;
	};
	HGE * hge;
	int resizewindow_w;
	int resizewindow_h;
	float mousex;
	float mousey;
	float lastmousex;
	float lastmousey;
	int lastmousewheel;
	int mousewheel;

	float wheelscalefactor;

	int cursorleftkeyindex;
	int cursorrightkeyindex;

	bool manageloop;

	int pickstate;
	int pickrestrict;
	float pickx;
	float picky;
	int pickid;

	float precision;
	bool bActive;
	int toactivedelay;
	int inactivereason;

	// MFC
	ChgeSDIView * parentview;
};