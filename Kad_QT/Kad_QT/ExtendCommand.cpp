#include "StdAfx.h"
#include "ExtendCommand.h"
#include "RenderTargetManager.h"


ExtendCommand::ExtendCommand(void)
{
	pTempLine = NULL;
}


ExtendCommand::~ExtendCommand(void)
{
}

void ExtendCommand::OnProcessCommand()
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
			CSI_EXTEND_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_EXTEND_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_I_F_INDEX_BEGINT, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_EXTEND_WANTBEGINT, CWP_PROPORTION
			);
	}
	else if (step == CSI_EXTEND_WANTBEGINT)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_I_F_INDEX_BEGINT, COMMPARAMFLAG_F, CWP_PROPORTION,
			CSI_EXTEND_WANTENDT, CWP_PROPORTION
			);
	}
	else if (step == CSI_EXTEND_WANTENDT)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_F_ENDT, COMMPARAMFLAG_F, CWP_PROPORTION,
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
			int iret = pgp->PickPoint();
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					GObject * pObj = pgp->GetPickedObj();
					if (step == CSI_EXTEND_WANTINDEX)
					{
						if (pObj)
						{
							if (pObj->isMidPoint())
							{
								pObj = pObj->getLine();
							}
							if (pObj->isLine())
							{
								int index = pObj->getID();

								pTempLine = (GBezierLine *)pObj->CreateNewClone(&tBaseNode);
								pTempLine->Extend(-0.5f, 1.5f);
								pTempLine->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);

								pgp->SetLockLockLine(pTempLine);

								pcommand->SetParamI(CSP_EXTEND_I_F_INDEX_BEGINT, index, CWP_INDEX);
								pcommand->StepTo(CSI_EXTEND_WANTBEGINT, CWP_PROPORTION);
							}
						}
					}
					else if (step > CSI_EXTEND_WANTINDEX)
					{
						if (pObj == pTempLine)
						{
						}
					}
				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(COMM_EXTEND),
				NULL
				);
		}
	}
	RenderToTarget();
}

void ExtendCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_EXTEND_WANTBEGINT && nstep <= CSI_EXTEND_WANTENDT && pTempLine)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		pTempLine->CallRender();
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}

void ExtendCommand::OnDoneCommand()
{
	ClearTemp();
}

void ExtendCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
}

void ExtendCommand::OnInitCommand()
{
	ClearTemp();
}

void ExtendCommand::OnClearCommand()
{
	ClearTemp();
}

void ExtendCommand::OnTerminalCommand()
{
	ClearTemp();
}

void ExtendCommand::ClearTemp()
{
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
	}
	pgp->UnlockLockLine();
}