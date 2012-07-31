#pragma once
#include "CommandTemplate.h"

class LineTransformCommand : public CommandTemplate
{
public:
	static LineTransformCommand& getInstance() { static LineTransformCommand instance; return instance; }

private:
	LineTransformCommand();
	~LineTransformCommand();
	LineTransformCommand(LineTransformCommand const&);
	void operator=(LineTransformCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
//	virtual void OnProcessUnDoCommand( RevertableCommand * rc );
	
};

