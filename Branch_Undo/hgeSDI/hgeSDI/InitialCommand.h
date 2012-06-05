#pragma once
#include "commandtemplate.h"

class InitialCommand:
	public CommandTemplate
{
public:
	static InitialCommand& getInstance()
	{
		static InitialCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	InitialCommand();
	~InitialCommand();
	// Don't forget to declare these two.
	InitialCommand(InitialCommand const&);
	// Don't Implement
	void operator=(InitialCommand const&);
	// Don't implement

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
};