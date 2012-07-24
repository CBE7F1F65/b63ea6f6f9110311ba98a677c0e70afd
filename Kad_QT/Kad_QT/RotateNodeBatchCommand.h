#pragma once
#include "commandtemplate.h"

class RotateNodeBatchCommand : public CommandTemplate
{
public:
	static RotateNodeBatchCommand& getInstance() { static RotateNodeBatchCommand instance; return instance; }

private:
	RotateNodeBatchCommand();
	~RotateNodeBatchCommand();
	RotateNodeBatchCommand(RotateNodeBatchCommand const&);
	void operator=(RotateNodeBatchCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand( RevertableCommand * rc );

	virtual void OnInitCommand();
	virtual void OnClearCommand();
	virtual void OnTerminalCommand();

	bool MIDCBAngle(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBAngle(MarkingUI * pmui, bool bAccept);
	bool MIDCBLength(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBLength(MarkingUI * pmui, bool bAccept);

	bool FilterCallback(GObject * pObj);
	static bool staticFilterCallback(GObject * pObj);
	
	void ClearTemp();

	list<GObject *> lstObj;
	bool bManualMode;

	GBezierLine * pTempLine;
	int lastAngle;

	float fBeginLength;
	float fLastLength;
};

