#pragma once

class GObjectPicker
{
public:
	static GObjectPicker& getInstance() { static GObjectPicker instance; return instance; }

private:
	GObjectPicker();
	~GObjectPicker();
	GObjectPicker(GObjectPicker const&);
	void operator=(GObjectPicker const&);

public:
};