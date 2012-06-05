#include "stdafx.h"
#include "CommandTemplate.h"

#include "LineCommand.h"
#include "BezierCommand.h"
#include "InitialCommand.h"
#include "NewLayerCommand.h"
#include "SetWorkingLayerCommand.h"


CommandTemplate * CommandTemplate::GetTemplateByCommand( int comm )
{
	switch (comm)
	{
	case COMM_INITIAL:
		return &InitialCommand::getInstance();

	case COMM_NEWLAYER:
		return &NewLayerCommand::getInstance();
	case COMM_NEWSUBLAYER:
		return &NewLayerCommand::getInstance();
	case COMM_SETWORKINGLAYER:
		return &SetWorkingLayerCommand::getInstance();

	case COMM_LINE:
		return &LineCommand::getInstance();
	case COMM_BEZIER:
		return &BezierCommand::getInstance();

	default:
		ASSERT(true);
	}
	return NULL;
}