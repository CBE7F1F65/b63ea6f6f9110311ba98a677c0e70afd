#pragma once
#include "CommandTemplate.h"

class CopyCommand : public CommandTemplate
{
public:
	static CopyCommand& getInstance() { static CopyCommand instance; return instance; }

private:
	CopyCommand();
	~CopyCommand();
	CopyCommand(CopyCommand const&);
	void operator=(CopyCommand const&);


public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	
};