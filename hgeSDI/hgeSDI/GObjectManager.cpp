#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "Main.h"
#include "ColorManager.h"
#include <sstream>

GObjectManager::GObjectManager()
{
	// Put all to Release
	Release();
}
GObjectManager::~GObjectManager()
{
}

void GObjectManager::Init()
{
	Release();
	NewLayer(NULL, GetDefaultLayerName(layerIndex));
}

void GObjectManager::Release()
{
	basenode.RemoveAllChildren(true);
	undobasenode.RemoveAllChildren(true);
	Delete();
	layerIndex = 0;
	tarObjs = NULL;
}

void GObjectManager::Update()
{
	basenode.CallUpdate();

	bool tarupdated=false;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_GOBJECTS, &tarupdated);
	if (tar != tarObjs || tarupdated || basenode.isModified())
	{
		tarObjs = tar;
		RenderHelper::getInstance().BeginRenderTar(tar);
		basenode.CallRender();
		RenderHelper::getInstance().EndRenderTar();
	}
	basenode.CallClearModify();
}

void GObjectManager::Render()
{
	if (tarObjs)
	{
		RenderHelper::getInstance().TargetQuadRender_S(tarObjs, 0, 0, 0xffffffff);
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

void GObjectManager::MoveToUnDoList( GObject * node )
{
	if (node)
	{
		node->Reparent(&undobasenode);
	}
}

void GObjectManager::OnTreeChanged( GObject * changingbase, GObject * activeitem )
{
	if (changingbase->GetBase() == &basenode)
	{
		if (!activeitem->isAttributeNode())
		{
			if (!activeitem->isRecDisplayFolded())
			{
				MainInterface::getInstance().OnRebuildLayerTree(changingbase, activeitem);
			}
		}
	}
}

GLayer * GObjectManager::NewLayer( GObject * node/*=NULL*/, const char * layername )
{
	/*
	if (!node)
	{
		node = &basenode;
	}
	GLayer * pLayer = new GLayer(layerIndex, ColorManager::getInstance().GetLayerLineColor(layerIndex));
	assert(pLayer != NULL);
	layerIndex++;
	if (node->getParent())
	{
		node->getParent()->AddChild(pLayer);
	}
	else
	{
		node->AddChild(pLayer);
	}

	return pLayer;
	*/
	if (!node)
	{
		node = &basenode;
	}
	return NewSubLayer(node->getParent(), layername);
}

GLayer * GObjectManager::NewSubLayer( GObject * node/*=NULL*/, const char * layername )
{
	if (!node)
	{
		node = &basenode;
	}
	GLayer * pLayer = new GLayer(layerIndex, ColorManager::getInstance().GetLayerLineColor(layerIndex), layername);
	assert(pLayer != NULL);
	layerIndex++;
	node->AddChild(pLayer);

	return pLayer;

}

GLayer * GObjectManager::GetActiveLayer()
{
	return MainInterface::getInstance().OnGetActiveLayer();
}

const char * GObjectManager::GetDefaultLayerName( int _layerIndex/*=-1 */ )
{
	if (_layerIndex < 0)
	{
		_layerIndex = layerIndex;
	}
	stringstream ss;
	ss << StringManager::getInstance().GetNNLayerName() << " " << (_layerIndex+1);
	defaultLayerName = ss.str();
	return defaultLayerName.c_str();
}