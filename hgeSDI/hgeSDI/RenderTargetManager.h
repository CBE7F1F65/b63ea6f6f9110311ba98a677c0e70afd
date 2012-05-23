#pragma once

#include "MainDependency.h"

enum
{
	RTID_GRID,
	RTID_COMMAND,
};

struct RenderTargetInfo
{
	int id;
	HTARGET tar;
	int w;
	int h;
};

class RenderTargetManager
{
public:
	RenderTargetManager();
	~RenderTargetManager();

	static RenderTargetManager * getInstance();

	HTARGET UpdateTarget(int id, int w=0, int h=0);

	list<RenderTargetInfo> tars;
	HGE * hge;
};