#include "StdAfx.h"
#include "RotateNodeBatchCommand.h"
#include "MarkingManager.h"
#include "MarqueeSelect.h"


RotateNodeBatchCommand::RotateNodeBatchCommand(void)
{
	pTempLine = NULL;
	ClearTemp();
}


RotateNodeBatchCommand::~RotateNodeBatchCommand(void)
{
}

void RotateNodeBatchCommand::OnProcessCommand()
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
			CSI_ROTATENODE_BATCH_WANTX, CWP_X);
	}
	else if (step == CSI_ROTATENODE_BATCH_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_X, CWP_X,
			CSI_ROTATENODE_BATCH_WANTY, CWP_Y
			);
	}
	else if (step == CSI_ROTATENODE_BATCH_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_Y, CWP_Y,
			CSI_ROTATENODE_BATCH_WANTANGLE, CWP_ANGLE
			);
	}
	else if (step == CSI_ROTATENODE_BATCH_WANTANGLE)
	{
		ret = pcommand->ProcessPending(
			CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, COMMPARAMFLAG_F, CWP_ANGLE,
			CSI_ROTATENODE_BATCH_WANTINDEXES, CWP_INDEX
			);
	}
	else if (step >= CSI_ROTATENODE_BATCH_WANTINDEXES)
	{
		int nowindex = step - CSI_ROTATENODE_BATCH_WANTINDEXES;
		if (bManualMode)
		{
			if (nowindex)
			{
				pcommand->SetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+nowindex, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int i=nowindex;
				for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it, i++)
				{
					pcommand->SetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+i, (*it)->getID(), CWP_INDEX);
				}
				pcommand->SetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+i, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else
		{
			ret = pcommand->ProcessPending(
				CSP_ROTATENODE_BATCH_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
				CSI_ROTATENODE_BATCH_WANTINDEXES+nowindex+1, CWP_INDEX
				);
			if (pcommand->GetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+nowindex) < 0)
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


		if (step >= CSI_ROTATENODE_BATCH_WANTX && step < CSI_ROTATENODE_BATCH_WANTINDEXES)
		{
			int iret = pgp->PickPoint(staticFilterCallback);
			if (step < CSI_ROTATENODE_BATCH_WANTANGLE)
			{
				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						pcommand->SetParamX(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, pgp->GetPickX_C());
						pcommand->SetParamY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, pgp->GetPickY_C());
						pcommand->StepTo(CSI_ROTATENODE_BATCH_WANTANGLE, CWP_ANGLE);
					}
				}
			}
			else
			{
				if (!pTempLine)
				{
					pTempLine = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
					int nMarkFlag;
					if (comm == COMM_ROTATE_BATCH)
					{
						nMarkFlag = MARKFLAG_ANGLE;
					}
					else
					{
						nMarkFlag = MARKFLAG_LENGTH;
					}
					MarkingLine * pMarking = new MarkingLine(pTempLine, nMarkFlag);
					if (comm == COMM_ROTATE_BATCH)
					{
						MarkingUI * pmuiAngle = pMarking->getMarkingUI(MARKFLAG_ANGLE);
						pmuiAngle->SetEditable(true);
						pmuiAngle->SetCallback(staticMIDCBAngle);
					}
					else
					{
						MarkingUI * pmuiLength = pMarking->getMarkingUI(MARKFLAG_LENGTH);
						pmuiLength->SetEditable(true);
						pmuiLength->SetCallback(staticMIDCBLength);
					}
					MarkingManager::getInstance().EnableMarking(pMarking);

					pgp->UnlockAngles();
					pgp->UnlockLength();
				}

				float orix, oriy;
				pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);

				float pickx = pgp->GetPickX_C();
				float picky = pgp->GetPickY_C();
				pTempLine->SetBeginEnd(orix, oriy, pickx, picky);

				int angle = MathHelper::getInstance().GetLineAngle(pTempLine->GetBeginPoint()->GetPointF2D(), pTempLine->GetEndPoint()->GetPointF2D());
				float fNowLength = pTempLine->getLength();
				float fVal;
				if (comm != COMM_ROTATE_BATCH)
				{
					if (pgp->IsMouseDownReady())
					{
						if (fBeginLength <= 0)
						{
							fBeginLength = fNowLength;
						}
					}
				}
				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						pgm->EndTryMove();
						if (comm == COMM_ROTATE_BATCH)
						{
							fVal = angle*90.0f/ANGLEBASE_90;
						}
						else
						{
							fVal = fNowLength/fBeginLength;
						}
						pcommand->SetParamF(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, fVal, CWP_ANGLE);
						pcommand->StepTo(CSI_ROTATENODE_BATCH_WANTINDEXES, CWP_INDEX);
					}
				}
				else
				{
					pgm->BeginTryMove();
					int nMoveType;
					float fScaleX=1.0f;
					float fScaleY=1.0f;
					int nMoveAngle=0;
					if (comm == COMM_ROTATE_BATCH)
					{
						nMoveType = GMMATYPE_ROTATE;
						nMoveAngle = angle-lastAngle;
					}
					else
					{
						nMoveType = GMMATYPE_SCALE;
						fScaleX = fScaleY = fNowLength/fLastLength;
					}
					int moveActionID = pgm->GetNextMoveActionID(nMoveType, nMoveAngle, fScaleX, fScaleY);
					for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it)
					{
						GObject * pObj = *it;
						if (comm == COMM_ROTATE_BATCH)
						{
							pObj->CallRotate(pObj, orix, oriy, nMoveAngle, true, moveActionID);
						}
						else if (fBeginLength > 0)
						{
							pObj->CallScale(pObj, orix, oriy, fScaleX, fScaleY, true, moveActionID);
						}
					}
				}

				lastAngle = angle;
				fLastLength = fNowLength;
			}
		}
	}
}

void RotateNodeBatchCommand::OnDoneCommand()
{
	float fVal;
	float orix, oriy;
	pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);
	fVal = pcommand->GetParamF(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN);
	int angle = fVal*ANGLEBASE_90/90;

	MathHelper::getInstance().RestrictAngle(&angle);

	if (comm == COMM_ROTATE_BATCH)
	{
		if (!angle)
		{
			pcommand->TerminalCommand();
			return;
		}
	}
	else
	{
		if (fabsf(fVal-1) < M_FLOATEPS)
		{
			pcommand->TerminalCommand();
			return;
		}
	}

	int index = pcommand->GetParamI(CSP_ROTATENODE_BATCH_I_INDEXES);
	list<GObject *> lobjs;
	int i=0;
	while (index >= 0)
	{
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		lobjs.push_back(pObj);
		i++;
		index = pcommand->GetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+i);
	}

	pgm->SetLockTreeChange();
	int nMoveType;
	float fScaleX=1.0f;
	float fScaleY=1.0f;
	int nMoveAngle=0;
	if (comm == COMM_ROTATE_BATCH)
	{
		nMoveType = GMMATYPE_ROTATE;
		nMoveAngle = angle;
	}
	else
	{
		nMoveType = GMMATYPE_SCALE;
		fScaleX = fScaleY = fVal;
	}
	int moveActionID = pgm->GetNextMoveActionID(nMoveType, nMoveAngle, fScaleX, fScaleY);
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		GObject * pMoveObj = *it;

		bool bRet = false;
		if (comm == COMM_ROTATE_BATCH)
		{
			bRet = (*it)->CallRotate(pMoveObj, orix, oriy, nMoveAngle, false, moveActionID);
		}
		else
		{
			bRet = (*it)->CallScale(pMoveObj, orix, oriy, fScaleX, fScaleY, false, moveActionID);
		}
		if (!bRet)
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
			CCMake_C(comm),
			CCMake_F(orix),
			CCMake_F(oriy),
			CCMake_F(fVal),
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
	ClearTemp();
}

void RotateNodeBatchCommand::OnProcessUnDoCommand( RevertableCommand * rc )
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
	int nMoveType;
	float fScaleX=1.0f;
	float fScaleY=1.0f;
	int nMoveAngle=0;
	if (comm == COMM_ROTATE_BATCH)
	{
		nMoveType = GMMATYPE_ROTATE;
		nMoveAngle = -angle;
	}
	else
	{
		nMoveType = GMMATYPE_SCALE;
		fScaleX = fScaleY = 1/fVal;
	}
	int moveActionID = pgm->GetNextMoveActionID(nMoveType, nMoveAngle, fScaleX, fScaleY);
	for (; it!=--(rc->commandlist.end()); ++it)
	{
		int index = it->ival;
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		if (comm == COMM_ROTATE_BATCH)
		{
			pObj->CallRotate(pObj, orix, oriy, nMoveAngle, false, moveActionID);
		}
		else
		{
			ASSERT(fVal!=0);
			pObj->CallScale(pObj, orix, oriy, fScaleX, fScaleY, false, moveActionID);
		}
		pgm->OnTreeChanged(pObj->getParent(), pObj);
	}
}

void RotateNodeBatchCommand::OnInitCommand()
{
	ClearTemp();
	pgm->BlockTryMove();
}

void RotateNodeBatchCommand::ClearTemp()
{
	bManualMode = false;
	lstObj.clear();
	lastAngle = 0;
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
	}
	fBeginLength = 0;
	fLastLength = 0;
	pgp->UnlockAngles();
	pgp->UnlockLength();
	pgm->UnblockTryMove();
}

void RotateNodeBatchCommand::OnClearCommand()
{
	ClearTemp();
}

void RotateNodeBatchCommand::OnTerminalCommand()
{
	ClearTemp();
}

bool RotateNodeBatchCommand::staticMIDCBAngle( MarkingUI * pmui, bool bAccept )
{
	return getInstance().MIDCBAngle(pmui, bAccept);
}

bool RotateNodeBatchCommand::MIDCBAngle( MarkingUI * pmui, bool bAccept )
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

bool RotateNodeBatchCommand::MIDCBLength( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedLength = pmui->getFloat(&bOk);
		if (bOk)
		{
			float x, y;
			pcommand->GetParamXY(CSP_LINE_XY_B, &x, &y);
			pgp->SetLockOrigin(x, y);
			pgp->SetLockLength(fLockedLength);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockLength();
	}
	return true;
}

bool RotateNodeBatchCommand::staticMIDCBLength( MarkingUI * pmui, bool bAccept )
{
	return getInstance().MIDCBLength(pmui, bAccept);
}

bool RotateNodeBatchCommand::FilterCallback( GObject * pObj )
{
	int step = pcommand->GetStep();
	if (step == CSI_ROTATENODE_BATCH_WANTANGLE )
	{
		return !MarqueeSelect::getInstance().CheckObjInSelection(pObj, true, true, true, true, &lstObj);
	}
	return true;
}

bool RotateNodeBatchCommand::staticFilterCallback( GObject * pObj )
{
	return getInstance().FilterCallback(pObj);
}