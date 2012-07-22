#pragma once
#include "commandtemplate.h"

class ClipCommand : public CommandTemplate
{
public:
	static ClipCommand& getInstance() { static ClipCommand instance; return instance; }

private:
	ClipCommand();
	~ClipCommand();
	ClipCommand(ClipCommand const&);
	void operator=(ClipCommand const&);
	
public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

	static bool staticPickFilterFunc(GObject * pObj);
	bool PickFilterFunc(GObject * pObj);
};

