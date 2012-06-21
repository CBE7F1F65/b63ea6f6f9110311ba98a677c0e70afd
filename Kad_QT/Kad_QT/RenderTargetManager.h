#pragma once

#include "MainDependency.h"

enum
{
	RTID_GRID,
	RTID_COMMAND,
	RTID_GOBJECTS,
};

struct RenderTargetInfo
{
	int id;
	HTARGET tar;
	int w;
	int h;
	bool needupdate;
};

class RenderTargetManager
{
public:
	static RenderTargetManager& getInstance() { static RenderTargetManager instance; return instance; }

private:
	RenderTargetManager();
	~RenderTargetManager();
	RenderTargetManager(RenderTargetManager const&);
	void operator=(RenderTargetManager const&);

public:

	void SetNeedUpdate();
	HTARGET UpdateTarget(int id, bool * updated=0, int w=0, int h=0);

	list<RenderTargetInfo> tars;
	HGE * hge;
};