#pragma once
#include "CommandTemplate.h"

class MergeCommand : public CommandTemplate
{
public:
	static MergeCommand& getInstance() { static MergeCommand instance; return instance; }

private:
	MergeCommand();
	~MergeCommand();
	MergeCommand(MergeCommand const&);
	void operator=(MergeCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
//	virtual void OnProcessUnDoCommand( RevertibleCommand * rc );
	
};

