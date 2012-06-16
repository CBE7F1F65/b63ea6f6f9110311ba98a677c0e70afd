#include "stdafx.h"
#include "Command.h"

#include "GObject.h"
#include "GObjectManager.h"
#include "Main.h"
#include "CommandTemplate.h"
#include "SnapshotManager.h"


void Command::EnterUnDo()
{
	undoredoflag = CUNDOREDO_UNDOING;
	GObjectManager::getInstance().SetLockTreeChange();
}

void Command::EnterReDo()
{
	undoredoflag = CUNDOREDO_REDOING;
	GObjectManager::getInstance().SetLockTreeChange();
}

void Command::ExitUnDo()
{
	undoredoflag = CUNDOREDO_NULL;
}

void Command::ExitReDo()
{
	undoredoflag = CUNDOREDO_NULL;
}

bool Command::DoUnDo( int undostep/*=1*/ )
{
	if (undolist.size()<=1 || undostep < 1)
	{
		MainInterface::getInstance().MBeep();
		return false;
	}
	LogUnDo();

	SetRenderTarget(0);
	ClearCurrentCommand(true);

	RevertableCommand rc = undolist.back();

	// Dispatch
	EnterUnDo();
	for (list<CommittedCommand>::iterator it=rc.commandlist.begin(); it!=rc.commandlist.end(); ++it)
	{
		DASSERT(IsCCTypeCommand(it->type));
		DASSERT(IsInternalCommand(it->ival));
		int internalcommand = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (IsInternalCommand_Command(internalcommand))
		{
			ASSERT(pcount != 0);
			RevertableCommand rct;
			RevertableCommand rctundo;
			int command = COMM_NULL;
			++it;	//working layer
			for (int i=0; i<pcount; i++)
			{
				++it;
				if (!i)
				{
					DASSERT(IsCCTypeCommand(it->type));
					DASSERT(IsNormalCommand(it->ival));
					command = it->ival;
				}
				else
				{
					rct.PushCommand(&(*it));
				}
				DASSERT(it!=rc.commandlist.end());
			}
			for (int j=0; j<ucount; j++)
			{
				++it;
				DASSERT(IsCCTypeCommand(it->type));
				DASSERT(IsInternalCommand(it->ival));

				int usubcount = CI_GETPCOUNT(it->csub);
				

				int undocommand = it->ival;
				for (int i=0; i<usubcount; i++)
				{
					++it;
					DASSERT(it!=rc.commandlist.end());
					rctundo.PushCommand(&(*it));
				}
				if (undocommand == COMM_I_UNDO_COMMIT)
				{
					DoUnDoCommandCommit(&rctundo);
				}
				else if (undocommand == COMM_I_UNDO_PARAM)
				{
					ASSERT(command);
					DoUnDoCommandParam(command, &rctundo);
				}
				else if (undocommand == COMM_I_UNDO_PARAMFROMCOMMAND)
				{
					ASSERT(command);
					DoUnDoCommandParam(command, &rct);
				}
			}
		}
		else if (internalcommand == COMM_I_ADDNODE)
		{
			ASSERT(pcount == 2);
			++it;
			int objid = it->ival;
			++it;
			int objparentid = it->ival;
			DoUnDoAddNode(objid, objparentid);
		}
		else if (internalcommand == COMM_I_DELETENODE)
		{
			ASSERT(pcount == 2);
			++it;
			int objparentid = it->ival;
			++it;
			int objafterid = it->ival;
			DoUnDoDeleteNode(objparentid, objafterid);
		}
		else if (internalcommand == COMM_I_REPARENTNODE)
		{
			ASSERT(pcount == 3);
			++it;
			int objid = it->ival;
			++it;
			int oparentid = it->ival;
			++it;
			int afterid = it->ival;
			DoUnDoReparentNode(objid, oparentid, afterid);
		}
	}
	SnapshotManager::getInstance().OnUnDo();
	MainInterface::getInstance().OnUnDo();
	ExitUnDo();

	undolist.pop_back();
	redolist.push_back(rc);


	if (!GObjectManager::getInstance().GetActiveLayer())
	{
		GObjectManager::getInstance().SetActiveLayer_Internal();
	}

	if (undostep > 1)
	{
		DoUnDo(undostep-1);
	}

	return true;
}

bool Command::DoReDo( int redostep/*=1*/ )
{
	if (redolist.empty() || redostep < 1)
	{
		MainInterface::getInstance().MBeep();
		return false;
	}
	LogReDo();

	SetRenderTarget(0);
	ClearCurrentCommand(true);

	RevertableCommand rc = redolist.back();

	// Dispatch
	EnterReDo();
	for (list<CommittedCommand>::iterator it=rc.commandlist.begin(); it!=rc.commandlist.end(); ++it)
	{
		DASSERT(IsCCTypeCommand(it->type));
		DASSERT(IsInternalCommand(it->ival));
		int internalcommand = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (internalcommand == COMM_I_COMMAND)
		{
			ASSERT(pcount != 0);
			RevertableCommand rct;

			//
			++it;	//working layer
			DASSERT(IsCCTypeCommand(it->type));
			DASSERT(it->ival == COMM_I_COMM_WORKINGLAYER);
			GObject * pobj = GObjectManager::getInstance().FindObjectByID(it->csub);
			ASSERT(pobj);
			GObjectManager::getInstance().SetActiveLayer_Internal(pobj);
			//

			for (int i=0; i<pcount; i++)
			{
				++it;
				DASSERT(it!=rc.commandlist.end());
				rct.PushCommand(&(*it));
			}
			for (int j=0; j<ucount; j++)
			{
				++it;
//				DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
//				DASSERT(it->ival > _COMM_INTERNALBEGIN);

				int usubcount = CI_GETPCOUNT(it->csub);
//				DASSERT(usubcount != 0);

				int undocommand = it->ival;
				for (int i=0; i<usubcount; i++)
				{
					++it;
//					DASSERT(it!=rc.commandlist.end());
				}
			}
			DoReDoCommandSingle(&rct);
		}
		else if (internalcommand == COMM_I_ADDNODE)
		{
			ASSERT(pcount == 2);
			++it;
			int objid = it->ival;
			++it;
			int objparentid = it->ival;
			DoReDoAddNode(objid, objparentid);
		}
		else if (internalcommand == COMM_I_DELETENODE)
		{
			ASSERT(pcount == 2);
			++it;
			int objparentid = it->ival;
			++it;
			int objafterid = it->ival;
			DoReDoDeleteNode(objparentid, objafterid);
		}
		else if (internalcommand == COMM_I_REPARENTNODE)
		{
			ASSERT(pcount == 3);
			++it;
			int objid = it->ival;
			++it;
			int oparentid = it->ival;
			++it;
			int afterid = it->ival;
			DoReDoReparentNode(objid, oparentid, afterid);
		}
	}
	// Must process rcbuffer
	DoPushRevertable();

	SnapshotManager::getInstance().OnReDo();
	MainInterface::getInstance().OnReDo();
	// Clear redoflag when push revertable
//	undoredoflag = CUNDOREDO_NULL;

	redolist.pop_back();
	// No need to push back undolist
	// All ReDo should done by command
//	undolist.push_back(rc);
	CommandTemplate::CallOnUnDo();

	if (redostep > 1)
	{
		DoReDo(redostep-1);
	}

	return true;
}

bool Command::DoUnDoCommandCommit( RevertableCommand * rc )
{
	ProcessUnDoCommandCommit(rc);
	return true;
}

bool Command::DoUnDoCommandParam( int command, RevertableCommand * rc )
{
	ProcessUnDoCommandParam(command, rc);
	return true;
}

bool Command::DoReDoCommandSingle( RevertableCommand * rc )
{
	// Can only commit to front
	if (!rc)
	{
		return false;
	}

	for (list<CommittedCommand>::reverse_iterator it=rc->commandlist.rbegin(); it!=rc->commandlist.rend(); ++it)
	{
		CommitFrontCommand(*it);
	}
	//
	CommittedCommand tp = pendingparam;
	pendingparam.ClearSet();
	ProcessCommand();

	// To Finish last step
	while (!canCommandDone())
	{
		ProcessCommand();
	}
	ProcessCommand();

	pendingparam = tp;
	return true;
}

bool Command::DoUnDoAddNode( int objid, int objparentid )
{
	// Destroy Added
	GObject * obj = GObjectManager::getInstance().FindObjectByID(objid);
	GObject * parent = GObjectManager::getInstance().FindObjectByID(objparentid);
	ASSERT(obj != NULL);
	ASSERT(parent != NULL);
 
	DASSERT(obj->getParent() == parent);
	parent->RemoveChild(obj, true);
	return true;
}

bool Command::DoReDoAddNode( int objid, int objparentid )
{
	// No need to redo
	return true;
}

bool Command::DoUnDoDeleteNode( int objparentid, int objafterid )
{
	// move back from undobase
	GObject * obj = GObjectManager::getInstance().GetUnDoListFront();
	ASSERT(obj != NULL);
	GObject * objparent = GObjectManager::getInstance().FindObjectByID(objparentid);
	GObject * objafter = GObjectManager::getInstance().FindObjectByID(objafterid);
	obj->ReparentAfterObject(objparent, objafter);
	return true;
}

bool Command::DoReDoDeleteNode( int objparentid, int objafterid )
{
	/*
	// Put back to undobase
//	assert(obj != NULL);
	GObject * obj = GObjectManager::getInstance().FindObjectByID(objid);
	GObjectManager::getInstance().MoveToUnDoList(obj);
	*/
	// No need to redo
	return true;
}

bool Command::DoUnDoReparentNode( int objid, int oparentid, int afterid )
{
	// Swap parent

	GObject * obj = GObjectManager::getInstance().FindObjectByID(objid);
	if (oparentid >= 0)
	{
		GObject * objaparent = GObjectManager::getInstance().FindObjectByID(oparentid);
		GObject * objafter = GObjectManager::getInstance().FindObjectByID(afterid);
		obj->ReparentAfterObject(objaparent, objafter);
	}
	else
	{
		// ToDo!
		ASSERT(true);
	}

	return true;
}

bool Command::DoReDoReparentNode( int objid, int oparentid, int afterid )
{
	// Swap parent
//	assert(obj != NULL);
//	assert(oparent != NULL);
//	assert(aparent != NULL);

//	DASSERT(objid->getParent() == oparentid);
//	objid->Reparent(aparentid);
	// No need to redo
	return true;
}