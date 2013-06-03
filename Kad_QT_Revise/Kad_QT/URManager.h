#pragma once

#include "GBaseNode.h"


class URBaseNodeInfo
{
public:
	URBaseNodeInfo(GBaseNode * pBase);
	virtual ~URBaseNodeInfo();

	GBaseNodeCopyStack * getBase(){return &base;};
private:
	GBaseNodeCopyStack base;
};


class URManager
{
public:
	static URManager& getInstance() { static URManager instance; return instance; }

private:
	URManager();
	~URManager();
	URManager(URManager const&);
	void operator=(URManager const&);

public:
	void Init(GBaseNode * pBase);
	void Release();

	bool CanUnDo(int step=1);
	bool CanReDo(int step=1);
	int PushUnDo(GBaseNode * pBase);
	bool UnDo(GBaseNode * pBase, int step=1);
	bool ReDo(GBaseNode * pBase, int step=1);

	GBaseNode * GetLastBase();

	int ClearReDo();

private:
	int index;
	vector<URBaseNodeInfo *> stack;
};

