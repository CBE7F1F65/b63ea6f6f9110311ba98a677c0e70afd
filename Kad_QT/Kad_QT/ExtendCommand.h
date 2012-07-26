#pragma once
#include "commandtemplate.h"

class ExtendCommand : public CommandTemplate
{
public:
	static ExtendCommand& getInstance() { static ExtendCommand instance; return instance; }

private:
	ExtendCommand();
	~ExtendCommand();
	ExtendCommand(ExtendCommand const&);
	void operator=(ExtendCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

	void ClearTemp();

	virtual void OnInitCommand();
	virtual void OnClearCommand();
	virtual void OnTerminalCommand();

	virtual void RenderToTarget();
	GBezierLine * pTempLine;
	
};

