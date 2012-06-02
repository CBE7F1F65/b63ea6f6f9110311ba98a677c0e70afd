#include "stdafx.h"
#include "Command.h"

#include "GObject.h"
#include "GObjectManager.h"
#include "Main.h"


bool Command::DoUnDo( int undostep/*=1*/ )
{
	if (undolist.empty() || undostep < 1)
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
		assert(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
		assert(it->ival > _COMM_INTERNALBEGIN);
		int command = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (command == COMM_I_COMMAND)
		{
			assert(pcount != 0);
			RevertableCommand rct;
			for (int i=0; i<pcount; i++)
			{
				++it;
				assert(it!=rc.commandlist.end());
				rct.PushCommand(&(*it));
			}
			DoUnDoCommandSingle(&rct, ucount);
		}
		else if (command == COMM_I_ADDNODE)
		{
			assert(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoUnDoAddNode(obj, parent);
		}
		else if (command == COMM_I_DELETENODE)
		{
			assert(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoUnDoDeleteNode(obj, parent);
		}
		else if (command == COMM_I_REPARENTNODE)
		{
			assert(pcount == 3);
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
		assert(it->type == COMMITTEDCOMMANDTYPE_COMMAND);
		assert(it->ival > _COMM_INTERNALBEGIN);
		int command = it->ival;

		int pcount = CI_GETPCOUNT(it->csub);
		int ucount = CI_GETUCOUNT(it->csub);
		if (command == COMM_I_COMMAND)
		{
			assert(pcount != 0);
			RevertableCommand rct;
			for (int i=0; i<pcount; i++)
			{
				++it;
				assert(it!=rc.commandlist.end());
				rct.PushCommand(&(*it));
			}
			DoReDoCommandSingle(&rct);
		}
		else if (command == COMM_I_ADDNODE)
		{
			assert(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoReDoAddNode(obj, parent);
		}
		else if (command == COMM_I_DELETENODE)
		{
			assert(pcount == 2);
			++it;
			GObject * obj = (GObject *)((it)->ival);
			++it;
			GObject * parent = (GObject *)((it)->ival);
			DoReDoDeleteNode(obj, parent);
		}
		else if (command == COMM_I_REPARENTNODE)
		{
			assert(pcount == 3);
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

bool Command::DoUnDoCommandSingle( RevertableCommand * rc, int ucount )
{
	ProcessUnDoCommand(rc, ucount);
	MainInterface::getInstance().OnUnDo();
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

bool Command::DoUnDoAddNode( GObject * obj, GObject * parent )
{
	// Destroy Added
	assert(obj != NULL);
	assert(parent != NULL);

	assert(obj->parent == parent);
	parent->RemoveChild(obj);
//	obj->RemoveFromParent();
	return true;
}

bool Command::DoReDoAddNode( GObject * obj, GObject * parent )
{
	// No need to redo
	return true;
}

bool Command::DoUnDoDeleteNode( GObject * obj, GObject * parent )
{
	// All delete = move to undolist
	assert(obj != NULL);
	assert(parent != NULL);
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
	assert(obj != NULL);
	assert(oparent != NULL);
	assert(aparent != NULL);

	assert(obj->parent == aparent);
	obj->Reparent(oparent);
	return true;
}

bool Command::DoReDoReparentNode( GObject * obj, GObject * oparent, GObject * aparent )
{
	// Swap parent
//	assert(obj != NULL);
//	assert(oparent != NULL);
//	assert(aparent != NULL);

	assert(obj->parent == oparent);
	obj->Reparent(aparent);
	return true;
}