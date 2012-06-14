#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "Main.h"
#include "ColorManager.h"
#include <sstream>
#include "CommandTemplate.h"
#include "MarqueeSelect.h"

GObjectManager::GObjectManager()
{
	// Put all to Release
	pBaseNode = NULL;
	bReleasing = false;
	Release();
}
GObjectManager::~GObjectManager()
{
}

void GObjectManager::Init()
{
	Release();
	pBaseNode = &GMainBaseNode::getInstance();//new GLayer(0, ColorManager::getInstance().GetLayerLineColor(0), "");
	GLayer * pLayer = NewLayer(NULL, GetDefaultLayerName(stackedLayerIndex));
	workinglayer = pLayer;
	lastworkinglayer = pLayer;
}

void GObjectManager::Release()
{
	bReleasing = true;
	if (pBaseNode)
	{
		pBaseNode->RemoveAllChildren(true);
	}
	undobasenode.RemoveAllChildren(true);
	OnTreeChanged(pBaseNode, pBaseNode);
	Delete();
	stackedLayerIndex = 0;
	tarObjs = NULL;
	workinglayer = NULL;
	lastworkinglayer = NULL;
	bReleasing = false;
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
		MarqueeSelect::getInstance().OnDeleteNode(node);
	}
}

void GObjectManager::MoveToUnDoList( GObject * node )
{
	if (node)
	{
		node->Reparent(&undobasenode);
	}
}

GObject * GObjectManager::GetUnDoListFront()
{
	return undobasenode.getNewestChild();
}

void GObjectManager::OnTreeWillChange()
{
	MainInterface::getInstance().OnTreeLockChange(true);
}

void GObjectManager::OnTreeChanged( GObject * changingbase, GObject * activeitem )
{
	MainInterface::getInstance().OnTreeLockChange(false);
	if (bReleasing)
	{
		return;
	}
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
	GLayer * pActiveLayer = MainInterface::getInstance().OnGetActiveLayer();
	if (!pActiveLayer)
	{
		pActiveLayer = (GLayer *)pBaseNode->getNewestChild();
	}
//	ASSERT(pActiveLayer);
	return pActiveLayer;
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

void GObjectManager::SetActiveLayer_Internal( GObject * pObj/*=NULL*/ )
{
	if (!pObj)
	{
		pObj = workinglayer;
	}
	DASSERT(pObj);
	if (pObj)
	{
		GLayer * pLayer = (GLayer *)pObj->GetLayer();
		DASSERT(pLayer);
		if (pLayer)
		{
			MainInterface::getInstance().OnSetActiveLayer_Internal(pLayer);
		}
	}	
}

list<GObject*> * GObjectManager::GetSelectedNodes()
{
	selectednodes.clear();
	GObject * _pobj = NULL;
	int nextfromindex = -1;
	do 
	{
		_pobj = MainInterface::getInstance().OnGetSelectedNodes(&nextfromindex);
		if (_pobj)
		{
			selectednodes.push_back(_pobj);
		}
	} while (_pobj);

	return &selectednodes;
}

bool GObjectManager::CanDeleteItem( GObject * pObj )
{
	if (pObj)
	{
		if (pObj->isAttributeNode())
		{
			return false;
		}
		GObject * parent = pObj->getParent();
		if (parent)
		{
			if (parent != pBaseNode || parent->getChildren()->size() > 1)
			{
				return true;
			}
		}
	}
	return false;
}

bool GObjectManager::CanReparentItem( GObject * pObj, int newparentindex )
{
	if (pObj)
	{
		if (pObj->isAttributeNode())
		{
			return false;
		}
		if (newparentindex >= 0)
		{
			GObject * pNewPlaceAfter = FindObjectByID(newparentindex);
			if (!pNewPlaceAfter)
			{
				return false;
			}
			if (pNewPlaceAfter->isAttributeNode())
			{
				return false;
			}
			if (pNewPlaceAfter == pObj || pNewPlaceAfter->isDescendantOf(pObj))
			{
				return false;
			}
			return true;
		}
		else if (CanDeleteItem(pObj))
		{
			return true;
		}
	}
	return false;
}

bool GObjectManager::CanDuplicateItem( GObject * pObj )
{
	if (pObj->getParent())
	{
		return true;
	}
	return false;
}

void GObjectManager::OnInternalActiveLayerSetDone()
{
	UpdateWorkingLayer();
}

void GObjectManager::UpdateWorkingLayer( GLayer * pLayer/*=NULL*/, bool bothtoactive/*=false*/ )
{
	if (!pLayer)
	{
		pLayer = GetActiveLayer();
	}
	if (bothtoactive)
	{
		lastworkinglayer = pLayer;
	}
	else
	{
		lastworkinglayer = workinglayer;
	}
	workinglayer = pLayer;
	
}

GLayer * GObjectManager::GetDragDroppedLayerNode()
{
	GLayer * pLayerNode=NULL;
	GObject * pAfterNode=NULL;
	if (MainInterface::getInstance().OnGetDragDroppedNodes(&pLayerNode, &pAfterNode))
	{
		return pLayerNode;
	}
	return NULL;
}

GObject * GObjectManager::GetDragDroppedAfterNode()
{
	GLayer * pLayerNode=NULL;
	GObject * pAfterNode=NULL;
	if (MainInterface::getInstance().OnGetDragDroppedNodes(&pLayerNode, &pAfterNode))
	{
		return pAfterNode;
	}
	return NULL;
}