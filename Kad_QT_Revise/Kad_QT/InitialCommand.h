#pragma once
#include "commandtemplate.h"

class InitialCommand:
	public CommandTemplate
{
public:
	static InitialCommand& getInstance() { static InitialCommand instance; return instance; }

private:
	InitialCommand();
	~InitialCommand();
	InitialCommand(InitialCommand const&);
	void operator=(InitialCommand const&);

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
};