#pragma once
#include "CommandTemplate.h"

class DemergeCommand : public CommandTemplate
{
public:
	static DemergeCommand& getInstance() { static DemergeCommand instance; return instance; }

private:
	DemergeCommand();
	~DemergeCommand();
	DemergeCommand(DemergeCommand const&);
	void operator=(DemergeCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
//	virtual void OnProcessUnDoCommand( RevertibleCommand * rc );
	
};

