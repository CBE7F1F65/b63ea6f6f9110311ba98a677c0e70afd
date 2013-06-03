#pragma once
#include "commandtemplate.h"

class CircleCommand : public CommandTemplate
{
public:
	static CircleCommand& getInstance() { static CircleCommand instance; return instance; }

private:
	CircleCommand();
	~CircleCommand();
	CircleCommand(CircleCommand const&);
	void operator=(CircleCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnClearTemp();
	virtual void RenderToTarget();

	bool MIDCBLength(MarkingUI * pmui, bool bAccept);
	static bool staticMIDCBLength(MarkingUI * pmui, bool bAccept);

protected:
	GBezierLine * pTempRLine;
	GBezierLine * pTempLines[QUADRANTMAX];
};

