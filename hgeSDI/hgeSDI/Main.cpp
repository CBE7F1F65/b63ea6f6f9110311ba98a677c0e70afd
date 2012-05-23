#include "stdafx.h"
#include "Main.h"

#include "Command.h"
#include "GUICursor.h"
#include "GUICoordinate.h"

#include "Resource.h"

MainInterface * pmi = NULL;

MainInterface::MainInterface()
{
	assert(pmi==NULL);
	hge = hgeCreate(HGE_VERSION);

	resizewindow_w = -1;
	resizewindow_h = -1;
	mousex = 0;
	mousey = 0;
	mousewheel = 0;
	lastmousewheel = 0;
	bgcol = 0xfff0f0f0;
	wheelscalefactor = 1.25f;

	cursorleftkeyindex = 0;
	cursorrightkeyindex = 1;

	manageloop = false;

	pickstate = 0;
}

MainInterface::~MainInterface()
{
	if (hge)
	{
		hge->Release();
		hge = NULL;
	}
}

MainInterface * MainInterface::getInstance()
{
	if (!pmi)
	{
		pmi = new MainInterface();
	}
	return pmi;
}

bool RenderFunc()
{
	return MainInterface::getInstance()->Render();
}

bool FrameFunc()
{
	return MainInterface::getInstance()->Frame();
}

bool FocusLostFunc()
{
	return MainInterface::getInstance()->FocusLost();
}

bool FocusGainFunc()
{
	return MainInterface::getInstance()->FocusGain();
}

bool ExitFunc()
{
	return MainInterface::getInstance()->Exit();
}

bool GfxRestoreFunc()
{
	return MainInterface::getInstance()->GfxRestore();
}

bool MainInterface::Render()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(bgcol);

	float scrw=hge->System_GetState(HGE_SCREENWIDTH);
	float scrh=hge->System_GetState(HGE_SCREENWIDTH);



	hge->Input_GetMousePos(&mousex, &mousey);

	// Render Grids
	GUICoordinate::getInstance()->RenderGrid();

	/*
	float x[2], y[2];
	for (int i = 0; i < 2 ; i++)
	{
		x[i] = hge->Random_Float(0, scrw);
		y[i] = hge->Random_Float(0, scrh);
	}
	for (int i = 0; i < 400 ; i++)
	{
		hge->Gfx_RenderLine(x[0], y[0], x[1], y[1], 0xff000000|((DWORD)hge->Random_Int(0, 0xffffff)));
	}
	*/

	// Render Coord
	GUICoordinate::getInstance()->RenderCoordinate();
	// Render Cursor
	GUICursor::getInstance()->Render(mousex, mousey);

	hge->Gfx_EndScene();
	return false;
}

bool MainInterface::Frame()
{
	DoResizeWindow();

	if (hge->Input_IsMouseOver())
	{
		SetFocus(hge->System_GetState(HGE_HWND));
	}

	lastmousex = mousex;
	lastmousey = mousey;
	hge->Input_GetMousePos(&mousex, &mousey);
	lastmousewheel = mousewheel;
	mousewheel = hge->Input_GetMouseWheel();

	Command * pcommand = Command::getInstance();

	GUICoordinate * pguic = GUICoordinate::getInstance();
	if (mousewheel != lastmousewheel)
	{
		pcommand->CreateCommand(COMM_DOZOOM);
		pcommand->SetParamX(CSP_DOZOOM_C_XY_SCALE, mousex);
		pcommand->SetParamY(CSP_DOZOOM_C_XY_SCALE, mousey);
		pcommand->SetParamF(CSP_DOZOOM_C_XY_SCALE, powf(wheelscalefactor, mousewheel-lastmousewheel));
	}

	if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_DOWN) && IsMainViewActive())
	{
		pcommand->CreateCommand(COMM_PAN);
	}
	if (hge->Input_GetDIKey(DIK_ESCAPE, DIKEY_UP))
	{
		if (pcommand->ccomm.command)
		{
			pcommand->StepTo(CSI_TERMINAL);
		}
	}
	if (IsMainViewActive())
	{
		UpdatePickPoint();
	}
	/*
	if (hge->Input_GetDIKey(DIK_SPACE))
	{
		if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_DOWN))
		{
			GUICursor::getInstance()->ChangeCursor(GUIC_HAND);
		}
		if (hge->Input_GetDIMouseKey(1) && !hge->Input_GetDIMouseKey(1, DIKEY_DOWN))
		{
			pguic->DoPan(mousex-lastmousex, mousey-lastmousey);
		}
	}
	if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_UP))
	{
		GUICursor::getInstance()->ChangeCursor();
	}
	*/

	pguic->SetCursorPosition(mousex, mousey);

	pcommand->ProcessCommand();

	DoUpdateFPS();
	DoUpdateStatusInfo();

	if (hge->Input_GetDIMouseKey(cursorrightkeyindex, DIKEY_UP) && hge->Input_IsMouseOver()
		|| hge->Input_GetDIKey(DIK_APPS, DIKEY_UP)
		)
	{
		CallContextMenu(mousex, mousey);
	}
	return false;
}

bool MainInterface::FocusLost()
{
	hge->System_SetState(HGE_HIDEMOUSE, false);
	return true;
}

bool MainInterface::FocusGain()
{
	hge->System_SetState(HGE_HIDEMOUSE, true);
	return true;
}

bool MainInterface::Exit()
{
	return false;
}

bool MainInterface::GfxRestore()
{
	return true;
}

void _HGEThreadFunc(void * dummy)
{
	if (MainInterface::getInstance()->HGEThreadFunc())
	{
		_endthread();
	}
}

bool MainInterface::HGEThreadFunc()
{
	if (!hge->System_Start())
	{
		return true;
	}
	return false;
}

bool MainInterface::OnInit(void * parent, int w, int h)
{
	if (!parent)
	{
		return false;
	}

	// MFC
	parentview = (ChgeSDIView *)parent;
	hge->System_SetState(HGE_HWNDPARENT, (HWND)(parentview->m_hWnd));

	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusLostFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusGainFunc);
	hge->System_SetState(HGE_EXITFUNC, ExitFunc);
	hge->System_SetState(HGE_GFXRESTOREFUNC, GfxRestoreFunc);

	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_LOGFILE, "log.log");
	hge->System_SetState(HGE_USESOUND, false);
	//	hge->System_SetState(HGE_USEDINPUT, false);
	//	hge->System_SetState(HGE_HIDEMOUSE, false);
	hge->System_SetState(HGE_NOWMPAINT, true);
	hge->System_SetState(HGE_FPS, 60);
	
	hge->System_SetState(HGE_SCREENWIDTH, w);
	hge->System_SetState(HGE_SCREENHEIGHT, h);

	if (!manageloop)
	{
		hge->System_SetState(HGE_MANAGELOOP, false);
	}
	
	hge->System_Initiate();

#ifdef __WIN32
	if (GetSystemMetrics(SM_SWAPBUTTON))
	{
		cursorleftkeyindex = 1;
		cursorrightkeyindex = 0;
	}
#endif
	hge->Input_GetMousePos(&mousex, &mousey);

	//
	Command::getInstance()->Init();
	GUICoordinate::getInstance()->SetGrid(GUICG_METRIC, 0, 0);

	return manageloop;

//	HANDLE hThread = (HANDLE)_beginthread(_HGEThreadFunc, 0, NULL);
}

void MainInterface::OnResizeWindow(int x, int y)
{
	resizewindow_w = x;
	resizewindow_h = y;
}

void MainInterface::OnUpdateTimer()
{
	if (!manageloop)
	{
		hge->System_Start();
	}
}

void MainInterface::OnMouseActivate()
{
//	hge->System_SetState(HGE_HIDEMOUSE, true);
}

void MainInterface::CallContextMenu(float x, float y)
{
	// MFC
	int px = (int)x;
	int py = (int)y;
	CPoint point;
	point.x = (LONG)px;
	point.y = (LONG)py;
	ClientToScreen(parentview->m_hWnd, &point);
//	PostMessage(parentview->m_hWnd, WM_RBUTTONUP, (WPARAM)parentview->m_hWnd, MAKELONG(px, py));
	PostMessage(parentview->m_hWnd, WM_CONTEXTMENU, (WPARAM)parentview->m_hWnd, MAKELONG(point.x, point.y));
//	parentview->CallContextMenu(x, y);

}

void MainInterface::CallUpdateStatusBarText( int id, const char * text )
{
	parentview->GetMainFrame()->SetStatusBarText(id, text);
}

void MainInterface::CallAppendCommandLogText( const char * text )
{
	parentview->GetMainFrame()->AppendCommandLogText(text);
}

int MainInterface::OnCommand( DWORD comm )
{
	return Command::getInstance()->CreateCommand(comm);
}

int MainInterface::OnCommitCommand( const char * str )
{
	return Command::getInstance()->CommitCommand(str);
}

void MainInterface::DoResizeWindow()
{
	if (resizewindow_w >= 0 && resizewindow_h >= 0)
	{
		// No Command (internal process)
		MoveWindow(hge->System_GetState(HGE_HWND), 0, 0, resizewindow_w, resizewindow_h, TRUE);
		GUICoordinate::getInstance()->UpdateScreenMeasure();
		resizewindow_w = -1;
		resizewindow_h = -1;
	}
}

void MainInterface::DoUpdateFPS()
{
	// No Command (internal process)
	char fpsstr[M_STRMAX];
	sprintf_s(fpsstr, M_STRMAX, "%.2f", hge->Timer_GetFPS(30));
	CallUpdateStatusBarText(IDS_STATUS_PANE2, fpsstr);
}

void MainInterface::DoUpdateStatusInfo()
{
	// No Command (internal process)
	char statusstr[M_STRMAX];
	GUICoordinate * pguic = GUICoordinate::getInstance();
	sprintf_s(statusstr, M_STRMAX, "%.4f, %.4f, %.4f", pguic->cursorx_c, pguic->cursory_c, pguic->scale);
	CallUpdateStatusBarText(IDS_STATUS_PANE1, statusstr);
}

#define _PICKSTATE_NULL	0
#define _PICKSTATE_PICKING		1
#define _PICKSTATE_READY		2

bool MainInterface::DoPickPoint( int restrict/*=0*/ )
{
	if (pickstate == _PICKSTATE_NULL)
	{
		pickrestrict = restrict;
		pickstate = _PICKSTATE_PICKING;
	}
	if (pickstate == _PICKSTATE_READY)
	{
		pickstate = _PICKSTATE_NULL;
		return true;
	}
	return false;
}

bool MainInterface::UpdatePickPoint()
{
	if (pickstate != _PICKSTATE_PICKING)
	{
		return false;
	}
	if (hge->Input_GetDIMouseKey(cursorleftkeyindex, DIKEY_UP))
	{
		pickx = mousex;
		picky = mousey;
		pickstate = _PICKSTATE_READY;
		return true;
	}
	return false;
}

bool MainInterface::IsMainViewActive()
{
	if (hge->Input_IsMouseOver())
	{
		return true;
	}
	return false;
}