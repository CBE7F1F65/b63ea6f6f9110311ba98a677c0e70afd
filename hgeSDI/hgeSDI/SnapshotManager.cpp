#include "StdAfx.h"
#include "SnapshotManager.h"
#include "Main.h"


SnapshotManager::SnapshotManager(void)
{
}


SnapshotManager::~SnapshotManager(void)
{
}

void SnapshotManager::OnDeleteUnDo( int maxsize )
{
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (!it->isSaved())
		{
			if (it->diffstep >= maxsize)
			{
				it->SetSaved(SaveNode());
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
		if (!it->isSaved())
		{
			if (it->diffstep < 0)
			{
				it->SetSaved(SaveNode());
			}
		}
	}
}

void SnapshotManager::_MovePointer( int movediff )
{
	if (movediff)
	{
		for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
		{
			if (!it->isSaved())
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
			if (it->isSaved())
			{
				LoadNode(it->savednode);
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

GObject * SnapshotManager::SaveNode()
{
	// ToDo
	return (GObject *)1;
}

void SnapshotManager::LoadNode( GObject * node )
{

}

bool SnapshotManager::NeedLoad( int nSnapshot )
{
	int i=0;
	for (list<SnapshotInfo>::iterator it=snapshots.begin(); it!=snapshots.end(); ++it)
	{
		if (i==nSnapshot)
		{
			if (it->isSaved())
			{
				return true;
			}
		}
		i++;
	}
	return false;
}