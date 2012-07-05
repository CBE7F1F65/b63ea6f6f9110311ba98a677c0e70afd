#include "StdAfx.h"
#include "MoveNodeBatchCommand.h"


MoveNodeBatchCommand::MoveNodeBatchCommand(void)
{
}


MoveNodeBatchCommand::~MoveNodeBatchCommand(void)
{
}

void MoveNodeBatchCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	if (IsStepped())
	{
		if (nowstep > CSI_INIT)
		{
			pcommand->EnableSubCommand(
				(laststep.step==CSI_RESUME)?false:true,
				SSC_TERMINAL,
				SSC_NULL);
		}
	}
	UpdateLastStep();

	int ret = -1;
	int cwpx = /*(comm == COMM_MOVENODE_BATCH)?CWP_X:*/CWP_XOFFSET;
	int cwpy = /*(comm == COMM_MOVENODE_BATCH)?CWP_Y:*/CWP_YOFFSET;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_MOVENODE_BATCH_WANTX,
			cwpx);
	}
	else if (step == CSI_MOVENODE_BATCH_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS, COMMPARAMFLAG_X, cwpx,
			CSI_MOVENODE_BATCH_WANTY, cwpy
			);
	}
	else if (step == CSI_MOVENODE_BATCH_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS, COMMPARAMFLAG_Y, cwpy,
			CSI_MOVENODE_BATCH_WANTINDEXES, CWP_INDEX
			);
	}
	else if (step >= CSI_MOVENODE_BATCH_WANTINDEXES)
	{
		int nowindex = step - CSI_MOVENODE_BATCH_WANTINDEXES;
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_MOVENODE_BATCH_WANTINDEXES+nowindex+1, CWP_INDEX
			);
		if (pcommand->GetParamI(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS+nowindex) == -1)
		{
			pcommand->StepTo(CSI_FINISH);
		}
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
	else if (ret < 0)
	{
		if (step >= CSI_MOVENODE_BATCH_WANTX && step < CSI_MOVENODE_BATCH_WANTINDEXES)
		{
			int iret = pgp->PickPoint();
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					pcommand->SetParamX(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS, pgp->GetPickX_C());
					pcommand->SetParamY(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS, pgp->GetPickY_C());
					pcommand->StepTo(CSI_MOVENODE_BATCH_WANTINDEXES, CWP_INDEX);
				}
			}
		}
	}
}

void MoveNodeBatchCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS);

	float xoffset, yoffset;
	pcommand->GetParamXY(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS, &xoffset, &yoffset);

	PointF2D p(xoffset, yoffset);
	if (p.Equals(PointF2D(0, 0)))
	{
		pcommand->StepTo(CSI_TERMINAL);
		return;
	}

	list<GObject *> lobjs;
	int i=0;
	while (index >= 0)
	{
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		lobjs.push_back(pObj);
		i++;
		index = pcommand->GetParamI(CSP_MOVENODE_BATCH_I_XY_INDEXES_NEWPOS+i);
	}

	pgm->SetLockTreeChange();
	int nMoveActionID = pgm->GetNextMoveActionID();
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		if (!(*it)->CallMoveByOffset(xoffset, yoffset, false, nMoveActionID))
		{
			it = lobjs.erase(it);
		}
		else
		{
			GObject * pMovedObj = *it;
			pgm->OnTreeChanged(pMovedObj->getParent(), pMovedObj);
//			MainInterface::getInstance().CallChangeNode(*it);
			++it;
		}
	}

	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
	{
		ReclingAfterMoveNode(*it, true, &lobjs);
	}

	if (lobjs.empty())
	{
		pcommand->StepTo(CSI_TERMINAL);
	}
	else
	{
		int movedcount = lobjs.size();
		PushRevertableBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 4+movedcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(comm),
			CCMake_F(xoffset),
			CCMake_F(yoffset),
			NULL
			);
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			PushRevertableBatch(
				PUSHREVERTABLESTATE_CONTINUE,
				CCMake_I((*it)->getID()),
				NULL
				);
		}
		PushRevertableBatch(
			PUSHREVERTABLESTATE_END,
			CCMake_I(-1),
			CCMake_C(COMM_I_UNDO_PARAMFROMCOMMAND),
			NULL
			);

	}
}

void MoveNodeBatchCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	float xoffset = it->fval;
	++it;
	float yoffset = it->fval;
	++it;
	pgm->SetLockTreeChange();
	int nMoveActionID = pgm->GetNextMoveActionID();
	for (; it!=--(rc->commandlist.end()); ++it)
	{
		int index = it->ival;
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		pObj->CallMoveByOffset(-xoffset, -yoffset, false, nMoveActionID);
		pgm->OnTreeChanged(pObj->getParent(), pObj);
//		MainInterface::getInstance().CallChangeNode(pObj);
	}

}