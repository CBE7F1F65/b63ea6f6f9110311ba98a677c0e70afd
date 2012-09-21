#pragma once
#include "commandtemplate.h"

class SaveCommand : public CommandTemplate
{
public:
	static SaveCommand& getInstance() { static SaveCommand instance; return instance; }

private:
	SaveCommand();
	~SaveCommand();
	SaveCommand(SaveCommand const&);
	void operator=(SaveCommand const&);

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
public:
	
};

