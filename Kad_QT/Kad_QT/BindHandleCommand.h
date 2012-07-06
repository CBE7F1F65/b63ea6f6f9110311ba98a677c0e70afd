#pragma once
#include "CommandTemplate.h"

class BindHandleCommand : public CommandTemplate
{
public:
	static BindHandleCommand& getInstance() { static BindHandleCommand instance; return instance; }

private:
	BindHandleCommand();
	~BindHandleCommand();
	BindHandleCommand(BindHandleCommand const&);
	void operator=(BindHandleCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );
	
};

