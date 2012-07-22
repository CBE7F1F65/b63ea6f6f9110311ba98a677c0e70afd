#include "StdAfx.h"
#include "ClipCommand.h"


ClipCommand::ClipCommand(void)
{
}


ClipCommand::~ClipCommand(void)
{
}

void ClipCommand::OnProcessCommand()
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
			CSI_CLIP_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_CLIP_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_F_TOINDEX_PROPORTION, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_CLIP_WANTPROPORTION
			);
	}
	else if (step == CSI_CLIP_WANTPROPORTION)
	{
		ret = pcommand->ProcessPending(
			CSP_CLIP_I_F_INDEX_PROPORTION, COMMPARAMFLAG_F, CWP_PROPORTION,
			CSI_FINISH
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
	else if (ret < 0)
	{
		// Routine
		if (step > CSI_INIT)
		{
			int iret = pgp->PickPoint(staticPickFilterFunc);
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					GObject * pObj = pgp->GetPickedObj();
					float fProportion = pgp->CalculateProportion();
					if (pObj->isMidPoint())
					{
						pObj = pObj->getLine();
					}
					int index = pObj->getID();

					pcommand->SetParamI(CSP_CLIP_I_F_INDEX_PROPORTION, index, CWP_INDEX);
					pcommand->SetParamF(CSP_CLIP_I_F_INDEX_PROPORTION, fProportion, CWP_PROPORTION);
					pcommand->StepTo(CSI_FINISH);
				}
			}
		}
	}

}

void ClipCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_CLIP_I_F_INDEX_PROPORTION);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj->isLine())
	{
		pcommand->TerminalCommand();
		return;
	}
	float fProporation = pcommand->GetParamF(CSP_CLIP_I_F_INDEX_PROPORTION);
	if (fProporation <= 0 || fProporation >= 1)
	{
		pcommand->TerminalCommand();
		return;
	}

	GLine * pLine = (GLine *)pObj;
	GLine * pClonedLine = (GLine *)pLine->CreateNewClone(NULL, pLine);

	GAnchorPoint * pBeginPoint = pLine->GetBeginPoint();
	GAnchorPoint * pEndPoint = pLine->GetEndPoint();

	GHandlePoint * pOEndHandleBind = pEndPoint->GetHandle()->getBindWith();

	PointF2D ptBegin = pBeginPoint->GetPointF2D();
	PointF2D ptEnd = pEndPoint->GetPointF2D();

	GLine * pOEndClingTo = pEndPoint->getClingTo();
	float fOEndClingProp = pEndPoint->getClingProportion();

	list<GPoint *> lstEndMergeWith = *(pEndPoint->getMergeWith());
	list<GPoint *> lstClingBy = *(pLine->getClingBy());
	list<float> lstFCP;
	for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it)
	{
		lstFCP.push_back((*it)->getClingProportion());
	}

	pEndPoint->ClingTo(NULL, 0);
	pEndPoint->SeperateFrom();
	pLine->DeclingByOther();

	float tx, ty;
	int isec;
	pLine->GetPositionAtProportion(fProporation, &tx, &ty, &isec);

	pLine->SetBeginEnd(ptBegin.x, ptBegin.y, tx, ty);
	pClonedLine->SetBeginEnd(tx, ty, ptEnd.x, ptEnd.y);

	GAnchorPoint * pNewBeginPoint = pClonedLine->GetBeginPoint();
	GAnchorPoint * pNewEndPoint = pClonedLine->GetEndPoint();

	pNewEndPoint->GetHandle()->BindWith(pOEndHandleBind);

	pNewEndPoint->ClingTo(pOEndClingTo, fOEndClingProp);
	if (!lstEndMergeWith.empty())
	{
		pNewEndPoint->MergeWith(lstEndMergeWith.front());
	}
	
	pEndPoint->MergeWith(pNewBeginPoint);
	if (!pLine->isStraightLine())
	{
		pEndPoint->GetHandle()->BindWith(pNewBeginPoint->GetHandle());
	}

	list<float>::iterator jt=lstFCP.begin();
	for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it, ++jt)
	{
		GPoint * pClingByPoint = *it;
		float fCP = *jt;
		if (fCP < fProporation)
		{
			float fNewProp = fCP/fProporation;
			ASSERT(fNewProp > 0 && fNewProp <= 1);
			pClingByPoint->ClingTo(pLine, fNewProp);
		}
		else if (fCP > fProporation)
		{
			float fNewProp = (fCP-fProporation)/fProporation;
			ASSERT(fNewProp > 0 && fNewProp <= 1);
			pClingByPoint->ClingTo(pClonedLine, fNewProp);
		}
		else
		{
			pClingByPoint->MergeWith(pEndPoint);
		}
	}
}

void ClipCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
}

bool ClipCommand::staticPickFilterFunc( GObject * pObj )
{
	return getInstance().PickFilterFunc(pObj);
}

bool ClipCommand::PickFilterFunc( GObject * pObj )
{
	if (pObj->isLine() || pObj->isMidPoint())
	{
		return true;
	}
	return false;
}