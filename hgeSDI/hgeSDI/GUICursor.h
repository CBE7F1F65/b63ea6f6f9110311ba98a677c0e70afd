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
	GUICursor();
	~GUICursor();

	static GUICursor * getInstance();

	void ChangeCursor(int guic=-1);
	void Render(float x, float y);

	int guic;
	int lastguic;
};