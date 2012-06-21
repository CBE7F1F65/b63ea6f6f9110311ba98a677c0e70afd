#pragma once
#include "commandtemplate.h"

class ReparentCommand :
	public CommandTemplate
{
public:
	static ReparentCommand& getInstance() { static ReparentCommand instance; return instance; }

private:
	ReparentCommand();
	~ReparentCommand();
	ReparentCommand(ReparentCommand const&);
	void operator=(ReparentCommand const&);

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

public:
	
};

