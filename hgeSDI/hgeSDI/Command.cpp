#include "stdafx.h"
#include "Command.h"

#include "LineCommand.h"
#include "GUICoordinate.h"

Command * pCommandSingleton = NULL;

Command::Command()
{
	assert(pCommandSingleton==NULL);
	ZeroMemory(&ccomm, sizeof(ccomm));
}

Command::~Command()
{

}

Command * Command::getInstance()
{
	if (!pCommandSingleton)
	{
		pCommandSingleton = new Command();
	}
	return pCommandSingleton;
}

int Command::CreateCommand(int comm)
{
	if (comm > _COMM_PUSHCOMMANDBEGIN && comm < _COMM_PUSHCOMMANDEND)
	{
		PushCommand();
	}

	if (comm > _COMM_INSTANTCOMMANDBEGIN && comm < _COMM_INSTANTCOMMANDEND)
	{
	}
	else
	{
		ClearCurrentCommand(true);
		ccomm.command = comm;
	}

	return ccomm.command;
}

int Command::ProcessCommand()
{
	if (!ccomm.command)
	{
		return 0;
	}

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

int Command::FinishCommand()
{
	if (ccomm.command > _COMM_PUSHCOMMANDBEGIN && ccomm.command < _COMM_PUSHCOMMANDEND)
	{
		return PullCommand();
	}
	else
	{
		return ClearCurrentCommand(true);
	}
}

int Command::PushCommand()
{
	if (ccomm.command)
	{
		StepTo(CSI_PAUSE);
		ProcessCommand();
		StepBack();
	}
	pushedcomm.push_back(ccomm);
	return ClearCurrentCommand();
}

int Command::PullCommand()
{
	if (!pushedcomm.size())
	{
		return 0;
	}
	SetCurrentCommand(&(pushedcomm.back()));
	pushedcomm.pop_back();
	if (ccomm.command)
	{
		StepTo(CSI_RESUME);
		ProcessCommand();
		StepBack();
	}
	return ccomm.command;
}

int Command::ClearCurrentCommand(bool callterminal /*=false*/)
{
	if (ccomm.command && callterminal)
	{
		StepTo(CSI_TERMINAL);
		ProcessCommand();
		StepBack();
	}
	ZeroMemory(&ccomm, sizeof(ccomm));
	return 0;
}

int Command::SetCurrentCommand( CommandStepInfo * info )
{
	if (!info)
	{
		return -1;
	}
	memcpy(&ccomm, info, sizeof(ccomm));
	return 0;
}

int Command::SetParamXY( int index, float x, float y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].x = x;
	ccomm.param[index].y = y;
	ccomm.param[index].useflag |= COMMPARAMFLAG_XY;
	return ccomm.command;
}

int Command::SetParamF( int index, float fval )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].fval = fval;
	ccomm.param[index].useflag |= COMMPARAMFLAG_F;
	return ccomm.command;
}

int Command::SetParamI( int index, int ival )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].ival = ival;
	ccomm.param[index].useflag |= COMMPARAMFLAG_I;
	return ccomm.command;
}

int Command::SetParamS( int index, const char * sval )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	strcpy_s(ccomm.param[index].sval, M_STRMAX, sval);
	ccomm.param[index].useflag |= COMMPARAMFLAG_S;
	return ccomm.command;
}

bool Command::GetParamXY( int index, float * x, float * y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return false;
	}
	if (x)
	{
		*x = ccomm.param[index].x;
	}
	if (y)
	{
		*y = ccomm.param[index].y;
	}
	return true;
}

float Command::GetParamF( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return 0;
	}
	return ccomm.param[index].fval;
}

float Command::GetParamI( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return 0;
	}
	return ccomm.param[index].ival;
}

char * Command::GetParamS( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return NULL;
	}
	return ccomm.param[index].sval;
}