#include "stdafx.h"
#include "Main.h"

#include "Command.h"
#include "GUICursor.h"
#include "GUICoordinate.h"
#include "ColorManager.h"
#include "GObjectManager.h"
#include "RenderHelper.h"
#include "RenderTargetManager.h"

#include "Resource.h"

#define MV_ACTIVEDELAY	10

MainInterface::MainInterface()
{
	hge = hgeCreate(HGE_VERSION);

	resizewindow_w = -1;
	resizewindow_h = -1;
	mousex = 0;
	mousey = 0;
	mousewheel = 0;
	lastmousewheel = 0;
	wheelscalefactor = 1.25f;

	cursorleftkeyindex = 0;
	cursorrightkeyindex = 1;

	toactivedelay = -1;

	manageloop = false;
	bActive = false;

	pickstate = 0;
	precision = 0.5f;
}

MainInterface::~MainInterface()
{
	if (hge)
	{
		hge->Release();
	}
}

bool RenderFunc()
{
	return MainInterface::getInstance().Render();
}

bool FrameFunc()
{
	return MainInterface::getInstance().Frame();
}

bool FocusLostFunc()
{
	return MainInterface::getInstance().FocusLost();
}

bool FocusGainFunc()
{
	return MainInterface::getInstance().FocusGain();
}

bool ExitFunc()
{
	return MainInterface::getInstance().Exit();
}

bool GfxRestoreFunc()
{
	return MainInterface::getInstance().GfxRestore();
}

bool MainInterface::Render()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(ColorManager::GetBGColor());

	float scrw=hge->System_GetState(HGE_SCREENWIDTH);
	float scrh=hge->System_GetState(HGE_SCREENWIDTH);



	hge->Input_GetMousePos(&mousex, &mousey);

	// Render Grids
	GUICoordinate::getInstance().RenderGrid();

	// All Nodes
	GObjectManager::getInstance().Render();
	// Render Command Floating
	Command::getInstance().Render();

	// Render Coord
	GUICoordinate::getInstance().RenderCoordinate();
	// Render Cursor
	GUICursor::getInstance().Render(mousex, mousey);

	hge->Gfx_EndScene();
	return false;
}

bool MainInterface::Frame()
{
	DoResizeWindow();

	if (toactivedelay > 0)
	{
		toactivedelay--;
	}
	else if (!toactivedelay)
	{
		if (!bActive)
		{
			bActive = true;
			toactivedelay = -1;
		}
	}

	if (hge->Input_GetDIKey(DIK_ESCAPE, DIKEY_UP))
	{
//		parentview->GetMainFrame()->m_wndUIFloatingEdit.Hide();
	}

	if (hge->Input_IsMouseOver() && bActive)
	{
		SetFocus(hge->System_GetState(HGE_HWND));
	}

	if (IsMainViewActive())
	{
		hge->System_SetState(HGE_HIDEMOUSE, true);
	}
	else
	{
		hge->System_SetState(HGE_HIDEMOUSE, false);
	}


	lastmousex = mousex;
	lastmousey = mousey;
	hge->Input_GetMousePos(&mousex, &mousey);
	lastmousewheel = mousewheel;
	mousewheel = hge->Input_GetMouseWheel();

	DoCheckFloatCommand();

	Command * pcommand = &Command::getInstance();

	GUICoordinate * pguic = &GUICoordinate::getInstance();
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
	if (IsMainViewActive() && hge->Input_GetDIKey(DIK_ESCAPE, DIKEY_UP))
	{
		if (pcommand->IsInternalProcessing())
		{
			pcommand->TerminalInternalProcess();
		}
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
			GUICursor::getInstance().ChangeCursor(GUIC_HAND);
		}
		if (hge->Input_GetDIMouseKey(1) && !hge->Input_GetDIMouseKey(1, DIKEY_DOWN))
		{
			pguic->DoPan(mousex-lastmousex, mousey-lastmousey);
		}
	}
	if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_UP))
	{
		GUICursor::getInstance().ChangeCursor();
	}
	*/

	pguic->SetCursorPosition(mousex, mousey);

	GObjectManager::getInstance().Update();
	pcommand->ProcessCommand();

	GObjectManager::getInstance().Delete();
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
	GObjectManager::getInstance().Release();
	RenderHelper::getInstance().Release();
	hge->System_Shutdown();
	return false;
}

bool MainInterface::GfxRestore()
{
	return true;
}

void _HGEThreadFunc(void * dummy)
{
	if (MainInterface::getInstance().HGEThreadFunc())
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
	bActive = true;

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
	Command::getInstance().Init();
	GUICoordinate::getInstance().SetGrid(GUICG_METRIC, 0, 0);
	GObjectManager::getInstance().Init();
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
//	parentview->GetMainFrame()->m_wndUIFloatingEdit.Hide();
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

void MainInterface::CallAppendCommandLogText( const char * text, bool bNewLine/*=true*/ )
{
	parentview->GetMainFrame()->AppendCommandLogText(text, bNewLine);
}

int MainInterface::OnCommand( DWORD comm )
{
	return Command::getInstance().CreateCommand(comm);
}

int MainInterface::OnCommitCommand( const char * str )
{
	return Command::getInstance().CommitCommand(str);
}

void MainInterface::DoResizeWindow()
{
	if (resizewindow_w >= 0 && resizewindow_h >= 0)
	{
		// No Command (internal process)
		MoveWindow(hge->System_GetState(HGE_HWND), 0, 0, resizewindow_w, resizewindow_h, TRUE);
		GUICoordinate::getInstance().UpdateScreenMeasure();
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
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	sprintf_s(statusstr, M_STRMAX, "%.4f, %.4f, %.4f", pguic->cursorx_c, pguic->cursory_c, pguic->scale);
	CallUpdateStatusBarText(IDS_STATUS_PANE1, statusstr);
}

#define _PICKSTATE_NULL					0
#define _PICKSTATE_REQUIREUPDATE		1
#define _PICKSTATE_AFTERUPDATE			2
#define _PICKSTATE_READY				3

bool MainInterface::DoPickPoint( int restrict/*=0*/ )
{
	if (pickstate == _PICKSTATE_NULL)
	{
		pickrestrict = restrict;
		pickstate = _PICKSTATE_REQUIREUPDATE;
	}
	else if (pickstate == _PICKSTATE_AFTERUPDATE)
	{
		pickstate = _PICKSTATE_REQUIREUPDATE;
	}
	else if (pickstate == _PICKSTATE_READY)
	{
		pickstate = _PICKSTATE_NULL;
		return true;
	}
	return false;
}

bool MainInterface::UpdatePickPoint()
{
	if (pickstate != _PICKSTATE_REQUIREUPDATE)
	{
		return false;
	}
	pickstate = _PICKSTATE_AFTERUPDATE;
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
	if (bActive && hge->Input_IsMouseOver() && GetFocus()==hge->System_GetState(HGE_HWND))
	{
		return true;
	}
	return false;
}

void MainInterface::SetMainViewActive( bool _bActive, int reason )
{
	if (!_bActive)
	{
		bActive = _bActive;
		toactivedelay = -1;
		/*
		switch (inactivereason)
		{
		case 0:
			break;
		case MVINACTIVEREASON_POPUP:
			parentview->GetMainFrame()->m_wndUIPopupMenu.Hide(true);
			break;
		case MVINACTIVEREASON_FLOATINGCOMMAND:
			parentview->GetMainFrame()->m_wndUIFloatingEdit.Hide(true);
			break;
		}
		*/
		inactivereason = reason;
	}
	else
	{
		if (!bActive)
		{
			toactivedelay = MV_ACTIVEDELAY;
		}
		inactivereason = 0;
	}
}

void MainInterface::DoCheckFloatCommand()
{
	if (!IsMainViewActive())
	{
		return;
	}

	WPARAM vk=0;

#define _SETVKBYKEY(DIK)	\
	if (hge->Input_GetDIKey(DIK, DIKEY_DOWN))	\
	{	\
		vk = i;	\
		break;	\
	}	\
	i++;

	do
	{
		int i=0x41;
		_SETVKBYKEY(DIK_A);
		_SETVKBYKEY(DIK_B);
		_SETVKBYKEY(DIK_C);
		_SETVKBYKEY(DIK_D);
		_SETVKBYKEY(DIK_E);
		_SETVKBYKEY(DIK_F);
		_SETVKBYKEY(DIK_G);
		_SETVKBYKEY(DIK_H);
		_SETVKBYKEY(DIK_I);
		_SETVKBYKEY(DIK_J);
		_SETVKBYKEY(DIK_K);
		_SETVKBYKEY(DIK_L);
		_SETVKBYKEY(DIK_M);
		_SETVKBYKEY(DIK_N);
		_SETVKBYKEY(DIK_O);
		_SETVKBYKEY(DIK_P);
		_SETVKBYKEY(DIK_Q);
		_SETVKBYKEY(DIK_R);
		_SETVKBYKEY(DIK_S);
		_SETVKBYKEY(DIK_T);
		_SETVKBYKEY(DIK_U);
		_SETVKBYKEY(DIK_V);
		_SETVKBYKEY(DIK_W);
		_SETVKBYKEY(DIK_X);
		_SETVKBYKEY(DIK_Y);
		_SETVKBYKEY(DIK_Z);

		i=0x30;
		_SETVKBYKEY(DIK_0);
		_SETVKBYKEY(DIK_1);
		_SETVKBYKEY(DIK_2);
		_SETVKBYKEY(DIK_3);
		_SETVKBYKEY(DIK_4);
		_SETVKBYKEY(DIK_5);
		_SETVKBYKEY(DIK_6);
		_SETVKBYKEY(DIK_7);
		_SETVKBYKEY(DIK_8);
		_SETVKBYKEY(DIK_9);

		i=VK_NUMPAD0;
		_SETVKBYKEY(DIK_NUMPAD0);
		_SETVKBYKEY(DIK_NUMPAD1);
		_SETVKBYKEY(DIK_NUMPAD2);
		_SETVKBYKEY(DIK_NUMPAD3);
		_SETVKBYKEY(DIK_NUMPAD4);
		_SETVKBYKEY(DIK_NUMPAD5);
		_SETVKBYKEY(DIK_NUMPAD6);
		_SETVKBYKEY(DIK_NUMPAD7);
		_SETVKBYKEY(DIK_NUMPAD8);
		_SETVKBYKEY(DIK_NUMPAD9);

		i=VK_MULTIPLY;
		_SETVKBYKEY(DIK_MULTIPLY);
		_SETVKBYKEY(DIK_ADD);
		i=VK_SUBTRACT;
		_SETVKBYKEY(DIK_SUBTRACT);
		_SETVKBYKEY(DIK_DECIMAL);
		_SETVKBYKEY(DIK_DIVIDE);

		i=VK_OEM_1;
		_SETVKBYKEY(DIK_SEMICOLON);
		_SETVKBYKEY(DIK_EQUALS);
		_SETVKBYKEY(DIK_COMMA);
		_SETVKBYKEY(DIK_MINUS);
		_SETVKBYKEY(DIK_PERIOD);
		_SETVKBYKEY(DIK_SLASH);
		_SETVKBYKEY(DIK_GRAVE);

		i=VK_OEM_4;
		_SETVKBYKEY(DIK_LBRACKET);
		_SETVKBYKEY(DIK_BACKSLASH);
		_SETVKBYKEY(DIK_RBRACKET);
		_SETVKBYKEY(DIK_APOSTROPHE);
	} while(false);



	if (vk)
	{
		parentview->GetMainFrame()->CallEnableFloatCommand(vk);
	}
}

void MainInterface::OnDoScroll( bool horz, int pos, int range )
{
	GUICoordinate::getInstance().DoScroll(horz, pos, range);
}

void MainInterface::MBeep( int id/*=-1*/ )
{
	MessageBeep(id);
}