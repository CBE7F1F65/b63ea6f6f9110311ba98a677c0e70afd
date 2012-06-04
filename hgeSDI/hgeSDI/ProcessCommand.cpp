#include "stdafx.h"
#include "Command.h"

#include "GUICoordinate.h"
#include "LineCommand.h"
#include "BezierCommand.h"
#include "InitialCommand.h"
#include "NewLayerCommand.h"

int Command::ProcessPending( int index, int useflag, int fillprompt, int step, int wantprompt/*=0*/, bool pushback/*=true*/ )
{
	if (!pendingparam.type)
	{
		return -1;
	}

	if (pendingparam.type & COMMITTEDCOMMANDTYPE_SUBCOMMAND)
	{
		assert(pendingparam.csub != 0);
		return pendingparam.csub;
	}

	bool bok = false;
	switch (useflag)
	{
	case COMMPARAMFLAG_X:
		if (pendingparam.type & F_COMMITTEDCOMMANDTYPE_NUMBER)
		{
			SetParamX(index, pendingparam.fval, fillprompt);
			bok = true;
		}
		break;
	case COMMPARAMFLAG_Y:
		if (pendingparam.type & F_COMMITTEDCOMMANDTYPE_NUMBER)
		{
			SetParamY(index, pendingparam.fval, fillprompt);
			bok = true;
		}
		break;
	case COMMPARAMFLAG_F:
		if (pendingparam.type & F_COMMITTEDCOMMANDTYPE_NUMBER)
		{
			SetParamF(index, pendingparam.fval, fillprompt);
			bok = true;
		}
		break;
	case COMMPARAMFLAG_I:
		if (pendingparam.type & F_COMMITTEDCOMMANDTYPE_NUMBER)
		{
			SetParamI(index, pendingparam.ival, fillprompt);
			bok = true;
		}
		break;
	case COMMPARAMFLAG_S:
		SetParamS(index, pendingparam.sval.c_str(), fillprompt);
		bok = true;
		break;
	case COMMPARAMFLAG_G:
		if (pendingparam.type & F_COMMITTEDCOMMANDTYPE_NUMBER)
		{
			SetParamG(index, pendingparam.ival, fillprompt);
			bok = true;
		}
		break;
	}

	if (!bok)
	{
		LogError(pendingparam.sval.c_str());
		pendingparam.ClearSet();
		return -1;
	}

	StepTo(step, wantprompt, pushback);
	pendingparam.ClearSet();
	return 0;
}

int Command::ProcessCommittedCommand()
{
	if (inputcommandlist.empty() || pendingparam.type)
	{
		return ccomm.command;
	}

	list<CommittedCommand>::iterator it = inputcommandlist.begin();

	if (it->type == COMMITTEDCOMMANDTYPE_ERROR)
	{
		LogError(it->sval.c_str());
	}
	else if (((it->type) & COMMITTEDCOMMANDTYPE_COMMAND))
	{
		if (!ccomm.command)
		{
			CreateCommand(it->ival);
		}
		else if (!((it->type) & COMMITTEDCOMMANDTYPE_COMMANDSHORT))
		{
			if (ccomm.command)
			{
				TerminalCommand();
			}
			CreateCommand(it->ival);
		}
		else if (ccomm.command && ((it->type) & COMMITTEDCOMMANDTYPE_SUBCOMMAND))
		{
			pendingparam = (*it);
		}
	}
	else
	{
		pendingparam = (*it);
	}

	inputcommandlist.pop_front();
	return ccomm.command;
}

void Command::ProcessCommand()
{
	while (true)
	{
		ProcessCommittedCommand();

		if (!ccomm.command)
		{
			return;
		}

		switch (ccomm.command)
		{
		case COMM_INITIAL:
			InitialCommand::getInstance().OnProcessCommand();
			break;
		case COMM_NEWLAYER:
			NewLayerCommand::getInstance().OnProcessCommand();
			break;
		case COMM_NEWSUBLAYER:
			NewSubLayerCommand::getInstance().OnProcessCommand();
			break;
		case COMM_PAN:
			GUICoordinate::getInstance().OnProcessPanCommand();
			break;
		case COMM_DOZOOM:
			GUICoordinate::getInstance().OnProcessZoomCommand();
			break;
		case COMM_LINE:
			LineCommand::getInstance().OnProcessCommand();
			break;
		case COMM_BEZIER:
			BezierCommand::getInstance().OnProcessCommand();
			break;
		}

		if (inputcommandlist.empty())
		{
			break;
		}
	}
}

void Command::ProcessUnDoCommand( RevertableCommand * rc, int ucount )
{
	int nsize = rc->commandlist.size();
	assert(ucount <= nsize);

	list<CommittedCommand>::reverse_iterator it=rc->commandlist.rbegin();
	for (int i=nsize; i>ucount; i--)
	{
		++it;
	}
	for (; it!=rc->commandlist.rend(); ++it)
	{
		CommitFrontCommand(*it);
	}
	//
	CommittedCommand tp = pendingparam;
	pendingparam.ClearSet();
	ProcessCommand();
	pendingparam = tp;
}