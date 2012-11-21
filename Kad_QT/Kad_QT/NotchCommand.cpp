#include "StdAfx.h"
#include "NotchCommand.h"


NotchCommand::NotchCommand(void)
{
}


NotchCommand::~NotchCommand(void)
{
}

void NotchCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_NOTCH_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_NOTCH_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_NOTCH_I_F_INDEX_PROPORTION, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_NOTCH_WANTPROPORTION, CWP_PROPORTION
			);
	}
	else if (step == CSI_NOTCH_WANTPROPORTION)
	{
		ret = pcommand->ProcessPending(
			CSP_NOTCH_I_F_INDEX_PROPORTION, COMMPARAMFLAG_F, CWP_PROPORTION,
			CSI_FINISH
			//			CSI_FINISHCONTINUE
			);
	}

	if (ret < 0)
	{
		// Routine
		if (step > CSI_INIT)
		{
			int iret = pgp->PickPoint(staticPickFilterFunc, staticPickFilterFunc);
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					GObject * pObj = pgp->GetPickedObj();
					if (pObj)
					{
						float fProportion = pgp->CalculateProportion();
						if (pObj->isMidPoint())
						{
							pObj = pObj->getLine();
						}
						if (pObj->isAnchorPoint() && (fProportion == 0.0f || fProportion == 1.0f))
						{
							pObj = pObj->getLine();
						}
						if (pObj->isPoint())
						{
							GPoint * pPoint = (GPoint *)pObj;
							pObj = pPoint->getClingInfo()->GetClingTo();
						}
						if (!pObj)
						{
							Terminal();
							return;
						}
						int index = pObj->getID();

						pcommand->SetParamI(CSP_NOTCH_I_F_INDEX_PROPORTION, index, CWP_INDEX);
						pcommand->SetParamF(CSP_NOTCH_I_F_INDEX_PROPORTION, fProportion, CWP_PROPORTION);
						pcommand->StepTo(CSI_FINISH);
						//						pcommand->StepTo(CSI_FINISHCONTINUE);
					}
				}
			}
		}
	}

}

void NotchCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_NOTCH_I_F_INDEX_PROPORTION);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj || !pObj->isLine())
	{
		Terminal();
		return;
	}
	float fProporation = pcommand->GetParamF(CSP_NOTCH_I_F_INDEX_PROPORTION);
	if (fProporation < 0 || fProporation > 1)
	{
		Terminal();
		return;
	}

	GLine * pLine = (GLine *)pObj;
	GClingInfo clInfo;
	clInfo.SetClingTo(pLine, fProporation, 0);
	GNotch * pNotch = new GNotch(pLine->getLayer(), &clInfo);

	if (pNotch)
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 3),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_NOTCH),
			CCMake_I(index),
			CCMake_F(fProporation),
			NULL
			);		
	}
}

bool NotchCommand::staticPickFilterFunc( GObject * pObj )
{
	return getInstance().PickFilterFunc(pObj);
}

bool NotchCommand::PickFilterFunc( GObject * pObj )
{
	if (pObj->isPoint() && pObj->canAttach())
	{
		GPoint * pPoint = (GPoint *)pObj;
		if (pPoint->getClingInfo()->GetClingTo())
		{
			return false;
		}
	}
	if (pObj->isLine() || pObj->isMidPoint() || pObj->isAnchorPoint())
	{
		return true;
	}
	return false;
}