#include "stdafx.h"
#include "MainInterface.h"

#include "Command.h"
#include "GUICursor.h"
#include "GUICoordinate.h"
#include "ColorManager.h"
#include "GObjectManager.h"
#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "StringManager.h"
#include "IconManager.h"
#include "CommandTemplate.h"
#include "MouseCursorManager.h"
#include "SnapshotManager.h"
#include "GObjectPicker.h"
#include "MarqueeSelect.h"

#include "Resource.h"

#include "qmaininterface.h"

#include <sstream>

#define MV_ACTIVEDELAY	10

MainInterface::MainInterface()
{
	hge = hgeCreate(HGE_VERSION);

	resizewindow_w = -1;
	resizewindow_h = -1;
	mousex = 0;
	mousey = 0;
	mousevwheel = 0;
	lastmousevwheel = 0;
	wheelscalefactor = 1.25f;

	cursorleftkeyindex = 0;
	cursorrightkeyindex = 1;

	toactivedelay = -1;

	manageloop = false;
	bActive = false;

    nPrecision = 25;
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
	if (!hge)
	{
		return false;
	}
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(ColorManager::getInstance().GetBGColor());

	float scrw=hge->System_GetState(HGE_SCREENWIDTH);
	float scrh=hge->System_GetState(HGE_SCREENWIDTH);

//	hge->Input_GetMousePos(&mousex, &mousey);

	// Render Grids
	GUICoordinate::getInstance().RenderGrid();

	// All Nodes
	GObjectManager::getInstance().Render();
	// Render Command Floating
	Command::getInstance().Render();
	// Render Snap
	GObjectPicker::getInstance().Render();

	// Render MarqueeSelect
	MarqueeSelect::getInstance().Render();

	// Render Coord
	GUICoordinate::getInstance().RenderCoordinate();
	// Render Cursor
	GUICursor::getInstance().Render(mousex, mousey);

	hge->Gfx_EndScene();
	return false;
}

bool MainInterface::Frame()
{
	parentview->OnFrame();

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
		HWND hwnd = hge->System_GetState(HGE_HWND);
		if (GetFocus() != hwnd)
		{
			SetFocus(hwnd);
			hge->Input_ClearLastDIKeyState();
			hge->Input_ClearLastDIMouseState();
		}
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

	lastmousevwheel = mousevwheel;
	mousevwheel = hge->Input_GetMouseWheel();

	DoCheckFloatCommand();

	Command * pcommand = &Command::getInstance();

	GUICoordinate * pguic = &GUICoordinate::getInstance();
	if (mousevwheel != lastmousevwheel)
	{
		/*
		pcommand->CreateCommandCommit(COMM_DOZOOM);
		pcommand->SetParamX(CSP_DOZOOM_XY_F_C_SCALE, mousex);
		pcommand->SetParamY(CSP_DOZOOM_XY_F_C_SCALE, mousey);
		pcommand->SetParamF(CSP_DOZOOM_XY_F_C_SCALE, powf(wheelscalefactor, mousevwheel-lastmousevwheel));
		*/
		GUICoordinate::getInstance().DoZoom(mousex, mousey, powf(wheelscalefactor, mousevwheel-lastmousevwheel));
	}

	if (hge->Input_GetDIKey(DIK_SPACE, DIKEY_DOWN) && IsMainViewActive())
	{
		pcommand->CreateCommandCommit(COMM_PAN);
	}
	if (IsMainViewActive() && hge->Input_GetDIKey(DIK_ESCAPE, DIKEY_UP))
	{
		if (pcommand->IsInternalProcessing())
		{
			pcommand->TerminalInternalProcess();
		}
		if (pcommand->GetCurrentCommand())
		{
			pcommand->StepTo(CSI_TERMINAL);
		}
	}
	if (IsMainViewActive())
	{
		GObjectPicker::getInstance().UpdatePickPoint();
		MarqueeSelect::getInstance().Update();
	}

	pguic->SetCursorPosition(mousex, mousey);

	GObjectManager::getInstance().Update();
	pcommand->UpdateProcessCommand();//ProcessCommand();

	GObjectManager::getInstance().Delete();
	DoUpdateFPS();
	DoUpdateStatusInfo();

	if (hge->Input_GetDIKey(DIK_LMENU) || hge->Input_GetDIKey(DIK_RMENU))
	{
		GObjectManager::getInstance().SetHandleVisible(true);
	}
	else
	{
		GObjectManager::getInstance().SetHandleVisible(false);
	}

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
	SnapshotManager::getInstance().Release();
	GObjectManager::getInstance().Release();
	RenderHelper::getInstance().Release();
	hge->System_Shutdown();
	return false;
}

bool MainInterface::GfxRestore()
{
	return true;
}

bool MainInterface::HGEThreadFunc()
{
	if (!hge->System_Start())
	{
		return true;
	}
	return false;
}


bool MainInterface::OnPreInit()
{
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusLostFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusGainFunc);
	hge->System_SetState(HGE_EXITFUNC, ExitFunc);
	hge->System_SetState(HGE_GFXRESTOREFUNC, GfxRestoreFunc);

	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_LOGFILE, StringManager::getInstance().GetLogFileName());
	hge->System_SetState(HGE_INIFILE, StringManager::getInstance().GetIniFileName());
	hge->System_SetState(HGE_USESOUND, false);
	//	hge->System_SetState(HGE_USEDINPUT, false);
	//	hge->System_SetState(HGE_HIDEMOUSE, false);
	hge->System_SetState(HGE_NOWMPAINT, true);
	hge->System_SetState(HGE_FPS, 60);
	hge->System_SetState(HGE_OWNWINDOW, false);
	hge->System_SetState(HGE_CALLRENDER, false);


#ifdef __WIN32
	if (GetSystemMetrics(SM_SWAPBUTTON))
	{
		cursorleftkeyindex = 1;
		cursorrightkeyindex = 0;
	}
#endif
	StringManager::getInstance().Init();
	IconManager::getInstance().Init();
	Command::getInstance().Init();

	return true;
}

bool MainInterface::OnInit(void * parent, int w, int h)
{
	if (!parent)
	{
		return false;
	}
	bActive = true;

	// MFC
	parentview = (QTUI_GLView *)parent;
//	hge->System_SetState(HGE_HWNDPARENT, (HWND)(parentview->effectiveWinId()));
	hge->System_SetState(HGE_HWND, (HWND)(parentview->effectiveWinId()));
	hge->System_SetState(HGE_SCREENWIDTH, w);
	hge->System_SetState(HGE_SCREENHEIGHT, h);

	if (!manageloop)
	{
		hge->System_SetState(HGE_MANAGELOOP, false);
	}

	hge->System_Initiate();

	hge->Input_GetMousePos(&mousex, &mousey);

	//
	
	Command::getInstance().OnInit();
	GUICoordinate::getInstance().SetGrid(GUICG_METRIC, 0, 0);
	GObjectManager::getInstance().Init();
	GObjectPicker::getInstance().Init();
	
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
	POINT point;
	point.x = (LONG)px;
	point.y = (LONG)py;
//	ClientToScreen(parentview->m_hWnd, &point);
//	PostMessage(parentview->m_hWnd, WM_CONTEXTMENU, (WPARAM)parentview->m_hWnd, MAKELONG(point.x, point.y));
}

void MainInterface::CallUpdateStatusBarText( int id, const char * text )
{
//	parentview->GetMainFrame()->SetStatusBarText(id, text);
}

void MainInterface::CallAppendCommandLogText( const char * text, bool bNewLine/*=true*/ )
{
    QMainInterface::getInstance().GetPCommandLog()->AppendCommandLogText(text, bNewLine);
//	parentview->GetMainFrame()->AppendCommandLogText(text, bNewLine);
}

void MainInterface::CallChangeNode( GObject * pObj )
{
//	parentview->GetMainFrame()->ChangeNode(pObj);
}

int MainInterface::OnCommand( int comm )
{
	return Command::getInstance().CreateCommandCommit(comm);
}

int MainInterface::OnCommandWithParam( int comm, int firsttype, ... )
{
	va_list	ap;
	va_start(ap, firsttype);

	Command * pcommand = &Command::getInstance();
	OnCommand(comm);

	va_start(ap, firsttype);
	int vait = firsttype;

//	list<string>strlist;

	while (true)
	{
		if (!vait)
		{
			break;
		}
		switch (vait)
		{
		case COMMITTEDCOMMANDTYPE_FLOAT:
			{
				float vaif = (float)va_arg(ap, double);
				OnCommandSingleParamF(vaif);
// 				stringstream ss;
// 				ss << fixed << vaif;
// 				strlist.push_back(ss.str());
			}
			break;
		case COMMITTEDCOMMANDTYPE_INT:
			{
				int vaii = (int)va_arg(ap, int);
				OnCommandSingleParamI(vaii);
// 				stringstream ss;
// 				ss << vaii;
// 				strlist.push_back(ss.str());
			}
			break;
		case COMMITTEDCOMMANDTYPE_STRING:
			{
				char * vais = (char *)va_arg(ap, char *);
				ASSERT(vais);
				OnCommandSingleParamS(vais);
// 				stringstream ss;
// 				ss << "\"" << vais << "\"";
// 				strlist.push_back(ss.str());
			}
			break;
		case COMMITTEDCOMMANDTYPE_SUBCOMMAND:
			{
				int vaii = (int)va_arg(ap, int);
				OnCommandSingleParamSubCommand(vaii);
// 				const char * sstr = Command::getInstance().GetSubCommandStr(vaii);
// 				if (strlen(sstr))
// 				{
// 					strlist.push_back(sstr);
// 				}
// 				else
// 				{
// 					DASSERT(true);
// 				}
			}
			break;
		default:
			ASSERT(true);
		}
		vait = (int)va_arg(ap, int);
	}
	va_end(ap);

// 	for (list<string>::iterator it=strlist.begin(); it!=strlist.end(); ++it)
// 	{
// 		pcommand->CommitCommand(it->c_str());
// 	}

	return pcommand->GetCurrentCommand();
}

int MainInterface::OnCommandSingleParamI( int ival )
{
	stringstream ss;
	ss << ival;
	return OnCommitCommand(ss.str().c_str());
}

int MainInterface::OnCommandSingleParamF( float fval )
{
	stringstream ss;
	ss << fixed << fval;
	return OnCommitCommand(ss.str().c_str());
}

int MainInterface::OnCommandSingleParamS( const char * sval )
{
	return OnCommitCommand(sval);
}

int MainInterface::OnCommandSingleParamSubCommand( int ssc )
{
	const char * sstr = Command::getInstance().GetSubCommandStr(ssc);
	if (strlen(sstr))
	{
		return OnCommitCommand(sstr);
	}
	else
	{
		DASSERT(true);
	}
	return 0;
}

int MainInterface::OnCommitCommand( const char * str )
{
	return Command::getInstance().CommitCommand(str);
}

void MainInterface::OnTreeLockChange( bool toLock )
{
//	parentview->GetMainFrame()->LockTreeChange(toLock);
}

void MainInterface::OnRebuildLayerTree( GObject * changebase, GObject * activeitem )
{
    QMainInterface::getInstance().GetPLayerTree()->RebuildTree(changebase, activeitem);
//	parentview->GetMainFrame()->RebuildLayerTree(changebase, activeitem);
}

list<GObject *> * MainInterface::OnGetSelectedNodes()
{
    return QMainInterface::getInstance().GetPLayerTree()->GetActiveNodes();
//	return parentview->GetMainFrame()->GetActiveNodes(pnextfromIndex);
    //	return NULL;
}

GObject *MainInterface::OnGetHoveringNode()
{
    return QMainInterface::getInstance().GetPLayerTree()->GetHoveringNode();
}

bool MainInterface::OnGetDragDroppedNodes( GLayer ** pLayerNode, GObject ** pAfterNode )
{
    return QMainInterface::getInstance().GetPLayerTree()->GetDragDroppedNodes(pLayerNode, pAfterNode);
//	return parentview->GetMainFrame()->GetDragDropNodes(pLayerNode, pAfterNode);
//	return false;
}

void MainInterface::OnSetActiveLayer_Internal( GLayer * pLayer )
{
//	return parentview->GetMainFrame()->SetActiveLayer_Internal(pLayer);
}

void MainInterface::DoResizeWindow()
{
	if (resizewindow_w >= 0 && resizewindow_h >= 0)
	{
		// No Command (internal process)
//		MoveWindow(hge->System_GetState(HGE_HWND), 0, 0, resizewindow_w, resizewindow_h, TRUE);
		hge->Gfx_Resize(resizewindow_w, resizewindow_h);
		GUICoordinate::getInstance().UpdateScreenMeasure();
		resizewindow_w = -1;
		resizewindow_h = -1;
	}
}

void MainInterface::OnSetActiveLayerFromUI()
{
	GObjectManager::getInstance().GetActiveLayerFromUI();
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
	sprintf_s(statusstr, M_STRMAX, "%.4f, %.4f, %.4f", pguic->GetCursorX_C(), pguic->GetCursorY_C(), pguic->GetScale());
	CallUpdateStatusBarText(IDS_STATUS_PANE1, statusstr);
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
	int dik=-1;

#define _SETVKBYKEY(DIK)	\
	if (hge->Input_GetDIKey(DIK, DIKEY_DOWN))	\
	{	\
		vk = i;	\
		dik = DIK;	\
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

	bool bctrl=false;
	bool bshift=false;
	bool balt=false;

	if (hge->Input_GetDIKey(DIK_LCONTROL) || hge->Input_GetDIKey(DIK_RCONTROL))
	{
		bctrl = true;
	}
	if (hge->Input_GetDIKey(DIK_LSHIFT) || hge->Input_GetDIKey(DIK_RSHIFT))
	{
		bshift = true;
	}
	if (hge->Input_GetDIKey(DIK_LMENU) || hge->Input_GetDIKey(DIK_RMENU))
	{
		balt = true;
	}

	if (bctrl)
	{
		if (dik == DIK_Z)
		{
			if (!balt)
			{
				if (!bshift)
				{
					Command::getInstance().CreateUnDoCommandCommit();
//					Command::getInstance().CreateCommandCommit(COMM_UNDO);
//					Command::getInstance().DoUnDo();
				}
				else
				{
					Command::getInstance().CreateReDoCommandCommit();
//					Command::getInstance().CreateCommandCommit(COMM_REDO);
//					Command::getInstance().DoReDo();
				}
			}
		}
		else if (dik == DIK_Y)
		{
			if (!balt && !bshift)
            {
                Command::getInstance().CreateReDoCommandCommit();
			}
		}
	}

	if (vk && !bctrl && !balt)
	{
//		parentview->GetMainFrame()->CallEnableFloatCommand(vk);
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

void MainInterface::SetPrecisionInt(int nVal)
{
    if (nVal > 100)
    {
        nVal = 100;
    }
    else if (nVal < 0)
    {
        nVal = 0;
    }
    nPrecision = nVal;
    GObjectManager::getInstance().OnPrecisionChanged();
}

void MainInterface::OnPushRevertable( const char * desc, const char * commandstr, int command )
{
    QMainInterface::getInstance().GetPHistoryTable()->AddHistory(desc, commandstr, command);
//	parentview->GetMainFrame()->AddHistory(desc, commandstr, command);
}

void MainInterface::OnUnDo( int step/*=1*/ )
{
    QMainInterface::getInstance().GetPHistoryTable()->ChangeCurrentHistory(-step);
//	parentview->GetMainFrame()->ChangeCurrentHistory(-step);
}

void MainInterface::OnReDo( int step/*=1*/ )
{
    QMainInterface::getInstance().GetPHistoryTable()->ChangeCurrentHistory(step);
//	parentview->GetMainFrame()->ChangeCurrentHistory(step);
}

void MainInterface::OnClearReDo(int ndelete)
{
    QMainInterface::getInstance().GetPHistoryTable()->ClearLaterHistory(ndelete);
//	parentview->GetMainFrame()->ClearLaterHistory(ndelete);
}

void MainInterface::OnClearUnDo(int ndelete)
{
    QMainInterface::getInstance().GetPHistoryTable()->ClearPreviousHistory(ndelete);
//	parentview->GetMainFrame()->ClearPreviousHistory(ndelete);
}

void MainInterface::OnClearPreviousHistory( int ndelete/*=1*/ )
{
    QMainInterface::getInstance().GetPHistoryTable()->ClearPreviousHistory(ndelete);
//	parentview->GetMainFrame()->ClearPreviousHistory(ndelete);
}

void MainInterface::CallUnDoReDo( int step )
{
	if (step)
	{
		if (step < 0)
		{
			/*
			for (int i=0; i<-step; i++)
			{
				Command::getInstance().CreateCommandCommit(COMM_UNDO);
			}
			*/
			Command::getInstance().CreateUnDoCommandCommit(-step);
//			Command::getInstance().DoUnDo(-step);
		}
		else
		{
			/*
			for (int i=0; i<step; i++)
			{
				Command::getInstance().CreateCommandCommit(COMM_REDO);
			}
			*/
			Command::getInstance().CreateReDoCommandCommit(step);
//			Command::getInstance().DoReDo(step);
		}
	}
}

void MainInterface::OnChangeMouseCursor( HWND hwnd, int mousecursor/*=-1*/ )
{
	MouseCursorManager::getInstance().ChangeCursor(hwnd, mousecursor);
}

float MainInterface::GetMouseX_S()
{
	return GUICoordinate::getInstance().GetCursorX_S();
}

float MainInterface::GetMouseY_S()
{
	return GUICoordinate::getInstance().GetCursorY_S();
}

float MainInterface::GetMouseX_C()
{
	return GUICoordinate::getInstance().GetCursorX_C();
}

float MainInterface::GetMouseY_C()
{
	return GUICoordinate::getInstance().GetCursorY_C();
}
