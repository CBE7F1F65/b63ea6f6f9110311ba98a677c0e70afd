#pragma once

#include "GBaseNode.h"

class SnapshotInfo{
public:
	SnapshotInfo(){ClearSet();};
	~SnapshotInfo(){};
	void ClearSet()
	{
		diffstep=0;
		bValid = true;
		savednode.RemoveAllChildren(true);
	};

	bool SaveNode(GBaseNode * pBaseNode=NULL);
	bool isValid()
	{
		return bValid;
	};
	void Invalid()
	{
		bValid = false;
	};

	int diffstep;
	bool bValid;
	GBaseNode savednode;
};

class SnapshotManager
{
public:
	static SnapshotManager& getInstance()
	{
		static SnapshotManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	SnapshotManager();
	~SnapshotManager();
	// Don't forget to declare these two.
	SnapshotManager(SnapshotManager const&);
	// Don't Implement
	void operator=(SnapshotManager const&);
	// Don't implement

public:
	void OnDeleteUnDo(int maxsize);
	void OnUnDo();
	void OnPushRevertable();
	void OnReDo();
	void OnClearReDo(int nClear);
	void OnClearUnDo(int nClear);

	void LoadNode(GObject * node);

	void _MovePointer(int movediff);

	int AddSnapshot();
	int DeleteSnapshot(int nSnapshot);
	bool RevertToSnapshot(int nSnapshot);
	bool NeedLoad(int nSnapshot);

	void Release();

	list<SnapshotInfo>snapshots;
};
