#include "stdafx.h"
#include "Command.h"

#include "GUICoordinate.h"
#include "LineCommand.h"
#include "BezierCommand.h"

void Command::ProcessCommand()
{
	if (!ccomm.command)
	{
		return;
	}

	ProcessCommittedCommand();

	switch (ccomm.command)
	{
	case COMM_PAN:
		return GUICoordinate::getInstance().OnProcessPanCommand();
		break;
	case COMM_DOZOOM:
		return GUICoordinate::getInstance().OnProcessZoomCommand();
	case COMM_LINE:
		return LineCommand::getInstance().OnProcessCommand();
	case COMM_BEZIER:
		return BezierCommand::getInstance().OnProcessCommand();
	}
}
