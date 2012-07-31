#include "StdAfx.h"
#include "FlipNodeBatchCommand.h"
#include "MarkingManager.h"
#include "RenderTargetManager.h"


FlipNodeBatchCommand::FlipNodeBatchCommand(void)
{
	pTempLine = NULL;
}


FlipNodeBatchCommand::~FlipNodeBatchCommand(void)
{
}

void FlipNodeBatchCommand::OnProcessCommand()
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

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_FLIPNODE_BATCH_WANTX, CWP_X);
	}
	else if (step == CSI_FLIPNODE_BATCH_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_X, CWP_X,
			CSI_FLIPNODE_BATCH_WANTY, CWP_Y
			);
	}
	else if (step == CSI_FLIPNODE_BATCH_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_Y, CWP_Y,
			CSI_FLIPNODE_BATCH_WANTANGLE, CWP_ANGLE
			);
	}
	else if (step == CSI_FLIPNODE_BATCH_WANTANGLE)
	{
		ret = pcommand->ProcessPending(
			CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_F, CWP_ANGLE,
			CSI_FLIPNODE_BATCH_WANTINDEXES, CWP_INDEX
			);
	}
	else if (step >= CSI_FLIPNODE_BATCH_WANTINDEXES)
	{
		int nowindex = step - CSI_FLIPNODE_BATCH_WANTINDEXES;
		if (bManualMode)
		{
			if (nowindex)
			{
				pcommand->SetParamI(CSP_FLIPNODE_BATCH_I_INDEXES+nowindex, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int i=nowindex;
				for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it, i++)
				{
					pcommand->SetParamI(CSP_FLIPNODE_BATCH_I_INDEXES+i, (*it)->getID(), CWP_INDEX);
				}
				pcommand->SetParamI(CSP_FLIPNODE_BATCH_I_INDEXES+i, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else
		{
			ret = pcommand->ProcessPending(
				CSP_FLIPNODE_BATCH_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
				CSI_FLIPNODE_BATCH_WANTINDEXES+nowindex+1, CWP_INDEX
				);
			if (pcommand->GetParamI(CSP_FLIPNODE_BATCH_I_INDEXES+nowindex) < 0)
			{
				pcommand->StepTo(CSI_FINISH);
			}

		}
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
	else if (ret < 0)
	{
		if (step > CSI_INIT)
		{
			if (!bManualMode)
			{
				lstObj = *(pgm->GetSelectedNodes(false));
				if (lstObj.empty())
				{
					pcommand->StepTo(CSI_TERMINAL);
					return;
				}
				lstObj.sort();
			}
			bManualMode = true;
		}


		if (step >= CSI_FLIPNODE_BATCH_WANTX && step < CSI_FLIPNODE_BATCH_WANTINDEXES)
		{
			int iret = pgp->PickPoint();
			if (step < CSI_FLIPNODE_BATCH_WANTANGLE)
			{
				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						pcommand->SetParamX(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, pgp->GetPickX_C());
						pcommand->SetParamY(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, pgp->GetPickY_C());
						pcommand->StepTo(CSI_FLIPNODE_BATCH_WANTANGLE, CWP_ANGLE);
					}
				}
			}
			else
			{
				if (!pTempLine)
				{
					pTempLine = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
					pTempLine->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);

					MarkingLine * pMarking = new MarkingLine(pTempLine, MARKFLAG_ANGLE);
					MarkingUI * pmuiAngle = pMarking->getMarkingUI(MARKFLAG_ANGLE);
					pmuiAngle->SetEditable(true);
					pmuiAngle->SetCallback(staticMIDCBAngle);
					MarkingManager::getInstance().EnableMarking(pMarking);
				}
				pgp->SetCheckMouseDown();
				float orix, oriy;
				pcommand->GetParamXY(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);

				float pickx = pgp->GetPickX_C();
				float picky = pgp->GetPickY_C();
				pTempLine->SetBeginEnd(orix, oriy, pickx, picky);

				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						int angle = MathHelper::getInstance().GetLineAngle(pTempLine->GetBeginPoint()->GetPointF2D(), pTempLine->GetEndPoint()->GetPointF2D());
						float fAngle = angle*90.0f/ANGLEBASE_90;
						pcommand->SetParamF(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, fAngle, CWP_ANGLE);
						pcommand->StepTo(CSI_FLIPNODE_BATCH_WANTINDEXES, CWP_INDEX);
					}
				}
			}
		}
	}
	RenderToTarget();
}

void FlipNodeBatchCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_FLIPNODE_BATCH_WANTX && nstep <= CSI_FLIPNODE_BATCH_WANTINDEXES && pTempLine)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		pTempLine->CallRender();
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}
void FlipNodeBatchCommand::OnDoneCommand()
{
	float orix, oriy;
	pcommand->GetParamXY(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);
	float fAngle = pcommand->GetParamF(CSP_FLIPNODE_BATCH_F_XY_ANGLE_ORIGIN);
	int angle = fAngle*ANGLEBASE_90/90;

	MathHelper::getInstance().RestrictAngle(&angle);
	
	int index = pcommand->GetParamI(CSP_FLIPNODE_BATCH_I_INDEXES);
	list<GObject *> lobjs;
	int i=0;
	while (index >= 0)
	{
		GObject * pObj = pgm->FindObjectByID(index);
		if (pObj)
		{
			lobjs.push_back(pObj);
		}
		i++;
		index = pcommand->GetParamI(CSP_FLIPNODE_BATCH_I_INDEXES+i);
	}

	if (lobjs.empty())
	{
		Terminal();
		return;
	}

	pgm->SetLockTreeChange();
	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_FLIP, angle);
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		GObject * pMoveObj = *it;

		if (!(*it)->CallFlip(pMoveObj, orix, oriy, angle, false, moveActionID))
		{
			it = lobjs.erase(it);
		}
		else
		{
			pgm->OnTreeChanged(pMoveObj->getParent(), pMoveObj);
			++it;
		}
	}

	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
	{
		ReAttachAfterMoveNode(*it, true, &lobjs);
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
			CCMake_C(COMM_I_COMMAND, 5+movedcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_FLIPNODE_BATCH),
			CCMake_F(orix),
			CCMake_F(oriy),
			CCMake_F(fAngle),
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

void FlipNodeBatchCommand::OnClearTemp()
{
	bManualMode = false;
	lstObj.clear();
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
	}
	pgp->UnlockAngles();
}

void FlipNodeBatchCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	float orix = it->fval;
	++it;
	float oriy = it->fval;
	++it;
	float fVal = it->fval;
	int angle = fVal*ANGLEBASE_90/90;
	MathHelper::getInstance().RestrictAngle(&angle);
	++it;
	pgm->SetLockTreeChange();

	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_FLIP, angle);
	for (; it!=--(rc->commandlist.end()); ++it)
	{
		int index = it->ival;
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		pObj->CallFlip(pObj, orix, oriy, angle, false, moveActionID);
		pgm->OnTreeChanged(pObj->getParent(), pObj);
	}
}

bool FlipNodeBatchCommand::staticMIDCBAngle( MarkingUI * pmui, bool bAccept )
{
	return getInstance().MIDCBAngle(pmui, bAccept);
}

bool FlipNodeBatchCommand::MIDCBAngle( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fAngle = pmui->getFloat(&bOk);
		if (bOk)
		{
			int nLockedAngle = fAngle * ANGLEBASE_90/90;
			float x, y;
			pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &x, &y);
			pgp->SetLockOrigin(x, y);
			pgp->SetLockAngle(nLockedAngle);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockAngles();
	}
	return true;

}
