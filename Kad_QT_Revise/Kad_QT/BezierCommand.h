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
	virtual void OnClearTemp();

	bool MIDCBLength(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBLength(MarkingUI * pmui, bool bAccept);

private:


	GObject * pBeginPicked;
	GObject * pEndPicked;
	GObject * pNextBeginPicked;

	int iSecPicked;

	GAnchorPoint * pBindAnchorBegin;
	GAnchorPoint * pBindAnchorEnd;

	float fProportionBegin;
	float fProportionEnd;

	GLine * pNCLine;

	GBezierLine * pTempLine;
	bool bDrawTempBezierLine;
	bool bDrawTempLineHandle;

	MarkingLine * pMarking;
};