/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core system functions
*/

#include "hge_impl.h"

#if IF_PLATFORM(HPLATFORM_PSP)
 #include <pspkernel.h>
 #include <pspdebug.h>
 #include <pspdisplay.h>
 #include <pspsdk.h>
 #include <pspthreadman.h>
 #include <pspaudiocodec.h>
 #include <pspmpeg.h>
 #include <unistd.h>
#endif // PSP

#if IF_PLATFORM(HPLATFORM_IOS)
 #include <mach/mach_time.h>
#endif

#define LOWORDINT(n) ((int)((signed short)(LOWORD(n))))
#define HIWORDINT(n) ((int)((signed short)(HIWORD(n))))

#if IF_PLATFORM(HPLATFORM_WIN)
const char *WINDOW_CLASS_NAME = "HGE__WNDCLASS";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

int			nRef=0;
HGE_Impl*	pHGE=0;

#if IF_PLATFORM(HPLATFORM_WIN)
BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID)
{
	return TRUE;
}
#endif

HGE* CALL hgeCreate(int ver)
{
	if(ver==HGE_VERSION)
		return (HGE*)HGE_Impl::_Interface_Get();
	else
		return 0;
}

HGE_Impl* HGE_Impl::_Interface_Get()
{
	if(!pHGE) pHGE=new HGE_Impl();

	nRef++;

	return pHGE;
}

void CALL HGE_Impl::Release()
{
	nRef--;

	if(!nRef)
	{
		if(pHGE->hwnd) pHGE->System_Shutdown();
		Resource_RemoveAllPacks();
		delete pHGE;
		pHGE=0;

#if IF_PLATFORM(HPLATFORM_WIN) && IFNOT_FRAMWORK(HFRAMEWORK_QT)
 #ifdef _DEBUG
  m_dumpMemoryReport();
 #endif // _DEBUG
#elif IF_PLATFORM(HPLATFORM_PSP)
	sceKernelDelayThread(500000);
	sceKernelExitGame();
#endif
	}
}

#if IF_PLATFORM(HPLATFORM_PSP)
/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
	sceKernelDelayThread(500000);
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}
#endif // PSP

bool CALL HGE_Impl::System_Initiate()
{
	System_Log("HGE Started..\n");

	System_Log("HGE version: %X.%X", HGE_VERSION>>8, HGE_VERSION & 0xFF);

#if IF_PLATFORM(HPLATFORM_WIN)

	if (bOwnWindow)
	{
		OSVERSIONINFO	os_ver;
		SYSTEMTIME		tm;
		MEMORYSTATUS	mem_st;
		WNDCLASS		winclass;
		int				width, height;

		// Log system info

		GetLocalTime(&tm);
		System_Log("Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);

		System_Log("Application: %s",szWinTitle);
		os_ver.dwOSVersionInfoSize=sizeof(os_ver);
		GetVersionEx(&os_ver);
		System_Log("OS: Windows %ld.%ld.%ld",os_ver.dwMajorVersion,os_ver.dwMinorVersion,os_ver.dwBuildNumber);

		GlobalMemoryStatus(&mem_st);
		System_Log("Memory: %ldK total, %ldK free\n",mem_st.dwTotalPhys/1024L,mem_st.dwAvailPhys/1024L);

		// Register window class

		winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		winclass.lpfnWndProc	= WindowProc;
		winclass.cbClsExtra		= 0;
		winclass.cbWndExtra		= 0;
		winclass.hInstance		= hInstance;
		winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
		winclass.lpszMenuName	= NULL;
		winclass.lpszClassName	= WINDOW_CLASS_NAME;
		if(szIcon) winclass.hIcon = LoadIcon(hInstance, szIcon);
		else winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClass(&winclass)) {
			_PostError("Can't register window class");
			return false;
		}

		// Create window

		width=nScreenWidth + GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		height=nScreenHeight + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

		/************************************************************************/
		/* These parameters are changed by h5nc (h5nc@yahoo.com.cn)             */
		/************************************************************************/
		rectW.left=(GetSystemMetrics(SM_CXSCREEN)-width)/4;
		rectW.top=(GetSystemMetrics(SM_CYSCREEN)-height)/4;
		rectW.right=rectW.left+width;
		rectW.bottom=rectW.top+height;
		styleW=WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE; //WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

		rectFS.left=(GetSystemMetrics(SM_CXFULLSCREEN)-nScreenWidth)/2;
		rectFS.top=(GetSystemMetrics(SM_CYFULLSCREEN)-nScreenHeight)/2;
		rectFS.right=rectFS.left+nScreenWidth;
		rectFS.bottom=rectFS.top+nScreenHeight;
		styleFS=WS_POPUP|WS_VISIBLE; //WS_POPUP

		if(hwndParent)
		{
			rectW.left=0;
			rectW.top=0;
			rectW.right=nScreenWidth;
			rectW.bottom=nScreenHeight;
			styleW=WS_CHILD|WS_VISIBLE;
			bWindowed=true;
		}

		if(bWindowed)
			hwnd = CreateWindowEx(0, WINDOW_CLASS_NAME, szWinTitle, styleW,
			rectW.left, rectW.top, rectW.right-rectW.left, rectW.bottom-rectW.top,
			hwndParent, NULL, hInstance, NULL);
		else
			hwnd = CreateWindowEx(WS_EX_TOPMOST, WINDOW_CLASS_NAME, szWinTitle, styleFS,
			0, 0, 0, 0,
			NULL, NULL, hInstance, NULL);
		if (!hwnd)
		{
			_PostError("Can't create window");
			return false;
		}

		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
 #if IFNOT_FRAMWORK(HFRAMEWORK_QT)
		hwnd = (HWND)1;
 #endif
	}

	// Init subsystems

	timeBeginPeriod(1);
#else
 #if IFNOT_FRAMWORK(HFRAMEWORK_QT)
	hwnd = (HWND)1;
	if (!bOwnWindow)
	{
		hwndParent = (HWND)1;
	}
 #endif

 #if IF_PLATFORM(HPLATFORM_PSP)
	pspDebugScreenInit();
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

 #endif // PSP

#endif //WIN
	Random_Seed();
	_InputInit();

	/************************************************************************/
	/* These lines are added by h5nc (h5nc@yahoo.com.cn)                    */
	/************************************************************************/
	// begin
#if IF_RENDERSYS(HRENDERSYS_DX)
	for (int i=0; i<DIJOY_MAXDEVICE; i++)
	{
		haveJoy[i] = true;
	}

	ZeroMemory(joyState, sizeof(DIJOYSTATE)*DIJOY_MAXDEVICE);
	ZeroMemory(lastJoyState, sizeof(DIJOYSTATE)*DIJOY_MAXDEVICE);
#endif
	if (bUseDInput)
	{
		int diinitret = _DIInit();
	}
	_TouchInit();

	if(!_GfxInit()) { System_Shutdown(); return false; }
	if(!_SoundInit()) { System_Shutdown(); return false; }
	// end
	System_Log("Init done.\n");

	/************************************************************************/
	/* These parameters are changed by h5nc (h5nc@yahoo.com.cn)             */
	/************************************************************************/
	fTime=0.0f;
	fFPS=0;
	nFrameCounter = 0;
	nFrameSkip = 0;
	nRenderSkip = 0;

	return true;
}

void CALL HGE_Impl::System_Shutdown()
{
	if (bManageLoop) {
		_System_StartPost();
	}

	System_Log("\nFinishing..");
#if IF_PLATFORM(HPLATFORM_WIN)
	timeEndPeriod(1);
	if(hSearch) { FindClose(hSearch); hSearch=0; }
#endif
	_ClearQueue();
	_SoundDone();

	/************************************************************************/
	/* Blocks marked with "Yuki" are originally her codes                   */
	/* h5nc (h5nc@yahoo.com.cn) copied these codes under her permission     */
	/************************************************************************/
	/* add by Yuki */
	// begin
	_FontDone();
    // end

	/************************************************************************/
	/* This call is added by h5nc (h5nc@yahoo.com.cn)                       */
	/************************************************************************/
	_DIRelease();
	_TouchRelease();

	_GfxDone();

	if(hwnd)
	{
		//ShowWindow(hwnd, SW_HIDE);
		//SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		//ShowWindow(hwnd, SW_SHOW);
#if IF_PLATFORM(HPLATFORM_WIN)
 #if IFNOT_FRAMWORK(HFRAMEWORK_QT)
		if (bOwnWindow)
		{
			DestroyWindow(hwnd);
		}
 #endif
#endif
		hwnd=0;
	}
#if IF_PLATFORM(HPLATFORM_WIN)

 #if IFNOT_FRAMWORK(HFRAMEWORK_QT)
	if(hInstance) UnregisterClass(WINDOW_CLASS_NAME, hInstance);
 #endif
#endif

	System_Log("The End.");
}

bool CALL HGE_Impl::System_Start()
{
	static bool _bFirstSystemStart = true;
	if (bManageLoop)
	{
		if (!_System_StartPre())
		{
			return false;
		}
		for (; ; )
		{
			if (!_System_StartLoop())
			{
				return false;
			}
//			Sleep(1);
		}
		_System_StartPost();
	}
	else
	{
		if (_bFirstSystemStart) {
			if (!_System_StartPre())
			{
				return false;
			}
			_bFirstSystemStart = false;
		}
		if (!_System_StartLoop())
		{
			return false;
		}
	}

	return true;
}

void CALL HGE_Impl::System_SetStateBool(hgeBoolState state, bool value)
{
	switch(state)
	{
		case HGE_WINDOWED:
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(VertArray || hwndParent) break;
								if(pD3DDevice && bWindowed != value)
								{
									if(d3dppW.BackBufferFormat==D3DFMT_UNKNOWN || d3dppFS.BackBufferFormat==D3DFMT_UNKNOWN) break;

//									if(procFocusLostFunc) procFocusLostFunc();

									if(bWindowed) GetWindowRect(hwnd, &rectW);
									bWindowed=value;
									if(bWindowed) d3dpp=&d3dppW;
									else d3dpp=&d3dppFS;

									if(_format_id(d3dpp->BackBufferFormat) < 4) nScreenBPP=16;
									else nScreenBPP=32;

									_GfxRestore();
									_AdjustWindow();

//									if(procFocusGainFunc) procFocusGainFunc();
								}
								else
								{
									bWindowed=value;
								}
#else
								bWindowed = value;
#endif
								break;
		case HGE_ZBUFFER:
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(!pD3DDevice)
#endif
									bZBuffer=value;
								break;

		case HGE_TEXTUREFILTER: bTextureFilter=value;
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(pD3DDevice)
								{
									_render_batch();
									if(bTextureFilter)
									{
//										pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
//										pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
										pD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
										pD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
									}
									else
									{
//										pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
//										pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
										pD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
										pD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
									}
								}
#endif
								break;

		case HGE_USESOUND:		if(bUseSound!=value)
								{
									bUseSound=value;
									if(bUseSound && hwnd) _SoundInit();
									if(!bUseSound && hwnd) _SoundDone();
								}
								break;

		case HGE_HIDEMOUSE:		bHideMouse=value; break;

		case HGE_DONTSUSPEND:	bDontSuspend=value; break;

		case HGE_2DMODE:
								if (VertArray && b2DMode != value)
								{
									_render_batch();
								}
								b2DMode=value;
								break;
		case HGE_MANAGELOOP:	bManageLoop=value; break;
		case HGE_USEDINPUT:		bUseDInput=value; break;
		case HGE_NOWMPAINT:		bNoWMPaint=value; break;
		case HGE_OWNWINDOW:		bOwnWindow=value; break;
		case HGE_CALLRENDER:	bCallRender=value; break;

			/************************************************************************/
			/* HGE_SHOWSPLASH case is deleted by h5nc (h5nc@yahoo.com.cn)           */
			/************************************************************************/
	}
}

void CALL HGE_Impl::System_SetStateFunc(hgeFuncState state, hgeCallback value)
{
	switch(state)
	{
		case HGE_FRAMEFUNC:		procFrameFunc=value; break;
		case HGE_RENDERFUNC:	procRenderFunc=value; break;
		case HGE_FOCUSLOSTFUNC:	procFocusLostFunc=value; break;
		case HGE_FOCUSGAINFUNC:	procFocusGainFunc=value; break;
		case HGE_GFXRESTOREFUNC: procGfxRestoreFunc=value; break;
		case HGE_EXITFUNC:		procExitFunc=value; break;

		case HGE_LOADTEXTUREFUNC:	procLoadTextureFunc=value;	break;
	}
}

void CALL HGE_Impl::System_SetStateHwnd(hgeHwndState state, HWND value)
{
	switch(state)
	{
	case HGE_HWND:			if(!hwnd) hwnd=value; break;
	case HGE_HWNDPARENT:	if(!hwnd) hwndParent=value; break;
	}
}

void CALL HGE_Impl::System_SetStateInt(hgeIntState state, int value)
{
	switch(state)
	{
	case HGE_SCREENWIDTH:
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(!pD3DDevice)
#endif
									nScreenWidth=value; break;

	case HGE_SCREENHEIGHT:
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(!pD3DDevice)
#endif
									nScreenHeight=value; break;

	case HGE_SCREENBPP:
#if IF_RENDERSYS(HRENDERSYS_DX)
								if(!pD3DDevice)
#endif
									nScreenBPP=value; break;

		case HGE_SAMPLERATE:	if(!hBass) nSampleRate=value;
								break;

		case HGE_FXVOLUME:		nFXVolume=value;
								_SetFXVolume(nFXVolume);
								break;

		case HGE_SAMPLEVOLUME:	nSampleVolume=value;
								_SetSampleVolume(nSampleVolume);
								break;
		case HGE_STREAMVOLUME:	nStreamVolume=value;
								_SetStreamVolume(nStreamVolume);
								break;

		case HGE_FPS:			if(VertArray) break;

#if IF_RENDERSYS(HRENDERSYS_DX)
								if(pD3DDevice)
								{
									if((nHGEFPS>=0 && value <0) || (nHGEFPS<0 && value>=0))
									{
										/************************************************************************/
										/* These blocks are added by h5nc (h5nc@yahoo.com.cn)                   */
										/************************************************************************/
										if(value==HGEFPS_VSYNC)
										{
//											d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
											d3dppW.SwapEffect = D3DSWAPEFFECT_DISCARD;
//											d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
											d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
										}
										else
										{
											d3dppW.SwapEffect = D3DSWAPEFFECT_DISCARD;
//											d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
											d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
										}
//										if(procFocusLostFunc) procFocusLostFunc();
										_GfxRestore();
										_DIRelease();
//										if(procFocusGainFunc) procFocusGainFunc();
										_DIInit();
									}
								}
#endif
								nHGEFPS=value;
								break;

								/************************************************************************/
								/* These cases are added by h5nc (h5nc@yahoo.com.cn)                    */
								/************************************************************************/
		case HGE_FRAMESKIP:		nFrameSkip = value;
								break;
		case HGE_RENDERSKIP:	nRenderSkip = value;
								break;
	}
}

void CALL HGE_Impl::System_SetStateString(hgeStringState state, const char *value)
{
	FILE *hf;

	switch(state)
	{
		case HGE_ICON:			szIcon=value;
#if IF_PLATFORM(HPLATFORM_WIN)
								if(hwnd) {if (bOwnWindow) {SetClassLong(pHGE->hwnd, GCL_HICON, (LONG)LoadIcon(pHGE->hInstance, szIcon));}};
#endif
								break;
		case HGE_TITLE:			strcpy(szWinTitle,value);
#if IF_PLATFORM(HPLATFORM_WIN)
								if(hwnd) {if (bOwnWindow) {SetWindowText(pHGE->hwnd, szWinTitle);}};
#endif
								break;
		case HGE_INIFILE:		if(value) strcpy(szIniFile,Resource_MakePath(value));
								else szIniFile[0]=0;
								break;
		case HGE_LOGFILE:		if(value)
								{
									strcpy(szLogFile,Resource_MakePath(value));
									hf=fopen(szLogFile, "w");
									if(!hf) szLogFile[0]=0;
									else fclose(hf);
								}
								else szLogFile[0]=0;
								break;

								/************************************************************************/
								/* This case is added by h5nc (h5nc@yahoo.com.cn)                       */
								/************************************************************************/
		case HGE_BASSDLLFILE:	if (value)
								{
									strcpy(szBassDllFile,Resource_MakePath(value));
								}
								break;
	}
}

bool CALL HGE_Impl::System_GetStateBool(hgeBoolState state)
{
	switch(state)
	{
		case HGE_WINDOWED:		return bWindowed;
		case HGE_ZBUFFER:		return bZBuffer;
		case HGE_TEXTUREFILTER:	return bTextureFilter;
		case HGE_USESOUND:		return bUseSound;
		case HGE_DONTSUSPEND:	return bDontSuspend;
		case HGE_HIDEMOUSE:		return bHideMouse;
		case HGE_2DMODE:		return b2DMode;
		case HGE_MANAGELOOP:	return bManageLoop;
		case HGE_USEDINPUT:		return bUseDInput;
		case HGE_NOWMPAINT:		return bNoWMPaint;
		case HGE_OWNWINDOW:		return bOwnWindow;
		case HGE_CALLRENDER:	return bCallRender;

			/************************************************************************/
			/* HGE_SHOWSPLASH case is deleted by h5nc (h5nc@yahoo.com.cn)           */
			/************************************************************************/
	}
	return false;
}

hgeCallback CALL HGE_Impl::System_GetStateFunc(hgeFuncState state)
{
	switch(state)
	{
		case HGE_FRAMEFUNC:		return procFrameFunc;
		case HGE_RENDERFUNC:	return procRenderFunc;
		case HGE_FOCUSLOSTFUNC:	return procFocusLostFunc;
		case HGE_FOCUSGAINFUNC:	return procFocusGainFunc;
		case HGE_EXITFUNC:		return procExitFunc;

		case HGE_LOADTEXTUREFUNC:	return procLoadTextureFunc;
	}

	return NULL;
}

HWND CALL HGE_Impl::System_GetStateHwnd(hgeHwndState state)
{
	switch(state)
	{
		case HGE_HWND:			return hwnd;
		case HGE_HWNDPARENT:	return hwndParent;
	}

	return 0;
}

int CALL HGE_Impl::System_GetStateInt(hgeIntState state)
{
	switch(state)
	{
		case HGE_SCREENWIDTH:	return nScreenWidth;
		case HGE_SCREENHEIGHT:	return nScreenHeight;
		case HGE_SCREENBPP:		return nScreenBPP;
		case HGE_SAMPLERATE:	return nSampleRate;
		case HGE_FXVOLUME:		return nFXVolume;
		case HGE_SAMPLEVOLUME:	return nSampleVolume;
		case HGE_STREAMVOLUME:	return nStreamVolume;
		case HGE_FPS:			return nHGEFPS;

			/************************************************************************/
			/* These cases are added by h5nc (h5nc@yahoo.com.cn)                    */
			/************************************************************************/
		case HGE_FRAMECOUNTER:	return nFrameCounter;
		case HGE_FRAMESKIP:		return nFrameSkip;
		case HGE_RENDERSKIP:	return nRenderSkip;
	}

	return 0;
}

const char* CALL HGE_Impl::System_GetStateString(hgeStringState state) {
	switch(state) {
		case HGE_ICON:			return szIcon;
		case HGE_TITLE:			return szWinTitle;
		case HGE_INIFILE:		if(szIniFile[0]) return szIniFile;
								else return 0;
		case HGE_LOGFILE:		if(szLogFile[0]) return szLogFile;
								else return 0;
	}

	return NULL;
}

char* CALL HGE_Impl::System_GetErrorMessage()
{
	return szError;
}

void CALL HGE_Impl::System_Log(const char *szFormat, ...)
{
	FILE *hf = NULL;
	va_list ap;

	if(!szLogFile[0]) return;
	hf = fopen(szLogFile, "a");
	if(!hf) return;

	va_start(ap, szFormat);
	vfprintf(hf, szFormat, ap);
#if IF_PLATFORM(HPLATFORM_IOS)
	char buffer[_MAX_PATH];
	vsprintf(buffer, szFormat, ap);
#endif
	va_end(ap);

	fprintf(hf, "\n");
	fclose(hf);
#if IF_PLATFORM(HPLATFORM_IOS)
	printf(buffer);
	printf("\n");
#endif
}

bool CALL HGE_Impl::System_Launch(const char *url)
{
#if IF_PLATFORM(HPLATFORM_WIN)
	if((DWORD)ShellExecute(pHGE->hwnd, NULL, url, NULL, NULL, SW_SHOWMAXIMIZED)>32) return true;
	else return false;
#else
	return true;
#endif
}

void CALL HGE_Impl::System_Snapshot(const char *filename)
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DSURFACE9 pSurf;
	char *shotname, tempname[_MAX_PATH];
	int i;

	if(!filename)
	{
		i=0;
		shotname=Resource_EnumFiles("SnapShot/shot???.bmp");
		while(shotname)
		{
			i++;
			shotname=Resource_EnumFiles();
		}
		sprintf(tempname, "SnapShot/shot%03d.bmp", i);
		filename=Resource_MakePath(tempname);
	}

	if(pD3DDevice)
	{
		//+1st
		pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
		D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurf, NULL, NULL);
		pSurf->Release();
	}
#endif
}

int CALL HGE_Impl::System_MessageBox(const char * text, const char * title, DWORD type)
{
#if IF_PLATFORM(HPLATFORM_WIN)
	return MessageBox(hwnd, text, title, type);
#elif IF_PLATFORM(HPLATFORM_PSP)
	pspDebugScreenPrintf("%s\n%s", title, text);
	return IDOK;
#else
	return IDOK;
#endif // WIN32
}

//////// Implementation ////////

HGE_Impl::HGE_Impl()
{
#if IF_PLATFORM(HPLATFORM_WIN)
	hInstance=GetModuleHandle(0);
#else
	hInstance=(HINSTANCE)1;
#endif
	hwnd=0;

	/************************************************************************/
	/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	bActive=true;
	szError[0]=0;
#if IF_RENDERSYS(HRENDERSYS_DX)
	pD3D=0;
	pD3DDevice=0;
	d3dpp=0;
	pScreenSurf=0;
	pScreenDepth=0;
	pVB=NULL;
	pIB=NULL;
#endif
	pTargets=0;
	pCurTarget=0;

	VertArray=0;
	textures=0;

	hBass=0;
	bSilent=false;
	streams=0;

#if IF_PLATFORM(HPLATFORM_WIN)
	hSearch=0;
#endif
	res=0;

	queue=0;
	Char=VKey=ZVpos=ZHpos=0;
	Xpos=Ypos=0.0f;
	bMouseOver=false;
	bCaptured=false;

	nHGEFPS=HGEFPS_UNLIMITED;
	fTime=0.0f;
	fDeltaTime=0.0f;
	/************************************************************************/
	/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	fFPS=0;

	/************************************************************************/
	/* This parameter is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
	b2DMode = true;
	bManageLoop = true;
	bUseDInput = true;
	bNoWMPaint = false;
	bOwnWindow = true;
	bCallRender = true;

	procFrameFunc=NULL;
	procRenderFunc=NULL;
	procFocusLostFunc=NULL;
	procFocusGainFunc=NULL;
	procGfxRestoreFunc=NULL;
	procExitFunc=NULL;

	procLoadTextureFunc=NULL;
	textoload = NULL;

	szIcon=NULL;
	strcpy(szWinTitle,"HGE");
	nScreenWidth=800;
	nScreenHeight=600;
	nScreenBPP=32;
	bWindowed=false;
	bZBuffer=false;
	bTextureFilter=true;
	szLogFile[0]=0;
	szIniFile[0]=0;

	/************************************************************************/
	/* This parameter is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
	strcpy(szBassDllFile, "BASS.dll");
	bUseSound=true;
	nSampleRate=44100;
	nFXVolume=10000;
	nSampleVolume=10000;
	nStreamVolume=10000;
	bHideMouse=true;
	bDontSuspend=false;
	hwndParent=0;

#if IF_PLATFORM(HPLATFORM_WIN)
	GetModuleFileName(GetModuleHandle(NULL), szAppPath, sizeof(szAppPath));
#else
	strcpy(szAppPath, "");
#endif

	int i;
	for(i=strlen(szAppPath)-1; i>0; i--) if(szAppPath[i]==M_FOLDER_SLASH) break;
	szAppPath[i+1]=0;
	/************************************************************************/
	/* This parameter is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
	strcpy(szResourcePath, szAppPath);

	/************************************************************************/
	/* This parameter is added by Yuki                                      */
	/************************************************************************/
	/* add by Yuki */
	fontList = 0;
}

void HGE_Impl::_PostError(char *error)
{
	System_Log(error);
	strcpy(szError,error);
}

void HGE_Impl::_FocusChange(bool bAct)
{
	bActive=bAct;

	/************************************************************************/
	/* These blocks are modified by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	if(bActive) {
		_GfxRestore();
		if(procFocusGainFunc) procFocusGainFunc();
		_DIInit();
	}
	else
	{
		if(procFocusLostFunc) procFocusLostFunc();
		_DIRelease();
	}
}

bool HGE_Impl::_System_StartPre()
{
	if(!hwnd)
	{
		_PostError("System_Start: System_Initiate wasn't called");
		return false;
	}

	if(!procFrameFunc) {
		_PostError("System_Start: No frame function defined");
		return false;
	}

	bActive=true;

	return true;
}

bool HGE_Impl::_System_StartLoop()
{
	// Process window messages if not in "child mode"
	// (if in "child mode" the parent application will do this for us)

#if IF_PLATFORM(HPLATFORM_WIN)
	MSG		msg;
	if(!hwndParent)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	return false;
			// TranslateMessage(&msg);
			DispatchMessage(&msg);
			return true;
		}
	}

	if(IsIconic(hwnd))
		return true;
#endif

	// Check if mouse is over HGE window for Input_IsMouseOver

	_UpdateMouse();
	_TouchUpdate();

	// If HGE window is focused or we have the "don't suspend" state - process the main loop

	/************************************************************************/
	/* These lines are changed by h5nc (h5nc@yahoo.com.cn)                  */
	/* The core algorithm is based on LOVEHINA-AVC's code                   */
	/* h5nc copied his codes with his permission                            */
	/************************************************************************/

	if(bActive || bDontSuspend) {
		int DI_retv = 0;
		if(nFrameSkip < 0)
		{
			for(int i=0;i<-nFrameSkip;i++)
			{
				DI_retv = _DIUpdate();
				if(DI_retv & ERROR_NOJOYSTICK)
				{
					if(DI_retv & ERROR_NOKEYBOARD)
					{
						break;
					}
				}
				if(procFrameFunc())
				{
					return false;
				}
			}
		}
		else if(nFrameSkip < 2 || !(nFrameCounter % nFrameSkip))
		{
			DI_retv = _DIUpdate();
			if(procFrameFunc())
			{
				return false;
			}
		}
		/*
		if(DI_retv == (ERROR_NOJOYSTICK | ERROR_NOKEYBOARD))
			return true;
		*/
		if (bCallRender)
		{
			if(nRenderSkip < 2 || !(nFrameCounter % nRenderSkip))
			{
				if(procRenderFunc) procRenderFunc();
			}
		}

		// Clean up input events that were generated by
		// WindowProc and weren't handled by user's code
		/*
		if (!bManageLoop)
		{
			return true;
		}
		*/

		int bPriorityRaised = 0;
		LONGLONG TimeInterval, TimePrecision, NowTime;
		static LONGLONG lastTime = Timer_GetCurrentSystemTime();
		TimePrecision = Timer_GetPerformanceFrequency();

		if (nHGEFPS > 0)
		{
			TimeInterval = TimePrecision / nHGEFPS;
		}
		else
		{
			TimeInterval = 0;
		}

		if (bManageLoop)
		{
#if IF_PLATFORM(HPLATFORM_PSP)
			if (nHGEFPS == 60)
			{
				sceDisplayWaitVblankStart();
			}
			else
#endif
			{
				if (Timer_GetCurrentSystemTime() - lastTime >= TimeInterval)
				{
#if IF_PLATFORM(HPLATFORM_WIN)
					if (GetThreadPriority(GetCurrentThread()) > THREAD_PRIORITY_NORMAL)
						SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif // WIN32
				}
				else
				{
#if IFNOT_PLATFORM(HPLATFORM_IOS)
					for (;;)
					{
						if (Timer_GetCurrentSystemTime() - lastTime >= (TimeInterval - (TimePrecision / (1000 * 2))))
						{
 #if IF_PLATFORM(HPLATFORM_WIN)
							if (!bPriorityRaised && GetThreadPriority(GetCurrentThread()) > THREAD_PRIORITY_NORMAL)
								SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
 #endif
							for (;;)
							{
								if (Timer_GetCurrentSystemTime() - lastTime >= TimeInterval)
									break;
							}
							break;
						}
						else
						{
 #if IF_PLATFORM(HPLATFORM_WIN)
							if (!bPriorityRaised)
							{
								if (GetThreadPriority(GetCurrentThread()) < THREAD_PRIORITY_HIGHEST)
									SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
								bPriorityRaised = 1;
							}
							Sleep(1);
 #endif
						}
					}
#endif
				}
			}
		}

		NowTime = Timer_GetCurrentSystemTime();

		if (lastTime <= (NowTime - TimeInterval) && lastTime >= (NowTime - (TimeInterval * 2)))
		{
			fDeltaTime = (float)((double)(TimeInterval) / (double)(TimePrecision));
			lastTime += TimeInterval;
		}
		else
		{
			fDeltaTime = (float)((double)(NowTime - lastTime) / (double)(TimePrecision));
			lastTime = NowTime;
		}

		nFrameCounter++;
		fTime += fDeltaTime;
		fFPS = 1.0f / fDeltaTime;
	}

	// If main loop is suspended - just sleep a bit
	// (though not too much to allow instant window
	// redraw if requested by OS)

	else
	{
#if IF_PLATFORM(HPLATFORM_WIN)
		Sleep(1);
#elif IF_PLATFORM(HPLATFORM_PSP)
		sceKernelDelayThread(1000);
#elif IF_PLATFORM(HPLATFORM_IOS)
		mach_wait_until(mach_absolute_time()+1000);
#endif
	}

	if (!bManageLoop)
	{
		return true;
	}
//	if(hwndParent) return true;
	_ClearQueue();

	return true;
}

bool HGE_Impl::_System_StartPost()
{
	_ClearQueue();

	bActive=false;

	return true;
}

#if IF_PLATFORM(HPLATFORM_WIN)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool bActivating;

	switch(msg)
	{
		case WM_CREATE:
			return FALSE;

		case WM_PAINT:
#if IF_RENDERSYS(HRENDERSYS_DX)
			if (bCallRender)
			{
				if(pHGE->pD3D && pHGE->procRenderFunc && pHGE->bWindowed && !pHGE->bNoWMPaint) pHGE->procRenderFunc();
			}
#endif
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return FALSE;

		case WM_ACTIVATE:
			// tricky: we should catch WA_ACTIVE and WA_CLICKACTIVE,
			// but only if HIWORD(wParam) (fMinimized) == FALSE (0)
			bActivating = (LOWORD(wparam) != WA_INACTIVE) && (HIWORD(wparam) == 0);
#if IF_RENDERSYS(HRENDERSYS_DX)
			if(pHGE->pD3D && pHGE->bActive != bActivating) pHGE->_FocusChange(bActivating);
#endif
			return FALSE;

		case WM_SETCURSOR:
			if(pHGE->bActive && LOWORD(lparam)==HTCLIENT && pHGE->bHideMouse) SetCursor(NULL);
			else SetCursor(LoadCursor(NULL, IDC_ARROW));
			return FALSE;

		case WM_SYSKEYDOWN:
			if(wparam==VK_F4)
			{
				if(pHGE->procExitFunc && !pHGE->procExitFunc()) return FALSE;
						return DefWindowProc(hwnd, msg, wparam, lparam);
				}

			/************************************************************************/
			/* This block is deleted by h5nc (h5nc@yahoo.com.cn)                    */
			/************************************************************************/
			/*
			else if(wparam == VK_RETURN)
			{
				pHGE->System_SetState(HGE_WINDOWED, !pHGE->System_GetState(HGE_WINDOWED));
				return FALSE;
			}
			*/
			else
			{
				pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF, (lparam & 0x40000000) ? HGEINP_REPEAT:0, -1, -1);
				return FALSE;
			}

		case WM_KEYDOWN:
			pHGE->_BuildEvent(INPUT_KEYDOWN, wparam, HIWORD(lparam) & 0xFF, (lparam & 0x40000000) ? HGEINP_REPEAT:0, -1, -1);
			return FALSE;
		case WM_SYSKEYUP:
			pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
			return FALSE;
		case WM_KEYUP:
			pHGE->_BuildEvent(INPUT_KEYUP, wparam, HIWORD(lparam) & 0xFF, 0, -1, -1);
			return FALSE;

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_MBUTTONDOWN:
			SetFocus(hwnd);
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_RBUTTONDOWN:
			SetFocus(hwnd);
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;

		case WM_LBUTTONDBLCLK:
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_LBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_MBUTTONDBLCLK:
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_MBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_RBUTTONDBLCLK:
			pHGE->_BuildEvent(INPUT_MBUTTONDOWN, HGEK_RBUTTON, 0, HGEINP_REPEAT, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;

		case WM_LBUTTONUP:
			pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_LBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_MBUTTONUP:
			pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_MBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case WM_RBUTTONUP:
			pHGE->_BuildEvent(INPUT_MBUTTONUP, HGEK_RBUTTON, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;

		case WM_MOUSEMOVE:
			pHGE->_BuildEvent(INPUT_MOUSEMOVE, 0, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case 0x020A: // WM_MOUSEWHEEL, 120 : GET_WHEEL_DELTA_WPARAM(wparam);
			pHGE->_BuildEvent(INPUT_MOUSEWHEELV, short(HIWORD(wparam))/120, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;
		case 0x020E: // WM_MOUSEHWHEEL, 120 : GET_WHEEL_DELTA_WPARAM(wparam);
			pHGE->_BuildEvent(INPUT_MOUSEWHEELH, short(HIWORD(wparam))/120, 0, 0, LOWORDINT(lparam), HIWORDINT(lparam));
			return FALSE;

		case WM_SIZE:
			//			if(pHGE->pD3D && wparam==SIZE_RESTORED) pHGE->_Resize(LOWORD(lparam), HIWORD(lparam));
#if IF_RENDERSYS(HRENDERSYS_DX)
			if(pHGE->pD3D && wparam==SIZE_RESTORED) pHGE->Gfx_Resize(LOWORD(lparam), HIWORD(lparam));
#endif
			//return FALSE;
			break;

		case WM_SYSCOMMAND:
			if(wparam==SC_CLOSE)
			{
				if(pHGE->procExitFunc && !pHGE->procExitFunc()) return FALSE;
				pHGE->bActive=false;
				return DefWindowProc(hwnd, msg, wparam, lparam);
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}
#endif