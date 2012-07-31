#pragma once
#include "CommandTemplate.h"

class SeparateCommand : public CommandTemplate
{
public:
	static SeparateCommand& getInstance() { static SeparateCommand instance; return instance; }

private:
	SeparateCommand();
	~SeparateCommand();
	SeparateCommand(SeparateCommand const&);
	void operator=(SeparateCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
//	virtual void OnProcessUnDoCommand( RevertableCommand * rc );
	
};

