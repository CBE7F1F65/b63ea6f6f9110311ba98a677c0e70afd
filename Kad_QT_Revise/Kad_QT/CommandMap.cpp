#include "StdAfx.h"
#include "CommandMap.h"


CommandState::CommandState( FillCommandParamInfo * fcpinfo )
{
	ASSERT(fcpinfo);
	fcp = *fcpinfo;
	bPickPoint = false;
	pfcb = NULL;
	pMap = NULL;
	nowsubstate = -1;

	subcommandlist.push_back(SSC_UNDO);
	subcommandlist.push_back(SSC_REDO);
	subcommandlist.push_back(SSC_TERMINAL);
}
CommandState::~CommandState()
{

}

void CommandState::AddSubState( CommandSubStateTemplate * substate )
{
	ASSERT(substate);
	substatelist.push_back(substate);
	substate->SetState(this);
	if (substate->RequirePickPoint())
	{
		bPickPoint = true;
	}
}

void CommandState::Clear()
{
	for (list<CommandSubStateTemplate *>::iterator it=substatelist.begin(); it!=substatelist.end(); ++it)
	{
		CommandSubStateTemplate * pSubState = *it;
		pSubState->Clear();
		delete pSubState;
	}
	substatelist.clear();
}

void CommandState::SetPickFilterCallback( PickFilterCallback _pfcb )
{
	pfcb = _pfcb;
}

bool CommandState::ExecuteState( int step )
{
	if (fcp.step != step)
	{
		return false;
	}

	FlushSubCommand(step);

	CommandTemplate * pct = getCommandTemplate();

	if (step == CSI_INIT)
	{
		pct->pcommand->StepTo(fcp.tostep, fcp.wantprompt, fcp.pushback);
		return true;
	}

	int ret = -1;
	if (nowsubstate < 0)
	{
		ret = pct->pcommand->ProcessPending(fcp.index, fcp.useflag, fcp.fillprompt, fcp.tostep, fcp.wantprompt, fcp.pushback);
	}
	if (!ret)
	{
		// Processed
	}
	else if (ret < 0)
	{
		// Routine
		if (bPickPoint)
		{
			int iret = pct->pgp->PickPoint(pfcb);
			bool bReady = pct->pgp->IsPickReady(iret) && !pct->pcommand->IsInternalProcessing();

			for (list<CommandSubStateTemplate *>::iterator it=substatelist.begin(); it!=substatelist.end(); ++it)
			{
				if ((*it)->RequirePickPoint())
				{
					if ((*it)->SetPickParam(iret, bReady))
					{
						return true;
					}
				}
			}
		}
	}
	else if (ret > 0)
	{
		nowsubstate = ret;
	}

	if (nowsubstate >= 0)
	{
		// Enter Sub
		for (list<CommandSubStateTemplate *>::iterator it=substatelist.begin(); it!=substatelist.end(); ++it)
		{
			if ((*it)->CallExcuteSubState(nowsubstate))
			{
				pct->pcommand->FinishPendingSubCommand();
				return true;
			}
		}
	}

	if (nowsubstate >= 0)
	{
		pct->DispatchNormalSubCommand(ret);
	}

	return true;
}

CommandTemplate * CommandState::getCommandTemplate()
{
	return getMap()->getCommandTemplate();
}

void CommandState::FlushSubCommand( int step )
{
	int laststep = getMap()->laststep;
	if (laststep != step)
	{
		Command::getInstance().EnableSubCommand(
			(laststep==CSI_RESUME)?false:true,
			&subcommandlist);
	}
}

bool CommandState::EnableSubCommand( int ssc )
{
	if (!subcommandlist.empty())
	{
		for (list<int>::iterator it=subcommandlist.begin(); it!=subcommandlist.end(); ++it)
		{
			if (*it == ssc)
			{
				return false;
			}
		}
	}
	subcommandlist.push_back(ssc);
	return true;
}

bool CommandState::DisableSubCommand( int ssc )
{
	if (!subcommandlist.empty())
	{
		for (list<int>::iterator it=subcommandlist.begin(); it!=subcommandlist.end(); ++it)
		{
			if (*it == ssc)
			{
				it = subcommandlist.erase(it);
				return true;
			}
		}
	}
	return false;
}

CommandSubStateTemplate::CommandSubStateTemplate()
{
	pState = NULL;
	subindex = -1;
}

CommandSubStateTemplate::~CommandSubStateTemplate()
{

}

void CommandSubStateTemplate::SetSubCommandIndex( int index/*=-1*/ )
{
	subindex = index;
}

void CommandSubStateTemplate::Clear()
{
}

CommandMap * CommandSubStateTemplate::getMap()
{
	return getState()->getMap();
}

bool CommandSubStateTemplate::CallExcuteSubState( int nowsubstate )
{
	if (subindex != nowsubstate)
	{
		return false;
	}
	return ExcuteSubState();
}

CommandTemplate * CommandSubStateTemplate::getCommandTemplate()
{
	return getMap()->getCommandTemplate();
}
CommandMap::CommandMap(void)
{
	pct = NULL;
}


CommandMap::~CommandMap(void)
{
}

void CommandMap::AddState( CommandState * state )
{
	ASSERT(state);
	statelist.push_back(state);
	state->SetMap(this);
}

void CommandMap::Clear()
{
	for (list<CommandState *>::iterator it=statelist.begin(); it!=statelist.end(); ++it)
	{
		CommandState * pState = *it;
		pState->Clear();
		delete pState;
	}
	statelist.clear();
	pct = NULL;
}

bool CommandMap::ExecuteMap( int step, int nowstep )
{
	bool bret = false;
	for (list<CommandState *>::iterator it=statelist.begin(); it!=statelist.end(); ++it)
	{
		if ((*it)->ExecuteState(step))
		{
			bret = true;
			break;
		}
	}
	laststep = step;
	return bret;
}

void CommandMap::SetCommandTemplate( CommandTemplate * _pct )
{
	pct = _pct;
}