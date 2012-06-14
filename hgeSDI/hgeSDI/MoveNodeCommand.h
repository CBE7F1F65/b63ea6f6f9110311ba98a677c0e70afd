#pragma once

#include "CommandTemplate.h"

class MoveNodeCommand :
	public CommandTemplate
{
public:
	static MoveNodeCommand& getInstance() { static MoveNodeCommand instance; return instance; }

private:
	MoveNodeCommand();
	~MoveNodeCommand();
	MoveNodeCommand(MoveNodeCommand const&);
	void operator=(MoveNodeCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );
	
};

