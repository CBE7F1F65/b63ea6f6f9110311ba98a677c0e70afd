#include "stdafx.h"
#include "Command.h"

#include "LineCommand.h"

DWORD Command::currentcommand = 0;
int Command::step = 0;

Command::Command()
{

}

Command::~Command()
{

}

int Command::DispatchCommand(DWORD comm)
{
	switch (comm)
	{
	case COMM_LINE:
		return LineCommand::getInstance()->Line();
	}
	return 0;
}