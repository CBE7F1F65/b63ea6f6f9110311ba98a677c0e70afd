#pragma once
#include "commandtemplate.h"

class FlipNodeBatchCommand : public CommandTemplate
{
public:
	static FlipNodeBatchCommand& getInstance() { static FlipNodeBatchCommand instance; return instance; }

private:
	FlipNodeBatchCommand();
	~FlipNodeBatchCommand();
	FlipNodeBatchCommand(FlipNodeBatchCommand const&);
	void operator=(FlipNodeBatchCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );
	virtual void OnClearTemp();

	virtual void RenderToTarget();

	bool MIDCBAngle(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBAngle(MarkingUI * pmui, bool bAccept);

private:

	list<GObject *> lstObj;
	bool bManualMode;

	GBezierLine * pTempLine;
};

