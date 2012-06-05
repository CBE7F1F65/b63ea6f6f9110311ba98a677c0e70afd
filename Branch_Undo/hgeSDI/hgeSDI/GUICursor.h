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
	static GUICursor& getInstance()
	{
		static GUICursor instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	GUICursor();
	~GUICursor();
	// Don't forget to declare these two.
	GUICursor(GUICursor const&);
	// Don't Implement
	void operator=(GUICursor const&);
	// Don't implement

public:

	void ChangeCursor(int guic=-1);
	void Render(float x, float y);

	int guic;
	int lastguic;
};