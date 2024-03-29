#include "StdAfx.h"
#include "CommandTemplate.h"

#include "GUICursor.h"
#include <sstream>
#include <iomanip>

#include "GObjectManager.h"

CommandTemplate::CommandTemplate(void)
{
	pguic = &GUICoordinate::getInstance();
	pmain = &MainInterface::getInstance();
	pcommand = &Command::getInstance();
	pgm = &GObjectManager::getInstance();
	pgp = &GObjectPicker::getInstance();
	prh = &RenderHelper::getInstance();
	comm = COMM_NULL;
	workinglayerID = 0;
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
		CallDoneCommand();
//		if (comm != COMM_UNDO && comm != COMM_REDO)
//		{
			pcommand->FinishCommand();
//		}
		return CSI_FINISH;
		break;
	case CSI_FINISHCONTINUE:
		ReleaseTarget();
		CallDoneCommand();
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
		pcommand->CreateUnDoCommandCommit();
//		pcommand->CreateCommandCommit(COMM_UNDO);
//		DoUnDo();
//		ReleaseTarget();
		break;
	case SSC_REDO:
		pcommand->CreateReDoCommandCommit();
//		pcommand->CreateCommandCommit(COMM_REDO);
//		DoReDo();
//		ReleaseTarget();
		break;
	case SSC_FINISH:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		CallDoneCommand();
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
	return CCMake_CI(command, CI_MAKESUBEX(paramcount, undocount));
}

CommittedCommand * CommandTemplate::CCMake_CI( int command, int ival )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_COMMAND;
	cc->ival = command;
	if (pcommand->IsInternalCommand(command))
	{
		cc->csub = ival;
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
	stringstream ss;
	ss << fixed << fval;
	cc->sval = ss.str();
	madecctodelete.push_back(cc);
	return cc;
}

CommittedCommand * CommandTemplate::CCMake_I( int ival )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_INT;
	cc->fval = ival;
	cc->ival = ival;
	stringstream ss;
	ss << ival;
	cc->sval = ss.str();
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
	va_end(ap);

	pcommand->PushRevertable(&_rc);

	DeleteCC();
}

void CommandTemplate::PushRevertableBatch( int revertstate, CommittedCommand * first, ... )
{
	if (revertstate == PUSHREVERTABLESTATE_BEGIN)
	{
		rcbatch.Clear();
	}

	va_list ap;
	va_start(ap, first);
	CommittedCommand * vai = first;
	while (vai)
	{
		rcbatch.PushCommand(vai);
		vai = (CommittedCommand*)va_arg(ap, CommittedCommand*);
	}
	va_end(ap);

	if (revertstate == PUSHREVERTABLESTATE_END)
	{
		pcommand->PushRevertable(&rcbatch);
		rcbatch.Clear();
		DeleteCC();
	}
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
	va_end(ap);

	for (list<CommittedCommand*>::iterator it=listfc.begin(); it!=listfc.end(); ++it)
	{
		pcommand->CommitFrontCommand(*(*it));
	}

	DeleteCC();
}

bool CommandTemplate::DoUnDo( int undostep/*=1*/ )
{
	bool bret = pcommand->DoUnDo(undostep);
	ReleaseTarget();
	return bret;
}

bool CommandTemplate::DoReDo( int redostep/*=1*/ )
{
	bool bret = pcommand->DoReDo(redostep);
	ReleaseTarget();
	return bret;
}

void CommandTemplate::ProtectPendingFinishCommand()
{
	CommittedCommand ccp = pcommand->pendingparam;
	pcommand->FinishCommand();

	if (ccp.type)
	{
		CommitFrontCommand(&ccp, NULL);
	}
}

void CommandTemplate::CallDoneCommand()
{
	if (!pcommand->IsUnDoReDoing())
	{
		GLayer * activeUILayer = pgm->GetActiveLayerFromUI();
		GLayer * workinglayer = pgm->GetActiveLayer();
		if (workinglayer != activeUILayer)
		{
			if (workinglayer)
			{
				PushRevertable(
					CCMake_C(COMM_I_COMMAND, 2, 1),
					CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayer->getID()),
					CCMake_C(COMM_SETWORKINGLAYER),
//					CCMake_I(workingLayer->getID()),
					CCMake_I(activeUILayer->getID()),
					CCMake_C(COMM_I_UNDO_PARAM, 1),
					CCMake_I(workinglayer->getID()),
					NULL
					);
			}
		}
		pgm->UpdateActiveLayer(activeUILayer);
	}
	workinglayerID = pgm->GetActiveLayer()->getID();
	OnDoneCommand();
}

void CommandTemplate::InstantProcessCommand()
{
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_FINISH
			);
	}
}

void CommandTemplate::CallProcessCommand()
{
	comm = pcommand->GetCurrentCommand();
	OnProcessCommand();
}

void CommandTemplate::CallProcessUnDoCommand( int _comm, RevertableCommand * rc )
{
	comm = _comm;
	OnProcessUnDoCommand(rc);
}

void CommandTemplate::OnProcessUnDoCommand( RevertableCommand * rc )
{

}

void CommandTemplate::CallOnUnDo()
{
	// lastworkinglayer can be dangerous
	GObjectManager::getInstance().UpdateActiveLayer(NULL, true);
}
