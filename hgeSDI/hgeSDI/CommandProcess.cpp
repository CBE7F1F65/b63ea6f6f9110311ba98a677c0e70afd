#include "stdafx.h"
#include "Command.h"

#include "GUICoordinate.h"
#include "LineCommand.h"
#include "BezierCommand.h"


bool Command::ProcessPending( int index, int useflag, int fillprompt, int step, int wantprompt/*=0*/, bool pushback/*=true*/ )
{
	if (!pendingparam.type)
	{
		return false;
	}

	switch (useflag)
	{
	case COMMPARAMFLAG_X:
		SetParamX(index, pendingparam.fval, fillprompt);
		break;
	case COMMPARAMFLAG_Y:
		SetParamY(index, pendingparam.fval, fillprompt);
		break;
	case COMMPARAMFLAG_F:
		SetParamF(index, pendingparam.fval, fillprompt);
		break;
	case COMMPARAMFLAG_I:
		SetParamI(index, pendingparam.ival, fillprompt);
		break;
	case COMMPARAMFLAG_S:
		SetParamS(index, pendingparam.sval.c_str(), fillprompt);
		break;
	case COMMPARAMFLAG_G:
		SetParamG(index, pendingparam.ival, fillprompt);
		break;
	default:
		LogError(pendingparam.sval.c_str());
		pendingparam.ClearSet();
		return false;
	}

	StepTo(step, wantprompt, pushback);
	pendingparam.ClearSet();
	return true;
}

int Command::ProcessCommittedCommand()
{
	if (inputcommandlist.empty() || pendingparam.type)
	{
		return ccomm.command;
	}
	/*
	if (ccomm.command && ccomm.step <= CSI_INIT)
	{
		return ccomm.command;
	}
	*/

	list<CommittedCommand>::iterator it = inputcommandlist.begin();

	if (it->type == COMMITTEDCOMMANDTYPE_ERROR)
	{
		LogError(it->sval.c_str());
//		return ccomm.command;
	}
	else if (it->type == COMMITTEDCOMMANDTYPE_COMMAND)
	{
		if (ccomm.command)
		{
			TerminalCommand();
		}
		CreateCommand(it->ival);
//		return ccomm.command;
	}
	else// if (it->type == COMMITTEDCOMMANDTYPE_FLOAT)
	{
		pendingparam = (*it);
		/*
		switch (ccomm.wantnext)
		{
		case COMMPARAMFLAG_X:
			SetParamX(ccomm.wantnextindex, it->fval);
			break;
		case COMMPARAMFLAG_Y:
			SetParamY(ccomm.wantnextindex, it->fval);
			break;
		case COMMPARAMFLAG_F:
			SetParamF(ccomm.wantnextindex, it->fval);
			break;
		case COMMPARAMFLAG_I:
			SetParamI(ccomm.wantnextindex, it->ival);
			break;
		case COMMPARAMFLAG_G:
			SetParamG(ccomm.wantnextindex, it->ival);
			break;
		case COMMPARAMFLAG_S:
			SetParamS(ccomm.wantnextindex, it->sval.c_str());
			break;
		default:
			LogError(it->sval.c_str());
			break;
		}
		*/
	}


	inputcommandlist.pop_front();
	return ccomm.command;
}

void Command::ProcessCommand()
{
	ProcessCommittedCommand();
	if (!ccomm.command)
	{
		return;
	}

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
