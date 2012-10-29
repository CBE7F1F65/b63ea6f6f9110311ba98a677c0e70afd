#include "StdAfx.h"
#include "RotateNodeBatchCommand.h"
#include "MarkingManager.h"
#include "MarqueeSelect.h"


RotateNodeBatchCommand::RotateNodeBatchCommand(void)
{
	pTempLine = NULL;
	pMarking = NULL;
}


RotateNodeBatchCommand::~RotateNodeBatchCommand(void)
{
}

void RotateNodeBatchCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
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

	if (ret < 0)
	{
		if (step > CSI_INIT)
		{
			if (!bManualMode)
			{
				lstObj = *(pgm->GetSelectedNodes(false));
				if (lstObj.empty())
				{
					Terminal();
					return;
				}
				lstObj.sort();
			}
			bManualMode = true;
		}


		if (step >= CSI_ROTATENODE_BATCH_WANTX && step < CSI_ROTATENODE_BATCH_WANTINDEXES)
		{
			int iret = pgp->PickPoint(NULL, staticFilterCallback);
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

					pgp->UnlockAngles();
					pgp->UnlockLength();
				}
				pgp->SetCheckMouseDown();
				float orix, oriy;
				pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);

				float pickx = pgp->GetPickX_C();
				float picky = pgp->GetPickY_C();
				pTempLine->SetBeginEnd(orix, oriy, pickx, picky);

				if (pgp->IsMouseDownReady())
				{
					int angle = MathHelper::getInstance().GetLineAngle(pTempLine->GetBeginPoint()->GetPointF2D(), pTempLine->GetEndPoint()->GetPointF2D());
					if (!pMarking)
					{
						pMarking = new MarkingLine(pTempLine, MARKFLAG_ANGLE|MARKFLAG_LENGTH);
						pMarking->SetRelativeAngle(angle);
						relativeAngle = angle;
						MarkingUI * pmuiAngle = pMarking->getMarkingUI(MARKFLAG_ANGLE);
						MarkingUI * pmuiLength = pMarking->getMarkingUI(MARKFLAG_LENGTH);
						pmuiAngle->SetEditable(true);
						pmuiAngle->SetCallback(staticMIDCBAngle);
						pmuiLength->SetEditable(true);
						pmuiLength->SetCallback(staticMIDCBLength);
						MarkingManager::getInstance().EnableMarking(pMarking);
					}

					angle -= relativeAngle;
					float fAngle = angle*90.0f/ANGLEBASE_90;

					MarkingUI * pmuiLength = pMarking->getMarkingUI(MARKFLAG_LENGTH);
					if (!pmuiLength->IsValueLocked())
					{
						if (pmain->hge->Input_GetDIKey(DIK_LSHIFT) || pmain->hge->Input_GetDIKey(DIK_RSHIFT))
						{
							float fLength = MathHelper::getInstance().LineSegmentLength(PointF2D(orix, oriy), PointF2D(pgp->GetDownX_C(), pgp->GetDownY_C()));
//							pTempLine->SetBeginEnd(orix, oriy, orix+fLength*cost(angle+relativeAngle), oriy+fLength*sint(angle+relativeAngle));
							pgp->SetLockOrigin(orix, oriy);
							pgp->SetLockLength(fLength);
						}
					}

					if (pgp->IsPickReady(iret))
					{
						if (!pcommand->IsInternalProcessing())
						{
							pgm->EndTryMove();
							pcommand->SetParamF(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, fAngle, CWP_ANGLE);
							pcommand->StepTo(CSI_ROTATENODE_BATCH_WANTINDEXES, CWP_INDEX);
						}
					}
					else
					{
						pgm->BeginTryMove();
						int nMovedAngle = angle-lastAngle;
						int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_ROTATE, nMovedAngle);
						for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it)
						{
							GObject * pObj = *it;
							pObj->CallRotate(pObj, orix, oriy, nMovedAngle, true, moveActionID);
						}
					}

					lastAngle = angle;
				}
			}
		}
	}
}

void RotateNodeBatchCommand::OnDoneCommand()
{
	float orix, oriy;
	pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &orix, &oriy);
	float fAngle = pcommand->GetParamF(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN);
	int angle = fAngle*ANGLEBASE_90/90;

	MathHelper::getInstance().RestrictAngle(&angle);

	if (!angle)
	{
		Terminal();
		return;
	}

	int index = pcommand->GetParamI(CSP_ROTATENODE_BATCH_I_INDEXES);
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
		index = pcommand->GetParamI(CSP_ROTATENODE_BATCH_I_INDEXES+i);
	}

	if (lobjs.empty())
	{
		Terminal();
		return;
	}

	pgm->SetLockTreeChange();
	pgm->SetWillSelfMoveList(&lobjs);
	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_ROTATE, angle);
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		GObject * pMoveObj = *it;

		if (!(*it)->CallRotate(pMoveObj, orix, oriy, angle, false, moveActionID))
		{
			it = lobjs.erase(it);
		}
		else
		{
			pgm->OnTreeChanged(pMoveObj->getParent(), pMoveObj);
			++it;
		}
	}
	pgm->SetWillSelfMoveList();

	if (lobjs.empty())
	{
		Terminal();
	}
	else
	{
		int movedcount = lobjs.size();
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 5+movedcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_ROTATENODE_BATCH),
			CCMake_F(orix),
			CCMake_F(oriy),
			CCMake_F(fAngle),
			NULL
			);
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			PushRevertibleBatch(
				PUSHREVERTABLESTATE_CONTINUE,
				CCMake_I((*it)->getID()),
				NULL
				);
		}
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_END,
			CCMake_I(-1),
			CCMake_C(COMM_I_UNDO_PARAMFROMCOMMAND),
			NULL
			);
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			ReAttachAfterMoveNode(*it, true, &lobjs);
		}

	}
}
/*
void RotateNodeBatchCommand::OnProcessUnDoCommand( RevertibleCommand * rc )
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

	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_ROTATE, -angle);
	for (; it!=--(rc->commandlist.end()); ++it)
	{
		int index = it->ival;
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		pObj->CallRotate(pObj, orix, oriy, -angle, false, moveActionID);
		pgm->OnTreeChanged(pObj->getParent(), pObj);
	}
}
*/
void RotateNodeBatchCommand::OnInitCommand()
{
	pgm->BlockTryMove();
}

void RotateNodeBatchCommand::OnClearTemp()
{
	bManualMode = false;
	lstObj.clear();
	lastAngle = 0;
	relativeAngle = 0;
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
		pMarking = NULL;
	}
	pgp->UnlockAngles();
	pgp->UnlockLength();
	pgm->CancelTryMove();
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

bool RotateNodeBatchCommand::FilterCallback( GObject * pObj )
{
	int step = pcommand->GetStep();
	if (step == CSI_ROTATENODE_BATCH_WANTANGLE )
	{
		if (pObj->isHandlePoint())
		{
			return false;
		}
		return !MarqueeSelect::getInstance().CheckObjInSelection(pObj, true, true, true, true, &lstObj);
	}
	return true;
}

bool RotateNodeBatchCommand::staticFilterCallback( GObject * pObj )
{
	return getInstance().FilterCallback(pObj);
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
			pcommand->GetParamXY(CSP_ROTATENODE_BATCH_F_XY_ANGLE_ORIGIN, &x, &y);
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