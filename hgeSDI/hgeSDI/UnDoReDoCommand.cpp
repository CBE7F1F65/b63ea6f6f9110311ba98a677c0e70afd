#include "stdafx.h"
#include "Command.h"

#include "GObject.h"
#include "GObjectManager.h"
#include "Main.h"
#include "CommandTemplate.h"
bool Command::DoUnDo( int undostep/*=1*/ )
{
	if (undolist.size()<=1 || undostep < 1)
	{
		MainInterface::getInstance().MBeep();
		return false;
	}
	LogUnDo();
	if (undostep > 1)
	{
		DoUnDo(undostep-1);
	}

	RevertableCommand rc = undolist.back();

	// Dispatch
	undoredoflag = CUNDOREDO_UNDOING;
	for (list<CommittedCommand>::iterator it=rc.commandlist.begin(); it!=rc.commandlist.end(); ++it)
	{
		DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
		DASSERT(it->ival > _COMM_INTERNALBEGIN);
		int internalcommand = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (internalcommand == COMM_I_COMMAND)
		{
			ASSERT(pcount != 0);
			RevertableCommand rct;
			int command = COMM_NULL;
			++it;	//working layer
			for (int i=0; i<pcount; i++)
			{
				++it;
				if (!i)
				{
					DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
					DASSERT(it->ival < _COMM_INTERNALBEGIN);
					command = it->ival;
				}
				DASSERT(it!=rc.commandlist.end());
//				rct.PushCommand(&(*it));
			}
			for (int j=0; j<ucount; j++)
			{
				++it;
				DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
				DASSERT(it->ival > _COMM_INTERNALBEGIN);

				int usubcount = CI_GETPCOUNT(it->csub);
				DASSERT(usubcount != 0);

				int undocommand = it->ival;
				for (int i=0; i<usubcount; i++)
				{
					++it;
					DASSERT(it!=rc.commandlist.end());
					rct.PushCommand(&(*it));
				}
				if (undocommand == COMM_I_UNDO_COMMIT)
				{
					DoUnDoCommandCommit(&rct);
				}
				else if (undocommand == COMM_I_UNDO_PARAM)
				{
					ASSERT(command);
					DoUnDoCommandParam(command, &rct);
				}
			}
			MainInterface::getInstance().OnUnDo();
		}
		else if (internalcommand == COMM_I_ADDNODE)
		{
			ASSERT(pcount == 2);
			++it;
			int objid = it->ival;
//			GObject * obj = (GObject *)((it)->ival);
			++it;
			int objparentid = it->ival;
//			GObject * parent = (GObject *)((it)->ival);
//			DoUnDoAddNode(obj, parent);
			DoUnDoAddNode(objid, objparentid);
		}
		else if (internalcommand == COMM_I_DELETENODE)
		{
			ASSERT(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoUnDoDeleteNode(obj, parent);
		}
		else if (internalcommand == COMM_I_REPARENTNODE)
		{
			ASSERT(pcount == 3);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * oparent = (GObject *)((it)->ival);
			++it;
			GObject * aparent = (GObject *)((it)->ival);
			DoUnDoReparentNode(obj, oparent, aparent);
		}
	}
	undoredoflag = CUNDOREDO_NULL;

	undolist.pop_back();
	redolist.push_back(rc);

	if (!GObjectManager::getInstance().GetActiveLayer())
	{
		GObjectManager::getInstance().SetActiveLayer(CommandTemplate::workingLayer);
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
	if (redostep > 1)
	{
		DoReDo(redostep-1);
	}

	RevertableCommand rc = redolist.back();

	// Dispatch
	undoredoflag = CUNDOREDO_REDOING;
	for (list<CommittedCommand>::iterator it=rc.commandlist.begin(); it!=rc.commandlist.end(); ++it)
	{
		DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
		DASSERT(it->ival > _COMM_INTERNALBEGIN);
		int internalcommand = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (internalcommand == COMM_I_COMMAND)
		{
			ASSERT(pcount != 0);
			RevertableCommand rct;

			//
			++it;	//working layer
			DASSERT(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
			DASSERT(it->ival == COMM_I_COMM_WORKINGLAYER);
			GObject * pobj = GObjectManager::getInstance().FindObjectByID(it->csub);
			ASSERT(pobj);
			GObjectManager::getInstance().SetActiveLayer(pobj);
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
//			GObject * obj = (GObject *)((it)->ival);
			++it;
			int objparentid = it->ival;
//			GObject * parent = (GObject *)((it)->ival);
//			DoReDoAddNode(obj, parent);
			DoReDoAddNode(objid, objparentid);
		}
		else if (internalcommand == COMM_I_DELETENODE)
		{
			ASSERT(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoReDoDeleteNode(obj, parent);
		}
		else if (internalcommand == COMM_I_REPARENTNODE)
		{
			ASSERT(pcount == 3);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * oparent = (GObject *)((it)->ival);
			++it;
			GObject * aparent = (GObject *)((it)->ival);
			DoReDoReparentNode(obj, oparent, aparent);
		}
	}
	// Clear redoflag when push revertable
//	undoredoflag = CUNDOREDO_NULL;

	redolist.pop_back();
	// No need to push back undolist
	// All ReDo should done by command
//	undolist.push_back(rc);

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
	ProcessCommand();
	pendingparam = tp;
	MainInterface::getInstance().OnReDo();
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
	parent->RemoveChild(objid, true);
	return true;
}

bool Command::DoReDoAddNode( int objid, int objparentid )
{
	// No need to redo
	return true;
}

bool Command::DoUnDoDeleteNode( GObject * obj, GObject * parent )
{
	// All delete = move to undolist
	ASSERT(obj != NULL);
	ASSERT(parent != NULL);
	obj->Reparent(parent);
	return true;
}

bool Command::DoReDoDeleteNode( GObject * obj, GObject * parent )
{
	// Put back to undolist
//	assert(obj != NULL);
	GObjectManager::getInstance().MoveToUnDoList(obj);
	return true;
}

bool Command::DoUnDoReparentNode( GObject * obj, GObject * oparent, GObject * aparent )
{
	// Swap parent
	ASSERT(obj != NULL);
	ASSERT(oparent != NULL);
	ASSERT(aparent != NULL);

	DASSERT(obj->getParent() == aparent);
	obj->Reparent(oparent);
	return true;
}

bool Command::DoReDoReparentNode( GObject * obj, GObject * oparent, GObject * aparent )
{
	// Swap parent
//	assert(obj != NULL);
//	assert(oparent != NULL);
//	assert(aparent != NULL);

	DASSERT(obj->getParent() == oparent);
	obj->Reparent(aparent);
	return true;
}