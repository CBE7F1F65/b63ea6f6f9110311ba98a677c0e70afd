#pragma once
#include "commandtemplate.h"

class NotchCommand :
	public CommandTemplate
{
public:
	static NotchCommand& getInstance() { static NotchCommand instance; return instance; }

private:
	NotchCommand();
	~NotchCommand();
	NotchCommand(NotchCommand const&);
	void operator=(NotchCommand const&);

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

	static bool staticPickFilterFunc(GObject * pObj);
	bool PickFilterFunc(GObject * pObj);
}; 

