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
	static RenderTargetManager& getInstance()
	{
		static RenderTargetManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	RenderTargetManager();
	~RenderTargetManager();
	// Don't forget to declare these two.
	RenderTargetManager(RenderTargetManager const&);
	// Don't Implement
	void operator=(RenderTargetManager const&);
	// Don't implement

public:

	void SetNeedUpdate();
	HTARGET UpdateTarget(int id, bool * updated=0, int w=0, int h=0);

	list<RenderTargetInfo> tars;
	HGE * hge;
};