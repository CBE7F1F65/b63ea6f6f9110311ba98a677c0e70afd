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
	virtual void OnClearTemp();

	bool MIDCBLength(MarkingUI * pmui, bool bAccept);
	bool MIDCBAngle(MarkingUI * pmui, bool bAccept);

	static bool staticMIDCBLength(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBAngle(MarkingUI * pmui, bool bAccept);

private:


	GObject * pBeginPicked;
	GObject * pEndPicked;
	GObject * pNextBeginPicked;
	float fProportionBegin;
	float fProportionEnd;
	int iSecBegin;

	GLine * pNCLine;

	GBezierLine * pTempLine;

};