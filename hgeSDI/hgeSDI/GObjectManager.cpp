#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"

GObjectManager::GObjectManager()
{
}
GObjectManager::~GObjectManager()
{

}

void GObjectManager::Update()
{
	basenode.OnUpdate();
}

void GObjectManager::Render()
{
	basenode.OnRender();
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

void GObjectManager::AddNotToDelete( GObject * node )
{
	if (node)
	{
		nodetodelete.push_back(node);
	}
}

void GObjectManager::Init()
{

}

void GObjectManager::Release()
{
	basenode.RemoveAllChildren();
	Delete();
}