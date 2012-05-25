#pragma once

#include "CommandTemplate.h"

class BezierCommand : public CommandTemplate
{
public:
	static BezierCommand& getInstance()
	{
		static BezierCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	BezierCommand();
	~BezierCommand();
	// Don't forget to declare these two.
	BezierCommand(BezierCommand const&);
	// Don't Implement
	void operator=(BezierCommand const&);
	// Don't implement

public:

	virtual void OnProcessCommand();
	virtual void DoneCommand();
	virtual void RenderToTarget();

}; 

