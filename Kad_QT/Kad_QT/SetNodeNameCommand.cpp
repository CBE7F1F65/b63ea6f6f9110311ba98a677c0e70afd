#include "StdAfx.h"
#include "SetNodeNameCommand.h"


SetNodeNameCommand::SetNodeNameCommand(void)
{
}


SetNodeNameCommand::~SetNodeNameCommand(void)
{
}

void SetNodeNameCommand::OnProcessCommand()
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
			CSI_SETNODENAME_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_SETNODENAME_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_SETNODENAME_I_S_INDEX_NEWNAME, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_SETNODENAME_WANTNEWNAME, CWP_NAME
			);
		if (ret < 0)
		{
            list <GObject *> * activenodelist = pgm->GetSelectedNodes(true);
            if (activenodelist->empty())
			{
				pcommand->StepTo(CSI_TERMINAL);
			}
			else
			{
                int index = activenodelist->front()->getID();
				pcommand->SetParamI(CSP_SETNODENAME_I_S_INDEX_NEWNAME, index, CWP_INDEX);
				pcommand->StepTo(CSI_SETNODENAME_WANTNEWNAME, CWP_NAME);
			}
		}
	}
	else if (step == CSI_SETNODENAME_WANTNEWNAME)
	{
		ret = pcommand->ProcessPending(
			CSP_SETNODENAME_I_S_INDEX_NEWNAME, COMMPARAMFLAG_S, CWP_NAME,	// Fill in Param
			CSI_FINISH					// Step to with want
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
}

void SetNodeNameCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_SETNODENAME_I_S_INDEX_NEWNAME);
	string newname = pcommand->GetParamS(CSP_SETNODENAME_I_S_INDEX_NEWNAME);
	if (!newname.length() || index < 0)
	{
		return;
	}

	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj)
	{
		return;
	}

	string oldname = pObj->getDisplayName();
	pObj->setDisplayName(newname.c_str());
	pgm->OnTreeChanged(pObj->getParent(), pObj);
//	MainInterface::getInstance().CallChangeNode(pObj);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_SETNODENAME),
		CCMake_I(index),
		CCMake_S(newname.c_str()),
		CCMake_C(COMM_I_UNDO_PARAM, 2),
		CCMake_I(index),
		CCMake_S(oldname.c_str()),
		NULL
		);

}

void SetNodeNameCommand::OnProcessUnDoCommand( RevertableCommand * rc ) 
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int index = it->ival;
	++it;
	string oldname = it->sval;
	GObject * pObj = pgm->FindObjectByID(index);
	ASSERT(pObj);
	pObj->setDisplayName(oldname.c_str());
	pgm->OnTreeChanged(pObj->getParent(), pObj);
//	MainInterface::getInstance().CallChangeNode(pObj);

}
