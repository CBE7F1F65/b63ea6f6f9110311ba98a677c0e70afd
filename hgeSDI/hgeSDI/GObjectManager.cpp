#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "Main.h"
#include "ColorManager.h"
#include <sstream>
#include "CommandTemplate.h"

GObjectManager::GObjectManager()
{
	// Put all to Release
	pBaseNode = NULL;
	Release();
}
GObjectManager::~GObjectManager()
{
	if (pBaseNode)
	{
		delete pBaseNode;
	}
}

void GObjectManager::Init()
{
	Release();
	pBaseNode = new GLayer(0, ColorManager::getInstance().GetLayerLineColor(0), "");
	GLayer * pLayer = NewLayer(NULL, GetDefaultLayerName(stackedLayerIndex));
	CommandTemplate::Init(pLayer);
}

void GObjectManager::Release()
{
	if (pBaseNode)
	{
		pBaseNode->RemoveAllChildren(true);
	}
	undobasenode.RemoveAllChildren(true);
	Delete();
	stackedLayerIndex = 0;
	tarObjs = NULL;
	if (pBaseNode)
	{
		delete pBaseNode;
		pBaseNode = NULL;
	}
}

void GObjectManager::Update()
{
	pBaseNode->CallUpdate();

	bool tarupdated=false;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_GOBJECTS, &tarupdated);
	if (tar != tarObjs || tarupdated || pBaseNode->isModified())
	{
		tarObjs = tar;
		RenderHelper::getInstance().BeginRenderTar(tar);
		pBaseNode->CallRender();
		RenderHelper::getInstance().EndRenderTar();
	}
	pBaseNode->CallClearModify();
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
	if (changingbase->GetBase() == (GObject *)pBaseNode)
	{
		pBaseNode->CallResetID();
		if (!activeitem->isAttributeNode())
		{
			if (!activeitem->isRecDisplayFolded())
			{
				MainInterface::getInstance().OnRebuildLayerTree(changingbase, activeitem);
			}
		}
	}
}

GLayer * GObjectManager::NewLayer( GObject * node, const char * layername, int layerIndex/*=-1*/ )
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
	GObject * pnodeparent = node;
	if (!node || !node->getParent())
	{
		pnodeparent = pBaseNode;
	}
	else
	{
		pnodeparent = node->getParent();
	}
	return NewSubLayer(pnodeparent, layername, layerIndex);
}

GLayer * GObjectManager::NewSubLayer( GObject * node, const char * layername, int layerIndex/*=-1*/ )
{
	if (!node)
	{
		node = pBaseNode;
	}
	if (layerIndex < 0)
	{
		layerIndex = stackedLayerIndex;
		stackedLayerIndex++;
	}
	GLayer * pLayer = new GLayer(layerIndex, ColorManager::getInstance().GetLayerLineColor(layerIndex), layername);
	ASSERT(pLayer != NULL);
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
		_layerIndex = stackedLayerIndex;
	}
	stringstream ss;
	ss << StringManager::getInstance().GetNNLayerName() << " " << (_layerIndex+1);
	defaultLayerName = ss.str();
	return defaultLayerName.c_str();
}

GObject * GObjectManager::FindObjectByID( int id )
{
	return pBaseNode->FindNodeByID(id);
}

void GObjectManager::SetActiveLayer( GObject * pObj )
{
	ASSERT(pObj);
	GLayer * pLayer = (GLayer *)pObj->GetLayer();
	ASSERT(pLayer);
	
	MainInterface::getInstance().OnSetActiveLayer(pLayer);
}