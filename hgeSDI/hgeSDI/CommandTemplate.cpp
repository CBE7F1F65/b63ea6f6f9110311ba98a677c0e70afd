#include "StdAfx.h"
#include "CommandTemplate.h"

#include "GUICursor.h"


CommandTemplate::CommandTemplate(void)
{
	pguic = &GUICoordinate::getInstance();
	pmain = &MainInterface::getInstance();
	pcommand = &Command::getInstance();
}


CommandTemplate::~CommandTemplate(void)
{
}

void CommandTemplate::ReleaseTarget()
{
	pcommand->SetRenderTarget(0);
}

void CommandTemplate::RenderToTarget()
{

}

int CommandTemplate::OnNormalProcessCommand( int cursorindex/*=-1*/ )
{
	int step = pcommand->GetStep();

	switch (step)
	{
	case CSI_INIT:
		laststep.ClearSet();
		if (cursorindex >= 0)
		{
			GUICursor::getInstance().ChangeCursor(cursorindex);
		}
		break;
	case CSI_PAUSE:
		ReleaseTarget();
		break;
	case CSI_RESUME:
		if (cursorindex >= 0)
		{
			GUICursor::getInstance().ChangeCursor(cursorindex);
		}
		break;

	case CSI_FINISH:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		DoneCommand();
		pcommand->FinishCommand();
		return CSI_FINISH;
		break;
	case CSI_FINISHCONTINUE:
		ReleaseTarget();
		DoneCommand();
		return CSI_FINISHCONTINUE;
		break;
	case CSI_TERMINAL:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		pcommand->TerminalCommand();
		return CSI_TERMINAL;
		break;
	}
	return step;
}

void CommandTemplate::DispatchNormalSubCommand( int subcommand )
{
	switch (subcommand)
	{
	case SSC_UNDO:
		DoUnDo();
		ReleaseTarget();
		break;
	case SSC_REDO:
		DoReDo();
		ReleaseTarget();
		break;
	case SSC_FINISH:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		DoneCommand();
		pcommand->FinishCommand();
		break;
	case SSC_TERMINAL:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		pcommand->TerminalCommand();
		break;
	}
}

void CommandTemplate::UpdateLastStep()
{
	laststep = pcommand->ccomm;
}

bool CommandTemplate::IsStepped()
{
	return (laststep.command && laststep.step != pcommand->GetStep());
}

CommittedCommand * CommandTemplate::CCMake_C( int command, int paramcount/*=0*/, int undocount/*=0*/ )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_COMMAND;
	cc->ival = command;
	if (command > _COMM_INTERNALBEGIN)
	{
		cc->csub = CI_MAKESUBEX(paramcount, undocount);
	}
	cc->sval = pcommand->GetCommandStr(command);
	madecctodelete.push_back(cc);
	return cc;
}

CommittedCommand * CommandTemplate::CCMake_F( float fval )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_FLOAT;
	cc->fval = fval;
	cc->ival = fval;
	madecctodelete.push_back(cc);
	return cc;
}

CommittedCommand * CommandTemplate::CCMake_I( int ival )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_INT;
	cc->fval = ival;
	cc->ival = ival;
	madecctodelete.push_back(cc);
	return cc;
}

CommittedCommand * CommandTemplate::CCMake_S( const char * sval )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_STRING;
	cc->sval = sval;
	madecctodelete.push_back(cc);
	return cc;
}

CommittedCommand * CommandTemplate::CCMake_Sub( int subcommand )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_SUBCOMMAND;
	cc->csub = subcommand;
	cc->sval = pcommand->GetSubCommandStr(subcommand);
	madecctodelete.push_back(cc);
	return cc;
}

void CommandTemplate::DeleteCC()
{
	for (list<CommittedCommand*>::iterator it=madecctodelete.begin(); it!=madecctodelete.end(); ++it)
	{
		if (*it)
		{
			delete (*it);
		}
	}
	madecctodelete.clear();
}

void CommandTemplate::PushRevertable( CommittedCommand * first, ... )
{
	if (!first)
	{
		return;
	}

	RevertableCommand _rc;

	va_list ap;
	va_start(ap, first);
	CommittedCommand * vai = first;
	while (vai)
	{
		_rc.PushCommand(vai);
		vai = (CommittedCommand*)va_arg(ap, CommittedCommand*);
	}

	pcommand->PushRevertable(&_rc);

	DeleteCC();
}

void CommandTemplate::CommitFrontCommand( CommittedCommand * first, ... )
{
	if (!first)
	{
		return;
	}

	list<CommittedCommand *> listfc;

	va_list ap;
	va_start(ap, first);
	CommittedCommand * vai = first;
	while (vai)
	{
		listfc.push_front(vai);
		vai = (CommittedCommand *)va_arg(ap, CommittedCommand *);
	}

	for (list<CommittedCommand*>::iterator it=listfc.begin(); it!=listfc.end(); ++it)
	{
		pcommand->CommitFrontCommand(*(*it));
	}

	DeleteCC();
}

bool CommandTemplate::DoUnDo( int undostep/*=1*/ )
{
	return pcommand->DoUnDo(undostep);
}

bool CommandTemplate::DoReDo( int redostep/*=1*/ )
{
	return pcommand->DoReDo(redostep);
}