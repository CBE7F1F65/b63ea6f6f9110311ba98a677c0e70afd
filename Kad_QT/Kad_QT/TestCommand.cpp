#include "StdAfx.h"
#include "TestCommand.h"

#ifdef DEBUGTEST

#include "GUICursor.h"
#include "RenderTargetManager.h"
#include "CommandSubState.h"


TestCommand::TestCommand(void)
{
}


TestCommand::~TestCommand(void)
{
	cmap.Clear();
}

void TestCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand(GUIC_CREATEPOINT);
	int nowstep = pcommand->GetStep();
	cmap.ExecuteMap(step, nowstep);
	/*
	if (IsStepped())
	{
		if (nowstep > CSI_INIT)
		{
			pcommand->EnableSubCommand(
				(laststep.step==CSI_RESUME)?false:true,
				SSC_UNDO,
				SSC_REDO,
				SSC_TERMINAL,
				SSC_NULL);
		}
	}
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_LINE_WANTX1,
			CWP_XY_B);
	}
	else if (step == CSI_LINE_WANTX1)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_B, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_LINE_WANTY1, CWP_Y_B					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY1)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_B, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_LINE_WANTX2, CWP_XY_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTX2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_N, COMMPARAMFLAG_X, CWP_X_N,	// Fill in Param
			CSI_LINE_WANTY2, CWP_Y_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_N, COMMPARAMFLAG_Y, CWP_Y_N,	// Fill in Param
			CSI_FINISHCONTINUE							// Step to with want
			);
	}

	if (step >= CSI_LINE_WANTX2)
	{
		float bx, by;
		pcommand->GetParamXY(CSP_LINE_XY_B, &bx, &by);
		pgp->PushInterestPoint(bx, by);
	}

	if (!ret)
	{
	}
	else if (ret > 0)
	{
		// Dispatch Subcommand
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
					int tosetpindex=-1;
					switch (step)
					{
					case CSI_LINE_WANTX1:
					case CSI_LINE_WANTY1:
						tosetpindex = CSP_LINE_XY_B;
						break;
					case CSI_LINE_WANTX2:
					case CSI_LINE_WANTY2:
						tosetpindex = CSP_LINE_XY_N;
						break;
					}
					pcommand->SetParamX(tosetpindex, pgp->GetPickX_C(), CWP_X);
					pcommand->SetParamY(tosetpindex, pgp->GetPickY_C(), CWP_Y);
					if (step < CSI_LINE_WANTX2)
					{
						pcommand->StepTo(
							CSI_LINE_WANTX2, CWP_XY_N
							);
					}
					else
					{
						pcommand->StepTo(
							CSI_FINISHCONTINUE
							);
					}

				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			float nx1, ny1;
			pcommand->GetParamXY(CSP_LINE_XY_N, &nx1, &ny1);

			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(comm),
				CCMake_F(nx1),
				CCMake_F(ny1),
				NULL
				);
		}
	}
	*/
	RenderToTarget();
}

void TestCommand::OnDoneCommand()
{
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_XY_B, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_XY_N, &xe, &ye);
	GStraightLine * pLine = new GBezierLine(pgm->GetActiveLayer(), PointF2D(xb, yb), PointF2D(xe, ye));

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_I(pLine->getID()),
		CCMake_I(pLine->getParent()->getID()),
		CCMake_C(COMM_I_COMMAND, 5, 1),
		CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(comm),
		CCMake_F(xb),
		CCMake_F(yb),
		CCMake_F(xe),
		CCMake_F(ye),
		CCMake_C(COMM_I_UNDO_COMMIT, 3),
		CCMake_C(comm),
		CCMake_F(xb),
		CCMake_F(yb),
		NULL
		);

	if (pcommand->GetStep() == CSI_FINISHCONTINUE)
	{
//		ProtectPendingFinishCommand();

		CommitFrontCommand(
			CCMake_C(comm),
			CCMake_F(xe),
			CCMake_F(ye),
			NULL
			);
	}
}

void TestCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_LINE_WANTX2 && nstep <= CSI_LINE_WANTY2)
	{
		float x1, y1;
		pcommand->GetParamXY(CSP_LINE_XY_B, &x1, &y1);

		float x2 = pgp->GetPickX_C();//MainInterface::getInstance().mousex;
		float y2 = pgp->GetPickY_C();//MainInterface::getInstance().mousey;

		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		RenderHelper::getInstance().BeginRenderTar(tar);
		RenderHelper::getInstance().RenderLineMeasureMark(x1, y1, x2, y2, pgm->GetActiveLayer()->getLineColor());
		RenderHelper::getInstance().RenderLine(x1, y1, x2, y2, pgm->GetActiveLayer()->getLineColor());
		RenderHelper::getInstance().EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);
	}
}

void TestCommand::OnInitCommand()
{
	cmap.Clear();
	cmap.SetCommandTemplate(this);

	FillCommandParamInfo fcpInit(CSI_INIT, -1, -1, -1, CSI_LINE_WANTX1, CWP_X_B);
	FillCommandParamInfo fcpX1(
		CSI_LINE_WANTX1,
		CSP_LINE_XY_B, COMMPARAMFLAG_X, CWP_X_B,
		CSI_LINE_WANTY1, CWP_Y_B);
	FillCommandParamInfo fcpY1(
		CSI_LINE_WANTY1,
		CSP_LINE_XY_B, COMMPARAMFLAG_Y, CWP_Y_B,
		CSI_LINE_WANTX2, CWP_X_N);
	FillCommandParamInfo fcpX2(
		CSI_LINE_WANTX2,
		CSP_LINE_XY_N, COMMPARAMFLAG_X, CWP_X_N,
		CSI_LINE_WANTY2, CWP_Y_N);
	FillCommandParamInfo fcpY2(
		CSI_LINE_WANTY2,
		CSP_LINE_XY_N, COMMPARAMFLAG_Y, CWP_Y_N,
		CSI_FINISHCONTINUE);

	CommandState * pInitState = new CommandState(&fcpInit);
	CommandState * pWantX1State = new CommandState(&fcpX1);
	CommandState * pWantY1State = new CommandState(&fcpY1);
	CommandState * pWantX2State = new CommandState(&fcpX2);
	CommandState * pWantY2State = new CommandState(&fcpY2);
	
	pWantX1State->AddSubState(new CSS_PickVoid(&fcpX2, &fcpX1, &fcpY1));
	pWantY1State->AddSubState(new CSS_PickVoid(&fcpX2, &fcpX1, &fcpY1));
	pWantX2State->AddSubState(new CSS_PickVoid(&fcpY2, &fcpX2, &fcpY2));
	pWantY2State->AddSubState(new CSS_PickVoid(&fcpY2, &fcpX2, &fcpY2));

	cmap.AddState(pInitState);
	cmap.AddState(pWantX1State);
	cmap.AddState(pWantY1State);
	cmap.AddState(pWantX2State);
	cmap.AddState(pWantY2State);

}

void TestCommand::OnTerminalCommand()
{
	cmap.Clear();
}
#endif // DEBUGTEST