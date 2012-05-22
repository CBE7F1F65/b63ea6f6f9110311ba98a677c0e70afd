#pragma once

#include "MainDependency.h"

// MFC
#include "hgeSDIView.h"

class MainInterface
{
public:
	MainInterface();
	~MainInterface();

	static MainInterface * getInstance();

	// From Frame
	bool OnInit(void * parent, int w, int h);
	void OnResizeWindow(int x, int y);
	void OnUpdateTimer();
	void OnMouseActivate();

	// From Command
	enum{
		COMM_LINE,
	};
	int OnCommand(DWORD comm);

	// To Frame
	void CallContextMenu(float x, float y);
	void CallUpdateStatusBarText(int id, const char * text);
	void CallAppendCommandLogText(const char * text);

	// Do
	void DoResizeWindow();
	void DoUpdateFPS();
	void DoUpdateStatusInfo();

	bool HGEThreadFunc();

	bool Render();
	bool Frame();
	bool FocusLost();
	bool FocusGain();
	bool Exit();
	bool GfxRestore();

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

	DWORD bgcol;

	bool manageloop;

	// MFC
	ChgeSDIView * parentview;
};