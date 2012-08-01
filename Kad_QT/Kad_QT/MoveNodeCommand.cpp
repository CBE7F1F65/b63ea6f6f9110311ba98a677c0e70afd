#include "StdAfx.h"
#include "MoveNodeCommand.h"


MoveNodeCommand::MoveNodeCommand(void)
{
}


MoveNodeCommand::~MoveNodeCommand(void)
{
}

void MoveNodeCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;
	int cwpx = (comm == COMM_MOVENODE)?CWP_X:CWP_XOFFSET;
	int cwpy = (comm == COMM_MOVENODE)?CWP_Y:CWP_YOFFSET;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_MOVENODE_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_MOVENODE_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_I_XY_INDEX_NEWPOS, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_MOVENODE_WANTX, cwpx
			);
	}
	else if (step == CSI_MOVENODE_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_I_XY_INDEX_NEWPOS, COMMPARAMFLAG_X, cwpx,
			CSI_MOVENODE_WANTY, cwpy
			);
	}
	else if (step == CSI_MOVENODE_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_MOVENODE_I_XY_INDEX_NEWPOS, COMMPARAMFLAG_Y, cwpy,
			CSI_FINISH
			);
	}

	if (ret < 0)
	{
		if (step >= CSI_MOVENODE_WANTX && comm == COMM_MOVENODE)
		{
			int iret = pgp->PickPoint();
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					pcommand->SetParamX(CSP_MOVENODE_I_XY_INDEX_NEWPOS, pgp->GetPickX_C());
					pcommand->SetParamY(CSP_MOVENODE_I_XY_INDEX_NEWPOS, pgp->GetPickY_C());
					pcommand->StepTo(CSI_FINISH);
				}
			}
		}
	}
}

void MoveNodeCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_MOVENODE_I_XY_INDEX_NEWPOS);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj)
	{
		Terminal();
		return;
	}
	float newx, newy;
	pcommand->GetParamXY(CSP_MOVENODE_I_XY_INDEX_NEWPOS, &newx, &newy);

	float tox = newx;
	float toy = newy;
	float oldx = pObj->getX();
	float oldy = pObj->getY();
	if (comm == COMM_MOVENODEBYOFFSET)
	{
		tox += oldx;
		toy += oldy;
	}

	PointF2D p(tox, tox);
	if (p.Equals(PointF2D(oldx, oldy)))
	{
		Terminal();
		return;
	}
	
	if (pObj->CallMoveTo(pObj, tox, toy, false))
	{
		pgm->OnTreeChanged(pObj->getParent(), pObj);
//		MainInterface::getInstance().CallChangeNode(pObj);

		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 4),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(comm),
			CCMake_I(index),
			CCMake_F(newx),
			CCMake_F(newy),
			NULL
			);
		ReAttachAfterMoveNode(pObj, true);
	}
}
/*
void MoveNodeCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int index = it->ival;
	++it;
	float oldx = it->fval;
	++it;
	float oldy = it->fval;
	GObject * pObj = pgm->FindObjectByID(index);
	ASSERT(pObj);
	pObj->CallMoveTo(pObj, oldx, oldy, false);
	pgm->OnTreeChanged(pObj->getParent(), pObj);
//	MainInterface::getInstance().CallChangeNode(pObj);
}
*/