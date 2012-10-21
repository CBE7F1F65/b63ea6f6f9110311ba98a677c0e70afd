#include "StdAfx.h"
#include "ScaleNodeBatchCommand.h"

#include "MarkingManager.h"
#include "MarqueeSelect.h"


ScaleNodeBatchCommand::ScaleNodeBatchCommand(void)
{
	pTempLine = NULL;
}


ScaleNodeBatchCommand::~ScaleNodeBatchCommand(void)
{
}

void ScaleNodeBatchCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_SCALENODE_BATCH_WANTX, CWP_X);
	}
	else if (step == CSI_SCALENODE_BATCH_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_SCALENODE_BATCH_XY_ORIGIN, COMMPARAMFLAG_X, CWP_X,
			CSI_SCALENODE_BATCH_WANTY, CWP_Y
			);
	}
	else if (step == CSI_SCALENODE_BATCH_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_SCALENODE_BATCH_XY_ORIGIN, COMMPARAMFLAG_Y, CWP_Y,
			CSI_SCALENODE_BATCH_WANTXSCALE, CWP_XSCALE
			);
	}
	else if (step == CSI_SCALENODE_BATCH_WANTXSCALE)
	{
		ret = pcommand->ProcessPending(
			CSP_SCALENODE_BATCH_XY_SCALE, COMMPARAMFLAG_X, CWP_XSCALE,
			CSI_SCALENODE_BATCH_WANTYSCALE, CWP_YSCALE
			);
	}
	else if (step == CSI_SCALENODE_BATCH_WANTYSCALE)
	{
		ret = pcommand->ProcessPending(
			CSP_SCALENODE_BATCH_XY_SCALE, COMMPARAMFLAG_Y, CWP_YSCALE,
			CSI_SCALENODE_BATCH_WANTINDEXES, CWP_INDEX
			);
	}
	else if (step >= CSI_SCALENODE_BATCH_WANTINDEXES)
	{
		int nowindex = step - CSI_SCALENODE_BATCH_WANTINDEXES;
		if (bManualMode)
		{
			if (nowindex)
			{
				pcommand->SetParamI(CSP_SCALENODE_BATCH_I_INDEXES+nowindex, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int i=nowindex;
				for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it, i++)
				{
					pcommand->SetParamI(CSP_SCALENODE_BATCH_I_INDEXES+i, (*it)->getID(), CWP_INDEX);
				}
				pcommand->SetParamI(CSP_SCALENODE_BATCH_I_INDEXES+i, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else
		{
			ret = pcommand->ProcessPending(
				CSP_SCALENODE_BATCH_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
				CSI_SCALENODE_BATCH_WANTINDEXES+nowindex+1, CWP_INDEX
				);
			if (pcommand->GetParamI(CSP_SCALENODE_BATCH_I_INDEXES+nowindex) < 0)
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


		if (step >= CSI_SCALENODE_BATCH_WANTX && step < CSI_SCALENODE_BATCH_WANTINDEXES)
		{
			int iret = pgp->PickPoint(NULL, staticFilterCallback);
			if (step < CSI_SCALENODE_BATCH_WANTXSCALE)
			{
				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						pcommand->SetParamX(CSP_SCALENODE_BATCH_XY_ORIGIN, pgp->GetPickX_C());
						pcommand->SetParamY(CSP_SCALENODE_BATCH_XY_ORIGIN, pgp->GetPickY_C());
						pcommand->StepTo(CSI_SCALENODE_BATCH_WANTXSCALE, CWP_XSCALE);
					}
				}
			}
			else
			{
				float pickx = pgp->GetPickX_C();
				float picky = pgp->GetPickY_C();
				float orix, oriy;
				pcommand->GetParamXY(CSP_SCALENODE_BATCH_XY_ORIGIN, &orix, &oriy);
				if (!pTempLine)
				{
					pTempLine = new GBezierLine(&tBaseNode, PointF2D(orix, oriy), PointF2D(orix, oriy));
					pMarking = new MarkingOffset(pTempLine->GetEndPoint(), MARKFLAG_OFFSET);
					MarkingUI * pmuiXOffset = pMarking->getMarkingUI(MARKFLAG_XOFFSET);
					MarkingUI * pmuiYOffset = pMarking->getMarkingUI(MARKFLAG_YOFFSET);
					pmuiXOffset->SetEditable(true);
					pmuiYOffset->SetEditable(true);
					pmuiXOffset->SetCallback(staticMIDCOffset);
					pmuiYOffset->SetCallback(staticMIDCOffset);
					pMarking->SetMoveOrigin(orix, oriy);
					MarkingManager::getInstance().EnableMarking(pMarking);
				}
				pTempLine->SetBeginEnd(orix, oriy, pickx, picky);

				int angle = MathHelper::getInstance().GetLineAngle(pTempLine->GetBeginPoint()->GetPointF2D(), pTempLine->GetEndPoint()->GetPointF2D());

				float fNowXDiff = pickx-orix;
				float fNowYDiff = picky-oriy;
				if (pgp->IsMouseDownReady())
				{
					if (!bBeginSet)
					{
						fScaleBaseXDiff = pickx-orix;
						fScaleBaseYDiff = picky-oriy;
#define SCALECOMMAND_MINIMALSCALE		0.1f
#define SCALECOMMAND_MINIMALSCALEINV	(1.0f/SCALECOMMAND_MINIMALSCALE)
						if (fabsf(fScaleBaseXDiff) < SCALECOMMAND_MINIMALSCALEINV)
						{
							fScaleBaseXDiff = SCALECOMMAND_MINIMALSCALEINV;
						}
						if (fabsf(fScaleBaseYDiff) < SCALECOMMAND_MINIMALSCALEINV)
						{
							fScaleBaseYDiff = SCALECOMMAND_MINIMALSCALEINV;
						}
						fLastXDiff = fScaleBaseXDiff;
						fLastYDiff = fScaleBaseYDiff;
						bBeginSet = true;

					}
					pgp->SetCheckMouseDown();
					if (fabsf(fNowXDiff) < fabsf(fScaleBaseXDiff*SCALECOMMAND_MINIMALSCALE))
					{
						float fSign = 1.0f;
						if (fNowXDiff < 0)
						{
							fSign = -1.0f;
						}
						fNowXDiff = fSign * fabsf(fScaleBaseXDiff*SCALECOMMAND_MINIMALSCALE);
					}
					if (fabsf(fNowYDiff) < fabsf(fScaleBaseYDiff*SCALECOMMAND_MINIMALSCALE))
					{
						float fSign = 1.0f;
						if (fNowYDiff < 0)
						{
							fSign = -1.0f;
						}
						fNowYDiff = fSign * fabsf(fScaleBaseYDiff*SCALECOMMAND_MINIMALSCALE);
					}

					float fFinalScaleX = fNowXDiff/fScaleBaseXDiff;
					float fFinalScaleY = fNowYDiff/fScaleBaseYDiff;
					if (pmain->hge->Input_GetDIKey(DIK_LSHIFT) || pmain->hge->Input_GetDIKey(DIK_RSHIFT))
					{
						if (fabsf(fFinalScaleX) > fabsf(fFinalScaleY))
						{
							float fSign = 1.0f;
							if (fNowYDiff < 0)
							{
								fSign = -1.0f;
							}
							fNowYDiff = fSign*fabsf(fNowXDiff*fScaleBaseYDiff/fScaleBaseXDiff);

						}
						else
						{
							float fSign = 1.0f;
							if (fNowXDiff < 0)
							{
								fSign = -1.0f;
							}
							fNowXDiff = fSign*fabsf(fNowYDiff*fScaleBaseXDiff/fScaleBaseYDiff);
						}
					}

					fFinalScaleX = fNowXDiff/fScaleBaseXDiff;
					fFinalScaleY = fNowYDiff/fScaleBaseYDiff;

					if (pgp->IsPickReady(iret))
					{
						if (!pcommand->IsInternalProcessing())
						{
							pgm->EndTryMove();

							float fScaleX=1.0f;
							float fScaleY=1.0f;

							pcommand->SetParamX(CSP_SCALENODE_BATCH_XY_SCALE, fFinalScaleX, CWP_XSCALE);
							pcommand->SetParamY(CSP_SCALENODE_BATCH_XY_SCALE, fFinalScaleY, CWP_YSCALE);
							pcommand->StepTo(CSI_SCALENODE_BATCH_WANTINDEXES, CWP_INDEX);
						}
					}
					else
					{
						pgm->BeginTryMove();
						float fScaleX = fNowXDiff/fLastXDiff;
						float fScaleY = fNowYDiff/fLastYDiff;
						int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_SCALE, 0, fScaleX, fScaleY);
						for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it)
						{
							GObject * pObj = *it;
							pObj->CallScale(pObj, orix, oriy, fScaleX, fScaleY, true, moveActionID);
						}
					}

					fLastXDiff = fNowXDiff;
					fLastYDiff = fNowYDiff;
				}

				pMarking->SetNowPos(orix+fNowXDiff, oriy+fNowYDiff);
			}
		}
	}
}

void ScaleNodeBatchCommand::OnDoneCommand()
{
	float orix, oriy;
	pcommand->GetParamXY(CSP_SCALENODE_BATCH_XY_ORIGIN, &orix, &oriy);
	float fScaleX, fScaleY;
	pcommand->GetParamXY(CSP_SCALENODE_BATCH_XY_SCALE, &fScaleX, &fScaleY);

	if (fabsf(fScaleX-1) < M_FLOATEPS && fabsf(fScaleY-1)<M_FLOATEPS)
	{
		pcommand->TerminalCommand();
		return;
	}

	int index = pcommand->GetParamI(CSP_SCALENODE_BATCH_I_INDEXES);
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
		index = pcommand->GetParamI(CSP_SCALENODE_BATCH_I_INDEXES+i);
	}

	if (lobjs.empty())
	{
		Terminal();
		return;
	}

	pgm->SetLockTreeChange();

	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_SCALE, 0, fScaleX, fScaleY);
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		GObject * pMoveObj = *it;

		if (!(*it)->CallScale(pMoveObj, orix, oriy, fScaleX, fScaleY, false, moveActionID))
		{
			it = lobjs.erase(it);
		}
		else
		{
			pgm->OnTreeChanged(pMoveObj->getParent(), pMoveObj);
			++it;
		}
	}

	if (lobjs.empty())
	{
		Terminal();
	}
	else
	{
		int movedcount = lobjs.size();
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 6+movedcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_SCALENODE_BATCH),
			CCMake_F(orix),
			CCMake_F(oriy),
			CCMake_F(fScaleX),
			CCMake_F(fScaleY),
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
void ScaleNodeBatchCommand::OnProcessUnDoCommand( RevertibleCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	float orix = it->fval;
	++it;
	float oriy = it->fval;
	++it;
	float fScaleX = it->fval;
	++it;
	float fScaleY = it->fval;
	++it;
	pgm->SetLockTreeChange();

	ASSERT(fScaleX);
	ASSERT(fScaleY);

	fScaleX = 1/fScaleX;
	fScaleY = 1/fScaleY;
	int moveActionID = pgm->GetNextMoveActionID(GMMATYPE_SCALE, 0, fScaleX, fScaleY);
	for (; it!=--(rc->commandlist.end()); ++it)
	{
		int index = it->ival;
		GObject * pObj = pgm->FindObjectByID(index);
		ASSERT(pObj);
		pObj->CallScale(pObj, orix, oriy, fScaleX, fScaleY, false, moveActionID);
		pgm->OnTreeChanged(pObj->getParent(), pObj);
	}
}
*/
void ScaleNodeBatchCommand::OnInitCommand()
{
	pgm->BlockTryMove();
}

void ScaleNodeBatchCommand::OnClearTemp()
{
	bManualMode = false;
	lstObj.clear();
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
		pMarking = NULL;
	}
	bBeginSet = false;
	pgp->UnlockXAxis();
	pgp->UnlockYAxis();
	pgm->CancelTryMove();
}

bool ScaleNodeBatchCommand::FilterCallback( GObject * pObj )
{
	int step = pcommand->GetStep();
	if (step == CSI_SCALENODE_BATCH_WANTXSCALE || step == CSI_SCALENODE_BATCH_WANTYSCALE )
	{
		if (pObj->isHandlePoint())
		{
			return false;
		}
		return !MarqueeSelect::getInstance().CheckObjInSelection(pObj, true, true, true, true, &lstObj);
	}
	return true;
}

bool ScaleNodeBatchCommand::staticFilterCallback( GObject * pObj )
{
	return getInstance().FilterCallback(pObj);
}

bool ScaleNodeBatchCommand::staticMIDCOffset( MarkingUI * pmui, bool bAccept )
{
	return getInstance().MIDCBOffset(pmui, bAccept);
}

bool ScaleNodeBatchCommand::MIDCBOffset( MarkingUI * pmui, bool bAccept )
{
	bool bxoffset = true;
	float orix, oriy;
	pcommand->GetParamXY(CSP_SCALENODE_BATCH_XY_ORIGIN, &orix, &oriy);

	if (pmui->getTypeFlag() == MARKFLAG_YOFFSET)
	{
		bxoffset = false;
	}

	if (pmui->IsValueLocked())
	{
		bool bOk;

		QString str = pmui->getStr();
		float fLockedVal;
		if (str.endsWith("%"))
		{
			string s = str.toStdString();
			float fPercentage;
			sscanf_s(s.c_str(), "%f%%", &fPercentage);
			fPercentage/=100.0f;
			bOk = true;
			if (bxoffset)
			{
				fLockedVal = fPercentage*fScaleBaseXDiff;
			}
			else
			{
				fLockedVal = fPercentage*fScaleBaseYDiff;
			}

		}
		else
		{
			fLockedVal = str.toFloat(&bOk);
		}

		if (bOk)
		{
			if (bxoffset)
			{
				pgp->SetLockYAxis(fLockedVal+orix);
			}
			else
			{
				pgp->SetLockXAxis(fLockedVal+oriy);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (bxoffset)
		{
			pgp->UnlockYAxis();
		}
		else
		{
			pgp->UnlockXAxis();
		}
	}
	return true;
}