#pragma once

class GObjectPicker
{
public:
	static GObjectPicker& getInstance()
	{
		static GObjectPicker instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	GObjectPicker();
	~GObjectPicker();
	// Don't forget to declare these two.
	GObjectPicker(GObjectPicker const&);
	// Don't Implement
	void operator=(GObjectPicker const&);
	// Don't implement

public:
	

};