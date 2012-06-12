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
	static MouseCursorManager& getInstance()
	{
		static MouseCursorManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	MouseCursorManager();
	~MouseCursorManager();
	// Don't forget to declare these two.
	MouseCursorManager(MouseCursorManager const&);
	// Don't Implement
	void operator=(MouseCursorManager const&);
	// Don't implement

	HCURSOR hCursors[_MCHCURSOR_ENDINDEX];

public:
	void ChangeCursor(HWND hwnd, int cursor=-1);
	void Init();
};

