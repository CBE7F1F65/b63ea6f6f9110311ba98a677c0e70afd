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
#include "MarkingManager.h"

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

	bLockLeftMouseDown = false;

    nPrecision = 50;
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
//	return MainInterface::getInstance().Render();
	return false;
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

	// Marking
	MarkingManager::getInstance().Render();

	// All Nodes
	GObjectManager::getInstance().Render();
	// Render Command Floating
	Command::getInstance().Render();
	// Render Snap
	GObjectPicker::getInstance().Render();

	// Render MarqueeSelect
	MarqueeSelect::getInstance().Render();

	// Render Indicator
	GObjectManager::getInstance().RenderIndication();

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
	/*
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
	*/
	/*
	if (QApplication::focusWidget() != QMainInterface::getInstance().GetPGLView())
	{
		if (hge->Input_GetDIMouseKey(cursorleftkeyindex, DIKEY_UP))
		{
			hge->Input_SetDIKey(cursorleftkeyindex);
		}
	}
	*/
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

	if (hge->Input_GetDIKey(DIK_END, DIKEY_UP))
	{
		bLockLeftMouseDown = !bLockLeftMouseDown;
	}
	if (bLockLeftMouseDown)
	{
		hge->Input_SetDIMouseKey(cursorleftkeyindex);
	}
	
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

	MarkingManager::getInstance().Update();

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

	parentview->OnFrameEnd();

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
	CommandTemplate::Release();
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

void MainInterface::CallShowNodeInfo(GObject *pObj, bool bContext)
{
    QMainInterface::getInstance().GetPNodeInfoFloatingTree()->ShowNodeInfo(pObj, bContext);
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
				string * vais = (string *)va_arg(ap, string *);
				ASSERT(vais);
				OnCommandSingleParamS(vais->c_str());
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
// 					DASSERT(false);
// 				}
			}
			break;
		default:
			ASSERT(false);
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
		DASSERT(false);
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
}

list<GObject *> * MainInterface::OnGetSelectedNodes()
{
    return QMainInterface::getInstance().GetPLayerTree()->GetActiveNodes();
}

GObject *MainInterface::OnGetHoveringNode()
{
    QMainInterface * pqm = &QMainInterface::getInstance();
    if (!pqm->GetPNodeInfoFloatingWidget()->isHidden())
    {
        return pqm->GetPNodeInfoFloatingTree()->GetHoveringNode();
    }
    return pqm->GetPLayerTree()->GetHoveringNode();
}

bool MainInterface::OnGetDragDroppedNodes( GLayer ** pLayerNode, GObject ** pAfterNode )
{
    return QMainInterface::getInstance().GetPLayerTree()->GetDragDroppedNodes(pLayerNode, pAfterNode);
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
