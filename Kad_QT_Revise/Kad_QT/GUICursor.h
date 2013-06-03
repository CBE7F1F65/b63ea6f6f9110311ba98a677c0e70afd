#pragma once

enum
{
	GUIC_NORMAL,
	GUIC_CREATEPOINT,
	GUIC_HAND,
};

class GUICursor
{
public:
	static GUICursor& getInstance() { static GUICursor instance; return instance; }

private:
	GUICursor();
	~GUICursor();
	GUICursor(GUICursor const&);
	void operator=(GUICursor const&);

public:

	void ChangeCursor(int guic=-1);
	void Render(float x, float y);

	int guic;
	int lastguic;
};