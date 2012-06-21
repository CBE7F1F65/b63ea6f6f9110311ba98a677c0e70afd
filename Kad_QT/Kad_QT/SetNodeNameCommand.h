#pragma once

#include "CommandTemplate.h"

class SetNodeNameCommand :
	public CommandTemplate
{
public:
	static SetNodeNameCommand& getInstance() { static SetNodeNameCommand instance; return instance; }

private:
	SetNodeNameCommand();
	~SetNodeNameCommand();
	SetNodeNameCommand(SetNodeNameCommand const&);
	void operator=(SetNodeNameCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand(RevertableCommand * rc);
	
};

