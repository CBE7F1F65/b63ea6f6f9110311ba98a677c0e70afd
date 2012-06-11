#pragma once
#include "commandtemplate.h"

class ReparentCommand :
	public CommandTemplate
{
public:
	static ReparentCommand& getInstance()
	{
		static ReparentCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	ReparentCommand();
	~ReparentCommand();
	// Don't forget to declare these two.
	ReparentCommand(ReparentCommand const&);
	// Don't Implement
	void operator=(ReparentCommand const&);
	// Don't implement

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

public:
	
};

