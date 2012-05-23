#include "stdafx.h"
#include "Command.h"

#include "GUICoordinate.h"
#include "LineCommand.h"

int Command::ProcessCommand()
{
	if (!ccomm.command)
	{
		return 0;
	}

	ProcessCommittedCommand();

	switch (ccomm.command)
	{
	case COMM_PAN:
		return GUICoordinate::getInstance()->DoPanCommand();
		break;
	case COMM_DOZOOM:
		return GUICoordinate::getInstance()->DoZoomCommand();
	case COMM_LINE:
		return LineCommand::getInstance()->Line();
	}
	return ccomm.command;
}
