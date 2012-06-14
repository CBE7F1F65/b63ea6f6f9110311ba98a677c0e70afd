#include "StdAfx.h"
#include "BezierCommand.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "MathHelper.h"
#include "GUICoordinate.h"
#include "ColorManager.h"
#include "Command.h"
#include "GObjectManager.h"

BezierCommand::BezierCommand(void)
{
}

BezierCommand::~BezierCommand(void)
{
}

void BezierCommand::OnProcessCommand()
{
	int step = pcommand->GetStep();
	if (step == CSI_PAUSE || step == CSI_TERMINAL)
	{
		ReleaseTarget();
		if (step == CSI_TERMINAL)
		{
			pcommand->TerminalCommand();
		}
		return;
	}
	RenderToTarget();
}

void BezierCommand::OnDoneCommand()
{
}

void BezierCommand::RenderToTarget()
{
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

	RenderHelper::getInstance().BeginRenderTar(tar);
	PointF2D p1(50, 100);
	PointF2D p2(150, 150);
	PointF2D p4(400, 100);

	float x4 = MainInterface::getInstance().GetMouseX_S();
	float y4 = MainInterface::getInstance().GetMouseY_S();
	x4 = GUICoordinate::getInstance().StoCx(x4);
	y4 = GUICoordinate::getInstance().StoCy(y4);
	PointF2D p3(x4, y4);

	RenderHelper::getInstance().RenderBezier(p1, p2, p3, p4, pgm->GetActiveLayer()->getLineColor(), pmain->GetPrecision());
	RenderHelper::getInstance().EndRenderTar();

	Command::getInstance().SetRenderTarget(tar);
}