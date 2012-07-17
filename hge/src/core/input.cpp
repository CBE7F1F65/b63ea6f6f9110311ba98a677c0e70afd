/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: input
*/

/************************************************************************/
/* This define is added by h5nc (h5nc@yahoo.com.cn)                     */
/************************************************************************/
#define INITGUID

#include "hge_impl.h"

#if IF_PLATFORM(HPLATFORM_PSP)
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#endif // PSP

/************************************************************************/
/* These header files are added by h5nc (h5nc@yahoo.com.cn)             */
/************************************************************************/
//#include <objbase.h>

GUID HGE_Impl::joyGuid[DIJOY_MAXDEVICE];

char *KeyNames[] =
{
 "?",
 "Left Mouse Button", "Right Mouse Button", "?", "Middle Mouse Button",
 "?", "?", "?", "Backspace", "Tab", "?", "?", "?", "Enter", "?", "?",
 "Shift", "Ctrl", "Alt", "Pause", "Caps Lock", "?", "?", "?", "?", "?", "?",
 "Escape", "?", "?", "?", "?",
 "Space", "Page Up", "Page Down", "End", "Home",
 "Left Arrow", "Up Arrow", "Right Arrow", "Down Arrow",
 "?", "?", "?", "?", "Insert", "Delete", "?",
 "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
 "?", "?", "?", "?", "?", "?", "?",
 "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
 "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
 "Left Win", "Right Win", "Application", "?", "?",
 "NumPad 0", "NumPad 1", "NumPad 2", "NumPad 3", "NumPad 4",
 "NumPad 5", "NumPad 6", "NumPad 7", "NumPad 8", "NumPad 9",
 "Multiply", "Add", "?", "Subtract", "Decimal", "Divide",
 "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "Num Lock", "Scroll Lock",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "Semicolon", "Equals", "Comma", "Minus", "Period", "Slash", "Grave",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?",
 "Left bracket", "Backslash", "Right bracket", "Apostrophe",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
 "?", "?", "?"
};

bool CALL HGE_Impl::Input_GetEvent(hgeInputEvent *event)
{
	CInputEventList *eptr;

	if(queue)
	{
		eptr=queue;
		memcpy(event, &eptr->event, sizeof(hgeInputEvent));
		queue=eptr->next;
		delete eptr;
		return true;
	}

	return false;
}

void CALL HGE_Impl::Input_GetMousePos(float *x, float *y)
{
	*x=Xpos; *y=Ypos;
}

void CALL HGE_Impl::Input_SetMousePos(float x, float y)
{
#if IF_PLATFORM(HPLATFORM_WIN)
	POINT pt;
	pt.x=(long)x; pt.y=(long)y;

	Xpos = x;	Ypos = y;

	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x,pt.y);
#endif
}

int CALL HGE_Impl::Input_GetMouseWheel(bool bHorz)
{
	if (bHorz)
	{
		return ZHpos;
	}
	return ZVpos;
}

void CALL HGE_Impl::Input_SetMouseWheel( int inc, bool bHorz/*=false*/ )
{
	if (bHorz)
	{
		ZHpos += inc;
	}
	else
	{
		ZVpos += inc;
	}
}

bool CALL HGE_Impl::Input_IsMouseOver()
{
	return bMouseOver;
}

bool CALL HGE_Impl::Input_GetKeyState(int key)
{
	/************************************************************************/
	/* This condition is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
#if IF_PLATFORM(HPLATFORM_WIN)
	if(bActive)
		return ((GetKeyState(key) & 0x8000) != 0);
#endif
	return false;
}

bool CALL HGE_Impl::Input_KeyDown(int key)
{
	/************************************************************************/
	/* This condition is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
	if(bActive)
		return (keyz[key] & 1) != 0;
	return false;
}

bool CALL HGE_Impl::Input_KeyUp(int key)
{
	/************************************************************************/
	/* This condition is added by h5nc (h5nc@yahoo.com.cn)                  */
	/************************************************************************/
	if(bActive)
		return (keyz[key] & 2) != 0;
	return false;
}

char* CALL HGE_Impl::Input_GetKeyName(int key)
{
	return KeyNames[key];
}

int CALL HGE_Impl::Input_GetKey()
{
	return VKey;
}

int CALL HGE_Impl::Input_GetChar()
{
	return Char;
}

//////// Implementation ////////

void HGE_Impl::_InputInit()
{
#if IF_PLATFORM(HPLATFORM_WIN)
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	Xpos = (float)pt.x;
	Ypos = (float)pt.y;
#else
	Xpos = 0;
	Ypos = 0;
#endif

	memset(&keyz, 0, sizeof(keyz));

	memset(touchesState, 0, sizeof(touchesState));
}

/************************************************************************/
/* These functions are added by h5nc (h5nc@yahoo.com.cn)                */
/************************************************************************/
// begin
bool CALL HGE_Impl::Input_GetDIKey(int key, BYTE stateType)
{
	if (!bUseDInput)
	{
		return false;
	}

	if(key >= 0 && key < 256)
	{
		switch(stateType)
		{
		case DIKEY_PRESSED:
			if(keyState[key])
				return true;
			break;
		case DIKEY_DOWN:
			if(keyState[key] && !lastKeyState[key])
				return true;
			break;
		case DIKEY_UP:
			if(!keyState[key] && lastKeyState[key])
				return true;
			break;
		default:
			return false;
		}
	}
	return false;
}

bool CALL HGE_Impl::Input_SetDIKey(int key, bool set)
{
	if (!bUseDInput)
	{
		return true;
	}

	if(key >=0 && key < 256)
	{
		if(set)
			keyState[key] |= 1<<7;
		else
			keyState[key] = 0;
	}
	return true;
}

bool CALL HGE_Impl::Input_ClearLastDIKeyState()
{
	if (!bUseDInput)
	{
		return true;
	}
	ZeroMemory(lastKeyState, sizeof(BYTE)*256);
	return true;
}

bool CALL HGE_Impl::Input_GetDIMouseKey(int key, BYTE stateType /* = DIKEY_PRESSED */)
{
	if (!bUseDInput)
	{
		return false;
	}
	if(key >= 0 && key < nMouseButtons)
	{
		switch(stateType)
		{
		case DIKEY_PRESSED:
			if(mouseState.rgbButtons[key])
				return true;
			break;
		case DIKEY_DOWN:
			if(mouseState.rgbButtons[key] && !lastMouseState.rgbButtons[key])
				return true;
			break;
		case DIKEY_UP:
			if(!mouseState.rgbButtons[key] && lastMouseState.rgbButtons[key])
				return true;
			break;
		default:
			return false;
		}
	}
	return false;
}

bool CALL HGE_Impl::Input_SetDIMouseKey(int key, bool set /* = true */, bool bLast/*=false*/)
{
	if (!bUseDInput)
	{
		return true;
	}
	if(key >=0 && key < nMouseButtons)
	{
		if(set)
		{
			if (bLast)
			{
				lastMouseState.rgbButtons[key] |= 1<<7;
			}
			else
			{
				mouseState.rgbButtons[key] |= 1<<7;
			}
		}
		else
		{
			if (bLast)
			{
				lastMouseState.rgbButtons[key] = 0;
			}
			else
			{
				mouseState.rgbButtons[key] = 0;
			}
		}
	}
	return true;
}

LONG CALL HGE_Impl::Input_GetDIMouseAxis(int axis, bool relative/* =false */)
{
	if (!bUseDInput)
	{
		return 0;
	}
	if (axis >= 0 && axis < nMouseAxes)
	{
		switch (axis)
		{
		case DIMOUSE_X:
			if (relative)
			{
				return mouseState.lX - lastMouseState.lX;
			}
			return mouseState.lX;
		case DIMOUSE_Y:
			if (relative)
			{
				return mouseState.lY - lastMouseState.lY;
			}
			return mouseState.lY;
		case DIMOUSE_Z:
			if (relative)
			{
				return mouseState.lZ - lastMouseState.lZ;
			}
			return mouseState.lZ;
		}
	}
	return 0;
}

bool CALL HGE_Impl::Input_SetDIMouseAxis(int axis, LONG value, bool relative/* =false */)
{
	if (!bUseDInput)
	{
		return true;
	}
	if (axis >= 0 && axis < nMouseAxes)
	{
		switch (axis)
		{
		case DIMOUSE_X:
			mouseState.lX = value;
			if (relative)
			{
				mouseState.lX += lastMouseState.lX;
			}
			break;
		case DIMOUSE_Y:
			mouseState.lY = value;
			if (relative)
			{
				mouseState.lY += lastMouseState.lY;
			}
			break;
		case DIMOUSE_Z:
			mouseState.lZ = value;
			if (relative)
			{
				mouseState.lZ += lastMouseState.lZ;
			}
			break;
		}
	}
	return true;
}

bool CALL HGE_Impl::Input_ClearLastDIMouseState()
{
	if (!bUseDInput)
	{
		return true;
	}
	ZeroMemory(&lastMouseState, sizeof(DIMOUSESTATE2));
	return true;
}

bool CALL HGE_Impl::Input_GetDIJoy(int joy, BYTE stateType /* = DIKEY_PRESSED */, int joydevice/* =0 */)
{
	if (!bUseDInput)
	{
		return false;
	}
	if(joy >=0 && joy < 32)
	{
		switch(stateType)
		{
		case DIKEY_PRESSED:
			if(joyState[joydevice].rgbButtons[joy])
				return true;
			break;
		case DIKEY_DOWN:
			if(joyState[joydevice].rgbButtons[joy] && !lastJoyState[joydevice].rgbButtons[joy])
				return true;
			break;
		case DIKEY_UP:
			if(!joyState[joydevice].rgbButtons[joy] && lastJoyState[joydevice].rgbButtons[joy])
				return true;
			break;
		default:
			return false;
		}
	}
	else
	{
		switch(joy)
		{
		case JOY_LEFT:
			switch(stateType)
			{
			case DIKEY_PRESSED:
				if(joyState[joydevice].lX < 0)
					return true;
				break;
			case DIKEY_DOWN:
				if(joyState[joydevice].lX < 0 && !(lastJoyState[joydevice].lX < 0))
					return true;
				break;
			case DIKEY_UP:
				if(!(joyState[joydevice].lX < 0) && lastJoyState[joydevice].lX < 0)
					return true;
				break;
			default:
				return false;
			}
			break;
		case JOY_RIGHT:
			switch(stateType)
			{
			case DIKEY_PRESSED:
				if(joyState[joydevice].lX > 0)
					return true;
				break;
			case DIKEY_DOWN:
				if(joyState[joydevice].lX > 0 && !(lastJoyState[joydevice].lX > 0))
					return true;
				break;
			case DIKEY_UP:
				if(!(joyState[joydevice].lX > 0) && lastJoyState[joydevice].lX > 0)
					return true;
				break;
			default:
				return false;
			}
			break;
		case JOY_UP:
			switch(stateType)
			{
			case DIKEY_PRESSED:
				if(joyState[joydevice].lY < 0)
					return true;
				break;
			case DIKEY_DOWN:
				if(joyState[joydevice].lY < 0 && !(lastJoyState[joydevice].lY < 0))
					return true;
				break;
			case DIKEY_UP:
				if(!(joyState[joydevice].lY < 0) && lastJoyState[joydevice].lY < 0)
					return true;
				break;
			default:
				return false;
			}
			break;
		case JOY_DOWN:
			switch(stateType)
			{
			case DIKEY_PRESSED:
				if(joyState[joydevice].lY > 0)
					return true;
				break;
			case DIKEY_DOWN:
				if(joyState[joydevice].lY > 0 && !(lastJoyState[joydevice].lY > 0))
					return true;
				break;
			case DIKEY_UP:
				if(!(joyState[joydevice].lY > 0) && lastJoyState[joydevice].lY > 0)
					return true;
				break;
			default:
				return false;
			}
			break;
		default:
			return false;
		}
	}
	return false;
}

bool CALL HGE_Impl::Input_ClearLastDIJoyState( int joydevice/*=0*/ )
{
	if (!bUseDInput)
	{
		return true;
	}
	ZeroMemory(&(lastJoyState[joydevice]), sizeof(DIJOYSTATE));
	return true;
}

// end

void HGE_Impl::_UpdateMouse()
{
#if IF_PLATFORM(HPLATFORM_WIN)
	POINT	pt;
	RECT	rc;

	GetCursorPos(&pt);
	GetClientRect(hwnd, &rc);
	MapWindowPoints(hwnd, NULL, (LPPOINT)&rc, 2);

	if(bCaptured || (PtInRect(&rc, pt) && WindowFromPoint(pt)==hwnd))
		bMouseOver=true;
	else
		bMouseOver=false;
#endif
}

void HGE_Impl::_BuildEvent(int type, int key, int scan, int flags, int x, int y)
{
#if IF_PLATFORM(HPLATFORM_WIN)
	CInputEventList *last, *eptr=new CInputEventList;
	unsigned char kbstate[256];
	POINT pt;

	eptr->event.type=type;
	eptr->event.chr=0;
	pt.x=x; pt.y=y;

	GetKeyboardState(kbstate);
	if(type==INPUT_KEYDOWN)
	{
		if((flags & HGEINP_REPEAT) == 0) keyz[key] |= 1;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_KEYUP)
	{
		keyz[key] |= 2;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_MOUSEWHEELV || type==INPUT_MOUSEWHEELH)
	{
		eptr->event.key=0; eptr->event.wheel=key;
		ScreenToClient(hwnd,&pt);
	}
	else { eptr->event.key=key; eptr->event.wheel=0; }

	if(type==INPUT_MBUTTONDOWN)
	{
		keyz[key] |= 1;
		SetCapture(hwnd);
		bCaptured=true;
	}
	if(type==INPUT_MBUTTONUP)
	{
		keyz[key] |= 2;
		ReleaseCapture();
		if (!hwndParent)
		{
			Input_SetMousePos(Xpos, Ypos);
		}
		pt.x=(int)Xpos; pt.y=(int)Ypos;
		bCaptured=false;
	}

	if(kbstate[VK_SHIFT] & 0x80) flags|=HGEINP_SHIFT;
	if(kbstate[VK_CONTROL] & 0x80) flags|=HGEINP_CTRL;
	if(kbstate[VK_MENU] & 0x80) flags|=HGEINP_ALT;
	if(kbstate[VK_CAPITAL] & 0x1) flags|=HGEINP_CAPSLOCK;
	if(kbstate[VK_SCROLL] & 0x1) flags|=HGEINP_SCROLLLOCK;
	if(kbstate[VK_NUMLOCK] & 0x1) flags|=HGEINP_NUMLOCK;
	eptr->event.flags=flags;

	if(pt.x==-1) { eptr->event.x=Xpos;eptr->event.y=Ypos; }
	else
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		float windoww = rect.right - rect.left;
		float windowh = rect.bottom - rect.top;

		if(pt.x<0) pt.x=0;
		if(pt.y<0) pt.y=0;
		if(pt.x>=windoww) pt.x=windoww-1;
		if(pt.y>=windowh) pt.y=windowh-1;

		eptr->event.x=(float)pt.x;
		eptr->event.y=(float)pt.y;
	}

	eptr->next=0;

	if(!queue) queue=eptr;
	else
	{
		last=queue;
		while(last->next) last=last->next;
		last->next=eptr;
	}

	if(eptr->event.type==INPUT_KEYDOWN || eptr->event.type==INPUT_MBUTTONDOWN)
	{
		VKey=eptr->event.key;Char=eptr->event.chr;
	}
	else if(eptr->event.type==INPUT_MOUSEMOVE)
	{
		Xpos=eptr->event.x;Ypos=eptr->event.y;
	}
	else if(eptr->event.type==INPUT_MOUSEWHEELV)
	{
		ZVpos+=eptr->event.wheel;
	}
	else if (eptr->event.type==INPUT_MOUSEWHEELH)
	{
		ZHpos+=eptr->event.wheel;
	}
#endif
}

void HGE_Impl::_ClearQueue()
{
	CInputEventList *nexteptr, *eptr=queue;

	memset(&keyz, 0, sizeof(keyz));

	while(eptr)
	{
		nexteptr=eptr->next;
		delete eptr;
		eptr=nexteptr;
	}

	queue=0; VKey=0; Char=0; ZVpos=0; ZHpos=0;
}

/************************************************************************/
/* These functions are added by h5nc (h5nc@yahoo.com.cn)                */
/************************************************************************/
// begin
#if IF_INPUTSYS(HINPUTSYS_DI)
BOOL CALLBACK HGE_Impl::_EnumJoysticksCallback (const DIDEVICEINSTANCE * pdidInstance, VOID* pContext)
{
	int count = *(int*)pContext;
	if (count < DIJOY_MAXDEVICE)
	{
		joyGuid[count] = pdidInstance->guidInstance;
		count++;
		*(int *)pContext = count;
	}
	if (count >= DIJOY_MAXDEVICE)
	{
		*(int *)pContext = 0;
		return DIENUM_STOP;
	}

	return (DIENUM_CONTINUE);
}
#endif

LPDIRECTINPUT8 CALL HGE_Impl::Input_GetDevice()
{
	return lpDInput;
}

bool HGE_Impl::_DIKInit()
{
	ZeroMemory(keyState, sizeof(BYTE)*256);
	ZeroMemory(lastKeyState, sizeof(BYTE)*256);

	lpDIKDevice = NULL;

#if IF_INPUTSYS(HINPUTSYS_DI)
	if (FAILED (lpDInput->CreateDevice(GUID_SysKeyboard, &lpDIKDevice, NULL)))
	{
		return false;
	}
	if (FAILED (lpDIKDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}
	if (FAILED (lpDIKDevice->SetCooperativeLevel (hwndParent?hwndParent:hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return false;
	}
	if(FAILED (lpDIKDevice->Acquire ()))
	{
		return false;
	}
#endif
	return true;
}

bool HGE_Impl::_DIJInit()
{
	bool joyable = false;
	int enumcount = 0;
	for (int i=0; i<DIJOY_MAXDEVICE; i++)
	{
		ZeroMemory(&(joyState[i]), sizeof(DIJOYSTATE));
		ZeroMemory(&(lastJoyState[i]), sizeof(DIJOYSTATE));
		haveJoy[i] = true;
		lpDIJDevice[i] = NULL;
	}
#if IF_INPUTSYS(HINPUTSYS_DI)
	if (FAILED (lpDInput->EnumDevices (DI8DEVCLASS_GAMECTRL, _EnumJoysticksCallback, &enumcount, DIEDFL_ATTACHEDONLY)))
	{
		for (int i=0; i<DIJOY_MAXDEVICE; i++)
		{
			haveJoy[i] = false;
		}
	}
	for (int i=0; i<DIJOY_MAXDEVICE; i++)
	{
		if (haveJoy[i] && FAILED (lpDInput->CreateDevice(joyGuid[i], &lpDIJDevice[i], NULL)))
		{
			haveJoy[i] = false;
		}
		if (haveJoy[i] && FAILED (lpDIJDevice[i]->SetDataFormat(&c_dfDIJoystick)))
		{
			haveJoy[i] = false;
		}
		if (haveJoy[i] && FAILED (lpDIJDevice[i]->SetCooperativeLevel (hwndParent?NULL:hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		{
			haveJoy[i] = false;
		}

		if (haveJoy[i])
		{
			DIPROPRANGE joyRange;

			joyRange.lMin = -24;
			joyRange.lMax = 24;

			joyRange.diph.dwSize       = sizeof(DIPROPRANGE);
			joyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			joyRange.diph.dwObj        = DIJOFS_X;
			joyRange.diph.dwHow        = DIPH_BYOFFSET;

			lpDIJDevice[i]->SetProperty(DIPROP_RANGE, &joyRange.diph);

			joyRange.lMin = -24;
			joyRange.lMax = 24;

			joyRange.diph.dwSize       = sizeof(DIPROPRANGE);
			joyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			joyRange.diph.dwObj        = DIJOFS_Y;
			joyRange.diph.dwHow        = DIPH_BYOFFSET;

			lpDIJDevice[i]->SetProperty(DIPROP_RANGE, &joyRange.diph);

			DIPROPDWORD deadZone;

			deadZone.diph.dwSize       = sizeof(deadZone);
			deadZone.diph.dwHeaderSize = sizeof(deadZone.diph);
			deadZone.diph.dwObj        = DIJOFS_X;
			deadZone.diph.dwHow        = DIPH_BYOFFSET;
			deadZone.dwData            = 1000;

			lpDIJDevice[i]->SetProperty(DIPROP_DEADZONE, &deadZone.diph);

			deadZone.diph.dwSize       = sizeof(deadZone);
			deadZone.diph.dwHeaderSize = sizeof(deadZone.diph);
			deadZone.diph.dwObj        = DIJOFS_Y;
			deadZone.diph.dwHow        = DIPH_BYOFFSET;
			deadZone.dwData            = 1000;

			lpDIJDevice[i]->SetProperty(DIPROP_DEADZONE,&deadZone.diph);

			if (FAILED (lpDIJDevice[i]->Acquire()))
			{
				haveJoy[i] = false;
			}
		}
		if (haveJoy[i])
		{
			joyable = true;
		}
	}

	if (joyable)
	{
		for (int i=0; i<DIJOY_MAXDEVICE; i++)
		{
			for (int j=i; j<DIJOY_MAXDEVICE; j++)
			{
				if (haveJoy[j] && !haveJoy[i])
				{
					LPDIRECTINPUTDEVICE8 _lpDIJDevice;
					_lpDIJDevice = lpDIJDevice[i];
					lpDIJDevice[i] = lpDIJDevice[j];
					lpDIJDevice[j] = _lpDIJDevice;

					GUID _joyGuid;
					_joyGuid = joyGuid[i];
					joyGuid[i] = joyGuid[j];
					joyGuid[j] = _joyGuid;

					haveJoy[i] = !haveJoy[i];
					haveJoy[j] = !haveJoy[j];

					break;
				}
			}
		}
	}

#endif
	return joyable;
}

bool HGE_Impl::_DIMInit()
{
	ZeroMemory(&mouseState, sizeof(DIMOUSESTATE2));
	ZeroMemory(&lastMouseState, sizeof(DIMOUSESTATE2));

	/************************************************************************/
	/* TODO!!                                                               */
	/************************************************************************/
	nMouseAxes = 3;
	nMouseButtons = 3;

#if IF_INPUTSYS(HINPUTSYS_DI)
	if( FAILED(lpDInput->CreateDevice(GUID_SysMouse, &lpDIMDevice, NULL)))
	{
		return false;
	}
	if (FAILED(lpDIMDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}
	if (FAILED(lpDIMDevice->SetCooperativeLevel (hwndParent?NULL:hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return false;
	}
	/*
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_ABS;
	lpDIMDevice->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
	*/
	if (FAILED(lpDIMDevice->Acquire()))
	{
		return false;
	}

	DIDEVCAPS MouseCapabilities; //device capabilities
	MouseCapabilities.dwSize = sizeof(MouseCapabilities);

	lpDIMDevice->GetCapabilities(&MouseCapabilities);

	if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
	{
		return false;
	}

	nMouseAxes = MouseCapabilities.dwAxes;
	nMouseButtons = MouseCapabilities.dwButtons;

	/*
	POINT pt;
	GetCursorPos(&pt);
	if (FAILED(lpDIMDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &offsetMouseState)))
	{
		return false;
	}
	offsetMouseState.lX -= pt.x;
	offsetMouseState.lY -= pt.y;

	offsetMouseState.lX = -offsetMouseState.lX;
	offsetMouseState.lY = -offsetMouseState.lY;
	offsetMouseState.lZ = -offsetMouseState.lZ;

	mouseState.lX = lastMouseState.lX = pt.x;
	mouseState.lY = lastMouseState.lY = pt.y;
	*/

#endif
	return true;
}

int HGE_Impl::_DIInit()
{
#if IF_PLATFORM(HPLATFORM_WIN)
	SetFocus(hwnd);
#endif
	if (!bUseDInput)
	{
		return 0;
	}
#if IF_INPUTSYS(HINPUTSYS_DI)
	if (FAILED (DirectInput8Create (hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &lpDInput, NULL)))
	{
		return ERROR_NOKEYBOARD|ERROR_NOJOYSTICK|ERROR_NOMOUSE;
	}
#endif

	bool keyable = _DIKInit();
	bool joyable = _DIJInit();
	bool mouseable = _DIMInit();

	int retval = 0;
	if (!keyable)
	{
		retval |= ERROR_NOKEYBOARD;
	}
	if (!joyable)
	{
		retval |= ERROR_NOJOYSTICK;
	}
	if (!mouseable)
	{
		retval |= ERROR_NOMOUSE;
	}


#if IF_PLATFORM(HPLATFORM_PSP)
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
#endif
	return retval;
}

bool HGE_Impl::Input_HaveJoy(int joydevice)
{
	return haveJoy[joydevice];
}

void HGE_Impl::_DIRelease()
{
	if (!bUseDInput)
	{
		return;
	}
#if IF_INPUTSYS(HINPUTSYS_DI)
	if(lpDIKDevice != NULL)
	{
		lpDIKDevice->Unacquire();
		lpDIKDevice->Release();
		lpDIKDevice = NULL;
	}
	for (int i=0; i<DIJOY_MAXDEVICE; i++)
	{
		if(lpDIJDevice[i] != NULL)
		{
			lpDIJDevice[i]->Unacquire();
			lpDIJDevice[i]->Release();
			lpDIJDevice[i] = NULL;
		}
	}
	if (lpDIMDevice != NULL)
	{
		lpDIMDevice->Unacquire();
		lpDIMDevice->Release();
		lpDIMDevice = NULL;
	}
#endif
}

bool HGE_Impl::_DIKUpdate()
{
	memcpy(lastKeyState, keyState, sizeof(keyState));
	ZeroMemory(&keyState, sizeof(keyState));
#if IF_INPUTSYS(HINPUTSYS_DI)
	if(lpDIKDevice == NULL)
	{
		_DIRelease();
		if (_DIInit() & ERROR_NOKEYBOARD)
		{
			return false;
		}
	}
	if (FAILED(lpDIKDevice->GetDeviceState(256, keyState)))
	{
		if (FAILED(lpDIKDevice->Acquire()))
		{
			_DIRelease();
			if (_DIInit() & ERROR_NOKEYBOARD)
			{
				return false;
			}
		}
		if (FAILED(lpDIKDevice->GetDeviceState(256, keyState)))
		{
			return false;
		}
	}
	if (!hwndParent && hwnd != GetActiveWindow())
	{
		ZeroMemory(&keyState, sizeof(keyState));
	}
#else
	/************************************************************************/
	/* TODO                                                                 */
	/************************************************************************/

 #if IF_PLATFORM(HPLATFORM_PSP)
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
	//Analog pad.Lx, Pad.Ly
	for (int i=0; i<32; i++)
	{
		if (pad.Buttons & (1<<i))
		{
			keyState[i] = 1<<7;
		}
	}
 #endif // PSP

#endif // WIN32

	return true;
}

bool HGE_Impl::_DIJUpdate()
{
	bool joyable=false;
#if IF_INPUTSYS(HINPUTSYS_DI)
	for (int i=0; i<DIJOY_MAXDEVICE; i++)
	{
		lastJoyState[i] = joyState[i];
		if (haveJoy[i])
		{
			if (lpDIJDevice[i] == NULL)
			{
				_DIRelease();
				if (_DIInit() & ERROR_NOJOYSTICK)
				{
					haveJoy[i] = false;
				}
				if(lpDIJDevice[i] == NULL)
				{
					haveJoy[i] = false;
				}
			}
			if (haveJoy[i] && FAILED(lpDIJDevice[i]->Poll()))
			{
				if (FAILED(lpDIJDevice[i]->Acquire()))
				{
					_DIRelease();
					if (_DIInit() & ERROR_NOJOYSTICK)
					{
						haveJoy[i] = false;
					}
				}
				if (FAILED(lpDIJDevice[i]->Poll()))
				{
					haveJoy[i] = false;
				}
			}
			if (haveJoy[i] && FAILED(lpDIJDevice[i]->GetDeviceState(sizeof(DIJOYSTATE), &joyState[i])))
			{
				haveJoy[i] = false;
			}
		}
		else
		{
			haveJoy[i] = false;
		}

		if (haveJoy[i])
		{
			joyable = true;
		}
	}
#endif
	return joyable;
}

bool HGE_Impl::_DIMUpdate()
{
	lastMouseState = mouseState;
	ZeroMemory(&mouseState, sizeof(DIMOUSESTATE2));
#if IF_INPUTSYS(HINPUTSYS_DI)
	if(lpDIMDevice == NULL)
	{
		_DIRelease();
		if (_DIInit() & ERROR_NOMOUSE)
		{
			return false;
		}
	}
	/*
	if (FAILED(lpDIMDevice->Poll()))
	{
		if (FAILED(lpDIMDevice->Acquire()))
		{
		_DIRelease();
		if (_DIInit() & ERROR_NOMOUSE)
		{
			return false;
		}
		}
		if (FAILED(lpDIMDevice->Poll()))
		{
			return false;
		}
	}
	*/
	if (FAILED(lpDIMDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState)))
	{
		if (FAILED(lpDIMDevice->Acquire()))
		{
			_DIRelease();
			if (_DIInit() & ERROR_NOMOUSE)
			{
				return false;
			}
		}
		if (FAILED(lpDIMDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState)))
		{
			return false;
		}
	}
	/*
	mouseState.lX += lastMouseState.lX;
	mouseState.lY += lastMouseState.lY;
	mouseState.lZ += lastMouseState.lZ;
	*/
#endif

	return true;
}

int HGE_Impl::_DIUpdate()
{
	if (!bUseDInput)
	{
		return 0;
	}
	bool keyable = _DIKUpdate();
	bool joyable = _DIJUpdate();
	bool mouseable = _DIMUpdate();

	int retval = 0;
	if (!keyable)
	{
		retval |= ERROR_NOKEYBOARD;
	}
	if (!joyable)
	{
		retval |= ERROR_NOJOYSTICK;
	}
	if (!mouseable)
	{
		retval |= ERROR_NOMOUSE;
	}

	return retval;
}

void HGE_Impl::_TouchRelease()
{
}

void HGE_Impl::_TouchInit()
{
	memset(touchesState, 0, sizeof(touchesState));
}

void HGE_Impl::_TouchUpdate()
{
	for (int i=0; i<TOUCH_TOUCHMAX; i++)
	{
		if (touchesState[i].touching)
		{
			touchesState[i].touchtimer++;
			touchesState[i].notouchtimer = 0;
		}
		else
		{
			touchesState[i].notouchtimer++;
			touchesState[i].touchtimer = 0;
		}
		touchesState[i].lasttouching = touchesState[i].touching;

		touchesState[i].touching = false;
	}
}

bool HGE_Impl::Input_IsTouchDown(int touch)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return false;
	}
	if (touchesState[touch].lasttouching == false && touchesState[touch].touching == true)
	{
		return true;
	}
	return false;
}

bool HGE_Impl::Input_IsTouchOn(int touch)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return false;
	}
	if (touchesState[touch].touching == true)
	{
		return true;
	}
	return false;
}

bool HGE_Impl::Input_IsTouchUp(int touch)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return false;
	}
	if (touchesState[touch].lasttouching == true && touchesState[touch].touching == false)
	{
		return true;
	}
	return false;
}

void HGE_Impl::Input_GetTouchPos(int touch, float *x, float *y)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX || !x || !y)
	{
		return;
	}
	*x = touchesState[touch].x;
	*y = touchesState[touch].y;
}

void HGE_Impl::Input_GetLastTouchPos(int touch, float *x, float *y)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX || !x || !y)
	{
		return;
	}
	*x = touchesState[touch].lastx;
	*y = touchesState[touch].lasty;
}

void HGE_Impl::Input_GetFirstTouchPos(int touch, float *x, float *y)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX || !x || !y)
	{
		return;
	}
	*x = touchesState[touch].firstx;
	*y = touchesState[touch].firsty;
}

int HGE_Impl::Input_GetTouchDuration(int touch)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return 0;
	}
	return touchesState[touch].touchtimer;
}

int HGE_Impl::Input_GetNoTouchDuration(int touch)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return 0;
	}
	return touchesState[touch].notouchtimer;
}

void HGE_Impl::Input_SetTouchPos(int touch, float x, float y)
{
	if (touch < 0 || touch >= TOUCH_TOUCHMAX)
	{
		return;
	}

	touchesState[touch].touching = true;
	if (!touchesState[touch].lasttouching)
	{
		touchesState[touch].firstx = x;
		touchesState[touch].firsty = y;
		touchesState[touch].lastx = x;
		touchesState[touch].lasty = y;
	}
	else
	{
		touchesState[touch].lastx = touchesState[touch].x;
		touchesState[touch].lasty = touchesState[touch].y;
	}
	touchesState[touch].x = x;
	touchesState[touch].y = y;
}

// end