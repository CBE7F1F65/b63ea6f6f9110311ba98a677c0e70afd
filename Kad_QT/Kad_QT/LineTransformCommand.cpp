#include "StdAfx.h"
#include "LineTransformCommand.h"


LineTransformCommand::LineTransformCommand(void)
{
}


LineTransformCommand::~LineTransformCommand(void)
{
}

void LineTransformCommand::OnProcessCommand()
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
			CSI_LINETRANSFORM_WANTNEWINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_LINETRANSFORM_WANTNEWINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_LINETRANSFORM_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
}

void LineTransformCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_LINETRANSFORM_I_INDEX);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj || !pObj->isLine())
	{
		Terminal();
		return;
	}
	GLine * pLine = (GLine *)pObj;
	bool bret = false;
	switch (comm)
	{
	case COMM_TOBEZIER:
		
		if (pLine->toBezierLine())
		{
			PushRevertable(
				CCMake_C(COMM_I_COMMAND, 2),
				CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
				CCMake_C(COMM_TOBEZIER),
				CCMake_I(index),
				NULL
				);
		}
		break;
	case COMM_TOSTRAIGHTLINE:
		{
			if (!pLine->isStraightLine())
			{
				GBezierLine * pBezier = (GBezierLine *)pLine;

				if (pBezier->toStraightLine())
				{
					PushRevertable(
						CCMake_C(COMM_I_COMMAND, 2),
						CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
						CCMake_C(COMM_TOSTRAIGHTLINE),
						CCMake_I(index),
						NULL
						);
				}
			}
		}
		break;
	}

}
/*
void LineTransformCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int index = it->ival;
	GObject * pObj = pgm->FindObjectByID(index);
	ASSERT(pObj);
	GLine * pLine = (GLine *)pObj;
	switch (comm)
	{
	case COMM_TOBEZIER:
		pLine->toStraightLine();
		break;
	case COMM_TOSTRAIGHTLINE:
		{
			pLine->toBezierLine();

			++it;
			float bhx = it->fval;
			++it;
			float bhy = it->fval;
			++it;
			float ehx = it->fval;
			++it;
			float ehy = it->fval;
			++it;
			float bbindindex = it->ival;
			++it;
			float ebindindex = it->ival;

			GBezierLine * pBezier = (GBezierLine *)pLine;
			pBezier->SetBeginHandlePos(bhx, bhy);
			pBezier->SetEndHandlePos(ehx, ehy);
			if (bbindindex >= 0)
			{
				pBezier->GetBeginPoint()->GetHandle()->BindWith((GHandlePoint *)pgm->FindObjectByID(bbindindex));
			}
			if (ebindindex >= 0)
			{
				pBezier->GetEndPoint()->GetHandle()->BindWith((GHandlePoint *)pgm->FindObjectByID(ebindindex));
			}
		}
		break;
	}
}
*/