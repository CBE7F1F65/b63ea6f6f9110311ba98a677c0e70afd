#include "stdafx.h"
#include "CommandTemplate.h"

#include "LineCommand.h"
#include "BezierCommand.h"
#include "InitialCommand.h"
#include "NewLayerCommand.h"
#include "SetWorkingLayerCommand.h"
#include "DeleteItemCommand.h"
#include "ReparentCommand.h"
#include "OutputCommand.h"


CommandTemplate * CommandTemplate::GetTemplateByCommand( int comm )
{
	switch (comm)
	{
	case COMM_INITIAL:
		return &InitialCommand::getInstance();

	case COMM_REPARENT:
		return &ReparentCommand::getInstance();
	case COMM_DELETEITEM:
		return &DeleteItemCommand::getInstance();

	case COMM_NEWLAYER:
	case COMM_NEWSUBLAYER:
		return &NewLayerCommand::getInstance();
	case COMM_SETWORKINGLAYER:
		return &SetWorkingLayerCommand::getInstance();

	case COMM_LINE:
		return &LineCommand::getInstance();
	case COMM_BEZIER:
		return &BezierCommand::getInstance();

	case COMM_OUTPUT:
		return &OutputCommand::getInstance();

	default:
		ASSERT(true);
	}
	return NULL;
}