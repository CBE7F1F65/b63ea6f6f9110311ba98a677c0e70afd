#pragma once

#include "CommandTemplate.h"

class BezierCommand : public CommandTemplate
{
public:
	static BezierCommand& getInstance() { static BezierCommand instance; return instance; }

private:
	BezierCommand();
	~BezierCommand();
	BezierCommand(BezierCommand const&);
	void operator=(BezierCommand const&);

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

	GAnchorPoint * pBindAnchorBegin;
	GAnchorPoint * pBindAnchorEnd;

	float fProportionBegin;
	float fProportionEnd;

	GLine * pNCLine;

	GBezierLine * pTempLine;
	bool bDrawTempBezierLine;
	bool bDrawTempLineHandle;
};