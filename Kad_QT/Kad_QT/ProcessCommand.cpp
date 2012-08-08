#include "stdafx.h"
#include "Command.h"

#include "GUICoordinate.h"
#include "CommandTemplate.h"

#include "URManager.h"

int Command::ProcessPending( int index, int useflag, int fillprompt, int step, int wantprompt/*=0*/, bool pushback/*=true*/ )
{
	if (!pendingparam.type)
	{
		return -1;
	}
	if (IsCCTypeCommand(pendingparam.type) && IsInternalCommand_CommandEndMark(pendingparam.ival))
	{
		TerminalCommand();
		return -1;
	}

	if (pendingparam.type & COMMITTEDCOMMANDTYPE_SUBCOMMAND)
	{
		DASSERT(pendingparam.csub != 0);
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
		if (!(IsCCTypeCommand(pendingparam.type)&&IsInternalCommand_CommandEndMark(pendingparam.ival)))
		{
			LogError(pendingparam.sval.c_str());
		}
		pendingparam.ClearSet();
		return -1;
	}

	StepTo(step, wantprompt, pushback);
	pendingparam.ClearSet();
	return 0;
}

int Command::ProcessCommittedCommand()
{
	if (inputcommandlist.empty() || pendingparam.type || GetStep()==CSI_FINISH)
	{
		return GetCurrentCommand();
	}

	list<CommittedCommand>::iterator it = inputcommandlist.begin();

	if (it->type == COMMITTEDCOMMANDTYPE_ERROR)
	{
		LogError(it->sval.c_str());
	}
	else if (IsCCTypeCommand(it->type))
	{
		if (it->ival == COMM_UNDO || it->ival == COMM_REDO)
		{
			int comm = it->ival;
			inputcommandlist.pop_front();
			CreateCommand(comm);
			return GetCurrentCommand();
		}
		else
		{
			if (IsInternalCommand_CommandEndMark(it->ival))
			{
			}
			else if (!GetCurrentCommand())
			{
				CreateCommand(it->ival);
			}
			else// if (!((it->type) & COMMITTEDCOMMANDTYPE_COMMANDSHORT))
			{
				if (GetCurrentCommand())
				{
					TerminalCommand();
				}
				CreateCommand(it->ival);
			}
			/*
			else if (GetCurrentCommand() && ((it->type) & COMMITTEDCOMMANDTYPE_SUBCOMMAND))
			{
				pendingparam = (*it);
			}
			*/
		}
	}
	else
	{
		if (GetCurrentCommand())
		{
			pendingparam = *it;
			pendingparam.TranslateObjToID();
		}
		else
		{
			LogError(it->sval.c_str());
		}
	}

	inputcommandlist.pop_front();
	return GetCurrentCommand();
}

bool Command::WillTerminalCurrentCommand()
{
	if (!inputcommandlist.empty())
	{
		if (IsCCTypeCommand(inputcommandlist.front().type))
		{
			return true;
		}
	}
	return false;
}

void Command::UpdateProcessCommand()
{
	ProcessCommand();
	DoPushRevertable();
}

void Command::ProcessCommand()
{
	while (true)
	{
		ProcessCommittedCommand();

		if (!GetCurrentCommand())
		{
			return;
		}

		switch (GetCurrentCommand())
		{
		case COMM_PAN:
			GUICoordinate::getInstance().OnProcessPanCommand();
			break;
		case COMM_DOZOOM:
			GUICoordinate::getInstance().OnProcessZoomCommand();
			break;
		default:
			CommandTemplate * pct = CommandTemplate::GetTemplateByCommand(GetCurrentCommand());
			ASSERT(pct);
			pct->CallProcessCommand();
		}

		if (inputcommandlist.empty())
		{
			break;
		}
	}
//	DoPushRevertable();
}


void Command::ProcessUnDoCommandCommit( RevertableCommand * rc )
{
	for (list<CommittedCommand>::reverse_iterator it=rc->commandlist.rbegin(); it!=rc->commandlist.rend(); ++it)
	{
		CommitFrontCommand(*it);
	}
	//
	CommittedCommand tp = pendingparam;
	pendingparam.ClearSet();
	ProcessCommand();
	pendingparam = tp;
}

void Command::ProcessUnDoCommandParam( int command, RevertableCommand * rc )
{
	CommandTemplate * pct = CommandTemplate::GetTemplateByCommand(command);
	ASSERT(pct);
	pct->CallProcessUnDoCommand(command, rc);
}