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
//	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

	virtual void OnClearTemp();

	virtual void RenderToTarget();

private:
	GBezierLine * pTempLineLeft;
	GBezierLine * pTempLineRight;	
};

