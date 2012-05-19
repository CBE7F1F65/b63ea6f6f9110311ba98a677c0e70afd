#pragma once

#include "MainDependency.h"

enum
{
	RTID_GRID,
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

	HTARGET UpdateTarget(int id, int w, int h);

	list<RenderTargetInfo> tars;
	HGE * hge;
};