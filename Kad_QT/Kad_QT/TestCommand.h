#pragma once
#include "CommandTemplate.h"

#ifdef DEBUGTEST

#include "CommandMap.h"

class TestCommand : public CommandTemplate
{
public:
	static TestCommand& getInstance() { static TestCommand instance; return instance; }

private:
	TestCommand();
	~TestCommand();
	TestCommand(TestCommand const&);
	void operator=(TestCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

	virtual void RenderToTarget();
	virtual void OnInitCommand();
	virtual void OnTerminalCommand();

	CommandMap cmap;
};

#endif // DEBUGTEST
