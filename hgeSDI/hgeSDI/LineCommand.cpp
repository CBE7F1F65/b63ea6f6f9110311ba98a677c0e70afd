#include "stdafx.h"
#include "Command.h"
#include "LineCommand.h"

LineCommand * plc = NULL;

LineCommand::LineCommand()
{
	assert(plc==NULL);
}
LineCommand::~LineCommand()
{

}

LineCommand * LineCommand::getInstance()
{
	if (!plc)
	{
		plc = new LineCommand();
	}
	return plc;
}

int LineCommand::Line()
{
	return 0;
}