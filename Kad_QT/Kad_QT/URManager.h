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

	bool CanUnDo();
	bool CanReDo();
	int PushUnDo(GBaseNode * pBase);
	bool UnDo(GBaseNode * pBase);
	bool ReDo(GBaseNode * pBase);

	int ClearReDo();

private:
	int index;
	vector<URBaseNodeInfo *> stack;
};

