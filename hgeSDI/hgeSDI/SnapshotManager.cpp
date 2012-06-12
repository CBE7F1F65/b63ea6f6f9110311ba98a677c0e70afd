#include "StdAfx.h"
#include "SnapshotManager.h"
#include "Main.h"

#include "GObjectManager.h"
#include "Command.h"


SnapshotManager::SnapshotManager(void)
{
}


SnapshotManager::~SnapshotManager(void)
{
}

void SnapshotManager::Release()
{
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		it->ClearSet();
	}
}

void SnapshotManager::OnDeleteUnDo( int maxsize )
{
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (it->isValid())
		{
			if (it->diffstep >= maxsize)
			{
				it->Invalid();
			}
		}
	}
}

void SnapshotManager::OnUnDo()
{
	_MovePointer(-1);
}

void SnapshotManager::OnPushRevertable()
{
	_MovePointer(1);
}

void SnapshotManager::OnReDo()
{
	_MovePointer(1);
}

void SnapshotManager::OnClearReDo( int nClear )
{
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (it->isValid())
		{
			if (it->diffstep < 0)
			{
				it->Invalid();
			}
		}
	}
}

void SnapshotManager::OnClearUnDo( int nClear )
{
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (it->isValid())
		{
			it->Invalid();
		}
	}
}

void SnapshotManager::_MovePointer( int movediff )
{
	if (movediff)
	{
		for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
		{
			if (it->isValid())
			{
				it->diffstep += movediff;
			}
		}
	}
}

int SnapshotManager::AddSnapshot()
{
	SnapshotInfo _ssinfo;
	snapshots.push_back(_ssinfo);
	snapshots.back().SaveNode();
	return (int)snapshots.size()-1;
}

int SnapshotManager::DeleteSnapshot( int nSnapshot )
{
	int i=0;
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (i==nSnapshot)
		{
			snapshots.erase(it);
			break;
		}
		i++;
	}
	return (int)snapshots.size();
}

bool SnapshotManager::RevertToSnapshot( int nSnapshot )
{
	int i=0;
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (i==nSnapshot)
		{
			if (!it->isValid())
			{
				LoadNode(&(it->savednode));
			}
			else
			{
				MainInterface * pmain = &MainInterface::getInstance();
				pmain->CallUnDoReDo(-it->diffstep);
			}
			return true;
			break;
		}
		i++;
	}
	return false;
}

void SnapshotManager::LoadNode( GObject * node )
{
	ASSERT(node);
	GObjectManager * pgm = &GObjectManager::getInstance();
	list<GObject *>dellist;
	for (list<GObject *>::iterator it=pgm->pBaseNode->getChildren()->begin(); it!=pgm->pBaseNode->getChildren()->end(); ++it)
	{
		dellist.push_back(*it);
	}
	pgm->pBaseNode->CopyBaseFrom((GBaseNode *)node);
	for (list<GObject *>::iterator it=dellist.begin(); it!=dellist.end(); ++it)
	{
		pgm->pBaseNode->RemoveChild(*it, true);
	}
	Command::getInstance().StepTo(CSI_TERMINAL);
	Command::getInstance().ClearUnDo();
	Command::getInstance().ClearReDo();
}

bool SnapshotManager::NeedLoad( int nSnapshot )
{
	int i=0;
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (i==nSnapshot)
		{
			if (!it->isValid())
			{
				return true;
			}
		}
		i++;
	}
	return false;
}

bool SnapshotInfo::SaveNode(GBaseNode * pBaseNode/* =NULL */)
{
	if (!pBaseNode)
	{
		pBaseNode = GObjectManager::getInstance().pBaseNode;
	}
	if (pBaseNode)
	{
		pBaseNode->CopyBaseTo(&savednode);
	}
	bValid = true;
	return true;
}