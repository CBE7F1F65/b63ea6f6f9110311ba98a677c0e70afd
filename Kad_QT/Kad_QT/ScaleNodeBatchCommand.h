#pragma once
#include "commandtemplate.h"

class ScaleNodeBatchCommand : public CommandTemplate
{
public:
	static ScaleNodeBatchCommand& getInstance() { static ScaleNodeBatchCommand instance; return instance; }

private:
	ScaleNodeBatchCommand();
	~ScaleNodeBatchCommand();
	ScaleNodeBatchCommand(ScaleNodeBatchCommand const&);
	void operator=(ScaleNodeBatchCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

	virtual void OnInitCommand();
	virtual void OnClearCommand();
	virtual void OnTerminalCommand();

	bool MIDCBLength(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBLength(MarkingUI * pmui, bool bAccept);

	bool FilterCallback(GObject * pObj);
	static bool staticFilterCallback(GObject * pObj);

	void ClearTemp();

	list<GObject *> lstObj;
	bool bManualMode;

	GBezierLine * pTempLine;
	GBezierLine * pTempLineScale;

	float fScaleBaseXDiff;
	float fScaleBaseYDiff;
	bool bBeginSet;
	float fLastXDiff;
	float fLastYDiff;
	
};

