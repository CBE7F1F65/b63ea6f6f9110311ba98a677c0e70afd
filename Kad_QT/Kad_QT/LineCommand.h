#pragma once

#include "CommandTemplate.h"
#include "MarkingObject.h"

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
	virtual void OnInitCommand();
	virtual void OnTerminalCommand();


private:

	void ClearTemp();

	GObject * pMergeToBegin;
	GObject * pMergeToEnd;
	GObject * pNextMergeToBegin;
	float fProportionBegin;
	float fProportionEnd;

	GLine * pNCLine;

	GBezierLine * pTempLine;
};