#include "StdAfx.h"
#include "MouseCursorManager.h"
#include "hgeSDI.h"
#include "WinUser.h"

MouseCursorManager::MouseCursorManager(void)
{
	ZeroMemory(hCursors, sizeof(hCursors));
}


MouseCursorManager::~MouseCursorManager(void)
{
}


void MouseCursorManager::ChangeCursor( HWND hwnd, int cursor/*=-1*/ )
{
	if (cursor < 0)
	{
		cursor = MCHCURSOR_POINTER;
	}

	if (!hCursors[cursor])
	{
		Init();
	}
	SetCursor(hCursors[cursor]);
	SetClassLong(hwnd, GCL_HCURSOR, (DWORD)hCursors[cursor]);
}

void MouseCursorManager::Init()
{
	hCursors[MCHCURSOR_POINTER] = theApp.LoadStandardCursor(IDC_ARROW);
	hCursors[MCHCURSOR_HAND] = theApp.LoadStandardCursor(IDC_HAND);
	hCursors[MCHCURSOR_GRAB] = theApp.LoadStandardCursor(IDC_HAND);
}