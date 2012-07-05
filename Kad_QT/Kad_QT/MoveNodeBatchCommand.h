#pragma once

#include "CommandTemplate.h"

class MoveNodeBatchCommand :
	public CommandTemplate
{
public:
	static MoveNodeBatchCommand& getInstance() { static MoveNodeBatchCommand instance; return instance; }

private:
	MoveNodeBatchCommand();
	~MoveNodeBatchCommand();
	MoveNodeBatchCommand(MoveNodeBatchCommand const&);
	void operator=(MoveNodeBatchCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

};

