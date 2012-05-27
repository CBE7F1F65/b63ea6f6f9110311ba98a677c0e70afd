#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"

GObjectManager::GObjectManager()
{
	tarobjs = NULL;
}
GObjectManager::~GObjectManager()
{

}

void GObjectManager::Update()
{
	basenode.OnUpdate();

	bool tarupdated=false;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_GOBJECTS, &tarupdated);
	if (tar != tarobjs || tarupdated || basenode.bModified)
	{
		tarobjs = tar;
		RenderHelper::getInstance().BeginRenderTar(tar);
		basenode.OnRender();
		RenderHelper::getInstance().EndRenderTar();
	}
	basenode.OnClearModify();
}

void GObjectManager::Render()
{
	if (tarobjs)
	{
		RenderHelper::getInstance().TargetQuadRender_S(tarobjs, 0, 0, 0xffffffff);
	}
}

void GObjectManager::Delete()
{
	if (!nodetodelete.empty())
	{
		for (list<GObject*>::iterator it=nodetodelete.begin(); it!=nodetodelete.end(); )
		{
			if ((*it))
			{
				delete (*it);
			}
			it = nodetodelete.erase(it);
		}
	}
}

void GObjectManager::AddNodeToDelete( GObject * node )
{
	if (node)
	{
		nodetodelete.push_back(node);
	}
}

void GObjectManager::Init()
{
	Release();
}

void GObjectManager::Release()
{
	basenode.RemoveAllChildren();
	undobasenode.RemoveAllChildren();
	Delete();
}

void GObjectManager::MoveToUnDoList( GObject * node )
{
	if (node)
	{
		node->Reparent(&undobasenode);
	}
}