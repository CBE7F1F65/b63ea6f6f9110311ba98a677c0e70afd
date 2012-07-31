#pragma once

#include "GBaseNode.h"

class SnapshotInfo{
public:
	SnapshotInfo(){ClearSet();};
	~SnapshotInfo(){};
	void ClearSet()
	{
//		diffstep=0;
//		bValid = true;
		savednode.RemoveAllChildren(true);
	};

	bool SaveNode(GBaseNode * pBaseNode=NULL);
	/*
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
	*/
	GBaseNodeCopyStack savednode;
};

class SnapshotManager
{
public:
	static SnapshotManager& getInstance() { static SnapshotManager instance; return instance; }

private:
	SnapshotManager();
	~SnapshotManager();
	SnapshotManager(SnapshotManager const&);
	void operator=(SnapshotManager const&);

public:
//	void OnDeleteUnDo(int maxsize);
//	void OnUnDo();
//	void OnPushRevertable();
//	void OnReDo();
//	void OnClearReDo(int nClear);
//	void OnClearUnDo(int nClear);

	void LoadNode(GBaseNodeCopyStack * node);

//	void _MovePointer(int movediff);

	int AddSnapshot();
	int DeleteSnapshot(int nSnapshot);
	bool RevertToSnapshot(int nSnapshot);
//	bool NeedLoad(int nSnapshot);

	void Release();

	list<SnapshotInfo>snapshots;
};

