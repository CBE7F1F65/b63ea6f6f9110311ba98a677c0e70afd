#include "StdAfx.h"
#include "CommandTemplate.h"

#include "GUICursor.h"
#include <sstream>
#include <iomanip>

#include "GObjectManager.h"
#include "PickFilterTemplate.h"

GBaseNode CommandTemplate::tBaseNode;

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
		CallInitCommand();
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

CommittedCommand * CommandTemplate::CCMake_O( GObject * pObj )
{
	CommittedCommand * cc = new CommittedCommand();
	cc->type = COMMITTEDCOMMANDTYPE_OBJ;
	cc->ival = (int)pObj;
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

void CommandTemplate::PushRevertible( CommittedCommand * first, ... )
{
	if (!first)
	{
		return;
	}

	RevertibleCommand _rc;

	va_list ap;
	va_start(ap, first);
	CommittedCommand * vai = first;
	while (vai)
	{
		_rc.PushCommand(vai);
		vai = (CommittedCommand*)va_arg(ap, CommittedCommand*);
	}
	va_end(ap);

	pcommand->PushRevertible(&_rc);

	DeleteCC();
}

void CommandTemplate::PushRevertibleBatch( int revertstate, CommittedCommand * first, ... )
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
		pcommand->PushRevertible(&rcbatch);
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
	CallClearCommand();
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
				PushRevertible(
					CCMake_C(COMM_I_COMMAND, 2, 1),
					CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayer->getID()),
					CCMake_C(COMM_SETWORKINGLAYER),
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
	CallClearTemp();
}

void CommandTemplate::CallClearCommand()
{
	OnClearCommand();
	CallClearTemp();
}

void CommandTemplate::OnClearCommand()
{

}

void CommandTemplate::OnClearTemp()
{

}

void CommandTemplate::CallClearTemp()
{
	OnClearTemp();
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
	RenderToTarget();
}

void CommandTemplate::CallProcessUnDoCommand( int _comm, RevertibleCommand * rc )
{
	comm = _comm;
	OnProcessUnDoCommand(rc);
}

void CommandTemplate::OnProcessUnDoCommand( RevertibleCommand * rc )
{

}

void CommandTemplate::CallOnReDo()
{
	// lastworkinglayer can be dangerous
	GObjectManager::getInstance().UpdateActiveLayer(NULL, true);
}

void CommandTemplate::CallInitCommand()
{
	CallClearTemp();
	OnInitCommand();
}

void CommandTemplate::OnInitCommand()
{

}

void CommandTemplate::OnTerminalCommand()
{
}

void CommandTemplate::CallTerminalCommand()
{
	OnTerminalCommand();
	ReleaseTarget();
	CallClearTemp();
}

GObject * CommandTemplate::TestPickSingleFilter( float x, float y, GObject * pFilterObj, float * pfProportion )
{
	GObjectPicker * pTestPicker = &GObjectPicker::getTestPicker();
	int iret = 0;
	PickFilterTemplate * pFilterTemplate = NULL;
	if (pFilterObj)
	{
		if (pFilterObj->isPoint())
		{
			pFilterTemplate = &PickFilterSinglePoint::getInstance();
			PickFilterCallback pf = ((PickFilterSinglePoint *)pFilterTemplate)->Use((GPoint *)pFilterObj);
			iret = pTestPicker->TestPickPoint(x, y, pfProportion, pf, pf);
		}
		else
		{
			pFilterTemplate = &PickFilterSingleObj::getInstance();
			PickFilterCallback pf = ((PickFilterSingleObj *)pFilterTemplate)->Use(pFilterObj);
			iret = pTestPicker->TestPickPoint(x, y, pfProportion, pf, pf);
		}
	}
	else
	{
		iret = pTestPicker->TestPickPoint(x, y, pfProportion);
	}
	GObject * pRet = pTestPicker->GetPickedObj();
	if (pFilterTemplate)
	{
		pFilterTemplate->Dispose();
	}
	return pRet;
}

bool CommandTemplate::MergeClingNewPoint( GPoint * pFrom, GObject * pTo, float fProportion/*=0 */ )
{
	ASSERT(pFrom->isPoint());

	if (pTo->isPoint() && pTo->isSlaveToLine())
	{
		if (!pTo->isMidPoint())
		{
			ASSERT(false);
			return false;
		}
		pTo = pTo->getLine();
//		fProportion = ((GLine *)pTo)->CalculateMidPointProportion();
	}
	if (pTo->isPoint())
	{
		GPoint * pToPoint = (GPoint *)pTo;
		if (pToPoint->getMergeWith()->empty() && pFrom->getMergeWith()->empty())
		{
			if (pFrom->MergeWith(pToPoint))
			{
				PushRevertible(
					CCMake_C(COMM_I_COMMAND, 3),
					CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
					CCMake_C(COMM_MERGE),
					CCMake_I(pFrom->getID()),
					CCMake_I(pTo?pTo->getID():-1),
					NULL
					);
				/*
				CommitFrontCommand(
					CCMake_C(COMM_MERGE),
					CCMake_O(pFrom),
					CCMake_O(pTo),
					NULL
					);
					*/

			}
		}
		GClingInfo * pcli = pToPoint->getClingInfo();
		GLine * pToCling = pcli->GetClingTo();
		if (pToCling)
		{
			if (pFrom->ClingTo(*pcli))
			{
				PushRevertible(
					CCMake_C(COMM_I_COMMAND, 5),
					CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
					CCMake_C(COMM_CLING),
					CCMake_I(pFrom->getID()),
					CCMake_I(pToCling?pToCling->getID():-1),
					CCMake_I(pcli->GetClingType()),
					CCMake_F(pcli->GetClingVal()),
					NULL
					);
				/*
				CommitFrontCommand(
					CCMake_C(COMM_CLING),
					CCMake_O(pFrom),
					CCMake_O(pToCling),
					CCMake_F(pToPoint->getClingProportion()),
					NULL);
					*/
			}
		}
	}
	if (pTo->isLine())
	{
		if (pFrom->ClingTo((GLine *)pTo, fProportion))
		{
			PushRevertible(
				CCMake_C(COMM_I_COMMAND, 5),
				CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
				CCMake_C(COMM_CLING),
				CCMake_I(pFrom->getID()),
				CCMake_I(pTo?pTo->getID():-1),
				CCMake_I(GCLING_PROPORTION),
				CCMake_F(fProportion),
				NULL
				);
			/*
			CommitFrontCommand(
				CCMake_C(COMM_CLING),
				CCMake_O(pFrom),
				CCMake_O(pTo),
				CCMake_F(fProportion),
				NULL
				);
				*/

		}
	}
	return true;
}

void CommandTemplate::ReAttachAfterMoveNode( GObject * pObj, bool bFindMerge/*=true*/, list<GObject *>* lObjs/*=0 */ )
{
	ASSERT(pObj);
	if (pgm->IsIsolateMode())
	{
		pObj->Isolate();
	}
	if (!pObj->getChildren()->empty())
	{
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			ReAttachAfterMoveNode(*it, false, lObjs);
		}
	}
	if (!pObj->canAttach())
	{
		return;
	}
	if (!pObj->isPoint())
	{
		return;
	}

	GPoint * pPoint = (GPoint *)pObj;
	if (bFindMerge)
	{
		if (!pPoint->getMergeWith()->empty())
		{
			for (list<GPoint *>::iterator it=pPoint->getMergeWith()->begin(); it!=pPoint->getMergeWith()->end(); ++it)
			{
				ReAttachAfterMoveNode(*it, false, lObjs);
			}
		}
	}
	
	if (lObjs)
	{
		for (list<GObject *>::iterator it=lObjs->begin(); it!=lObjs->end(); ++it)
		{
			GObject * pAnotherObj = *it;
			GPoint * pAnotherPoint = NULL;
			if (pAnotherObj->isPoint())
			{
				pAnotherPoint = (GPoint *)pAnotherObj;
				if (pPoint->isMergeWith(pAnotherPoint))
				{
					return;
				}
			}
			if (pPoint->isClingTo(pAnotherObj))
			{
				return;
			}
		}
	}

	float fProportion;
	GObject * pTestPickedObj = TestPickObjSingleFilter(pObj, pObj, &fProportion);


	if (pTestPickedObj)
	{
		if (pPoint->isNotch())
		{
			if (pTestPickedObj->isAnchorPoint())
			{
				GLine * pTestLine = pTestPickedObj->getLine();
				if (pTestPickedObj == pTestLine->GetBeginPoint())
				{
					fProportion = 0.0f;
				}
				else
				{
					fProportion = 1.0f;
				}
				pTestPickedObj = pTestLine;
			}
		}
		if (pTestPickedObj->isMidPoint())
		{
			pTestPickedObj = pTestPickedObj->getLine();
//			fProportion = ((GLine *)pTestPickedObj)->CalculateMidPointProportion();
		}
		if (!pTestPickedObj->canBeClingTo() && !pTestPickedObj->canBeMergedWith())
		{
			return;
		}
	}


	if ((!pTestPickedObj || pTestPickedObj->canBeClingTo()) && !pPoint->isHandlePoint())
	{
		float fAllowance = pPoint->GetClingAllowance();
		if (fAllowance <= 0 || (fProportion >= M_FLOATEXTREMEEPS && fProportion <= 1-M_FLOATEXTREMEEPS))
		{
			GClingInfo tcli = *pPoint->getClingInfo();
			if (tcli.ApplyChange((GLine *)pTestPickedObj, fProportion, fAllowance))
			{
				if (pPoint->ClingTo(tcli))
				{
					PushRevertible(
						CCMake_C(COMM_I_COMMAND, 5),
						CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
						CCMake_C(COMM_CLING),
						CCMake_I(pPoint->getID()),
						CCMake_I(pTestPickedObj?pTestPickedObj->getID():-1),
						CCMake_I(tcli.GetClingType()),
						CCMake_F(tcli.GetClingVal()),
						NULL
						);
				}
			}
		}
	}
	if (pTestPickedObj && pTestPickedObj->canBeMergedWith())
	{
		GPoint * pTestPickedPoint = (GPoint *)pTestPickedObj;
//		if (!pPoint->isMergeWith(pTestPickedPoint))
		if (pPoint->getMergeWith()->empty() && pTestPickedPoint->getMergeWith()->empty())
		{
			pPoint->MergeWith(pTestPickedPoint);
			PushRevertible(
				CCMake_C(COMM_I_COMMAND, 3),
				CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
				CCMake_C(COMM_MERGE),
				CCMake_I(pPoint->getID()),
				CCMake_I(pTestPickedPoint?pTestPickedPoint->getID():-1),
				NULL
				);
			/*
			CommitFrontCommand(
				CCMake_C(COMM_MERGE),
				CCMake_O(pObj),
				CCMake_O(pTestPickedObj),
				NULL
				);
				*/
		}
	}
}

bool CommandTemplate::BindNewAnchorPoint( GAnchorPoint * pOld, GAnchorPoint * pNew )
{
	ASSERT(pOld);
	ASSERT(pNew);

	GHandlePoint * pOldHandle = pOld->GetHandle();
	GHandlePoint * pNewHandle = pNew->GetHandle();
	if ((pNew->getLine())->isStraightLine())
	{
		return false;
	}
	if (!pOldHandle->getBindWith() && !pNewHandle->getBindWith())
	{
		pOldHandle->BindWith(pNewHandle);
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 3),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_BINDHANDLE),
			CCMake_I(pOldHandle->getID()),
			CCMake_I(pNewHandle->getID()),
			NULL
			);
		/*
		CommitFrontCommand(
			CCMake_C(COMM_BINDHANDLE),
			CCMake_O(pOldHandle),
			CCMake_O(pNewHandle),
			NULL
			);
			*/
		return true;
	}
	return false;
}

void CommandTemplate::Release()
{
	tBaseNode.RemoveAllChildren(true);
}

void CommandTemplate::Terminal( int iReason/*=-1*/ )
{
	pcommand->TerminalCommand();
}