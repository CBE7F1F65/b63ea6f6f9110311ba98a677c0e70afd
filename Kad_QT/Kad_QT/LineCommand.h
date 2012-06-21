#pragma once

#include "CommandTemplate.h"

class LineCommand : public CommandTemplate
{
public:
	static LineCommand& getInstance() { static LineCommand instance; return instance; }

private:
	LineCommand();
	~LineCommand();
	LineCommand(LineCommand const&);
	void operator=(LineCommand const&);

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void RenderToTarget();
};