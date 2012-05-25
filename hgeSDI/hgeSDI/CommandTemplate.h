#pragma once

#include "GUICoordinate.h"
#include "Main.h"
#include "RenderHelper.h"
#include "Command.h"

class CommandTemplate
{
public:
	CommandTemplate(void);
	~CommandTemplate(void);

	virtual void OnProcessCommand()=0;
	virtual void DoneCommand()=0;

	virtual void RenderToTarget();
	virtual void ReleaseTarget();

	GUICoordinate * pguic;
	MainInterface * pmain;
	Command * pcommand;
};

