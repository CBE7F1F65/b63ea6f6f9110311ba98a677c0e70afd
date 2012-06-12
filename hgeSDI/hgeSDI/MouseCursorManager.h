#pragma once

enum{
	MCHCURSOR_POINTER=0,
	MCHCURSOR_HAND,
	MCHCURSOR_GRAB,

	_MCHCURSOR_ENDINDEX,
};

class MouseCursorManager
{
public:
	static MouseCursorManager& getInstance() { static MouseCursorManager instance; return instance; }

private:
	MouseCursorManager();
	~MouseCursorManager();
	MouseCursorManager(MouseCursorManager const&);
	void operator=(MouseCursorManager const&);

	HCURSOR hCursors[_MCHCURSOR_ENDINDEX];

public:
	void ChangeCursor(HWND hwnd, int cursor=-1);
	void Init();
};

