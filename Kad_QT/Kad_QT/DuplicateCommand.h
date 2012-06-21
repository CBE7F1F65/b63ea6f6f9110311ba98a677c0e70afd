#pragma once

#include "CommandTemplate.h"

class DuplicateCommand :
	public CommandTemplate
{
public:
	static DuplicateCommand& getInstance() { static DuplicateCommand instance; return instance; }

private:
	DuplicateCommand();
	~DuplicateCommand();
	DuplicateCommand(DuplicateCommand const&);
	void operator=(DuplicateCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	
};

