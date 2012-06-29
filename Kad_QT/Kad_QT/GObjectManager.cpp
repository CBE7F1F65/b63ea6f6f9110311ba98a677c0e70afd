#include "StdAfx.h"
#include "GObjectManager.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "MainInterface.h"
#include "ColorManager.h"
#include <sstream>
#include "CommandTemplate.h"
#include "MarqueeSelect.h"
#include "Command.h"

#define GMLOCKTREESTATE_NULL			0x00
#define GMLOCKTREESTATE_REQUIRELOCK		0x01
#define GMLOCKTREESTATE_AFTERLOCK		0x02

GObjectManager::GObjectManager()
{
	// Put all to Release
	pBaseNode = NULL;
	nLockTreeChangeState = GMLOCKTREESTATE_NULL;
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
	pActiveLayer = pLayer;
	pLastActiveLayer = pLayer;
}

void GObjectManager::Release()
{
	nLockTreeChangeState = GMLOCKTREESTATE_REQUIRELOCK;
	if (pBaseNode)
	{
		pBaseNode->RemoveAllChildren(true);
	}
	undobasenode.RemoveAllChildren(true);
	OnTreeChanged(pBaseNode, pBaseNode, false);
	Delete();
	stackedLayerIndex = 0;
	tarObjs = NULL;
	pActiveLayer = NULL;
	pLastActiveLayer = NULL;
	pushedmovenodebyoffset.clear();
    bRedraw = false;
    bRenderUILayerIndicators = false;
//	bLockTreeChange = false;
}

void GObjectManager::Update()
{
	pBaseNode->CallUpdate();
	if (!pushedmovenodebyoffset.empty())
	{
		DoMoveNodeByOffsetBatch();
	}

	bool tarupdated=false;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_GOBJECTS, &tarupdated);
    if (tar != tarObjs || tarupdated || pBaseNode->isModified() || bRedraw)
	{
		tarObjs = tar;
		RenderHelper::getInstance().BeginRenderTar(tar);
		pBaseNode->CallRender();
		RenderHelper::getInstance().EndRenderTar();
	}
    bRedraw = false;
	pBaseNode->CallClearModify();

	if (nLockTreeChangeState == GMLOCKTREESTATE_REQUIRELOCK)
	{
		nLockTreeChangeState = GMLOCKTREESTATE_AFTERLOCK;
	}
	else if (nLockTreeChangeState == GMLOCKTREESTATE_AFTERLOCK)
	{
		nLockTreeChangeState = GMLOCKTREESTATE_NULL;
		OnTreeWillChange();
		OnTreeChanged(pBaseNode, NULL);
	}
}

void GObjectManager::Render()
{
	if (tarObjs)
	{
		RenderHelper::getInstance().TargetQuadRender_S(tarObjs, 0, 0, 0xffffffff);
    }
    if (bRenderUILayerIndicators)
    {
        list<GObject *> * lstSelectedNodes = GetSelectedNodes();
        GObject * pHover = MainInterface::getInstance().OnGetHoveringNode();
        for (list<GObject *>::iterator it=lstSelectedNodes->begin(); it!=lstSelectedNodes->end(); ++it)
        {
            if (pHover != *it)
            {
                (*it)->CallRender(LINECOLOR_UISELECT);
            }
        }
        if (pHover)
        {
            pHover->CallRender(LINECOLOR_INDICATING);
        }
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
		GObjectManager::getInstance().OnDeleteNode(node);
		MarqueeSelect::getInstance().OnDeleteNode(node);
		GObjectPicker::getInstance().OnDeleteNode(node);
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

void GObjectManager::OnTreeChanged( GObject * changingbase, GObject * activeitem, bool bSetActiveLayer/*=true*/ )
{
	if (!changingbase || !pBaseNode)
	{
		return;
	}

	MainInterface::getInstance().OnTreeLockChange(false);
	if (changingbase->GetBase() == (GObject *)pBaseNode)
	{
		pBaseNode->CallResetID();
//		ASSERT(activeitem != NULL);
		if (!activeitem)
		{
			activeitem = pLastToSetActiveNode;
		}
		ASSERT(activeitem);
		if (bSetActiveLayer)
		{
			if (activeitem)
			{
				pActiveLayer = (GLayer *)activeitem->GetLayer();
				ASSERT(pActiveLayer);
			}
		}
		if (nLockTreeChangeState)
		{
			if (activeitem)
			{
				if (!activeitem->isAttributeNode() && !activeitem->isRecDisplayFolded())
				{
					pLastToSetActiveNode = activeitem;
				}
			}
			return;
		}
		if (activeitem)
		{
			if (!activeitem->isAttributeNode() && !activeitem->isRecDisplayFolded())
			{
				MainInterface::getInstance().OnRebuildLayerTree(changingbase, activeitem);
			}
		}
    }
}

void GObjectManager::OnPrecisionChanged()
{
    if (pBaseNode)
    {
        pBaseNode->CallPrecisionChanged(MainInterface::getInstance().GetPrecision());
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
	GLayer * pLayer = new GLayer(layerIndex, layername);
	ASSERT(pLayer != NULL);
	node->AddChild(pLayer);

	return pLayer;

}

GLayer * GObjectManager::GetActiveLayer()
{
	/*
	GLayer * pActiveLayer = MainInterface::getInstance().OnGetActiveLayer();
	if (!pActiveLayer)
	{
		pActiveLayer = (GLayer *)pBaseNode->getNewestChild();
	}
//	ASSERT(pActiveLayer);
	*/
	ASSERT(pActiveLayer);
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

void GObjectManager::SetActiveLayer_Internal( GObject * pObj/*=NULL*/, bool bCallUI/*=true*/ )
{
	if (!pObj)
	{
		pObj = pActiveLayer;
	}
	DASSERT(pObj);
	if (pObj)
	{
		GLayer * pLayer = (GLayer *)pObj->GetLayer();
		DASSERT(pLayer);
		if (pLayer)
		{
//			pActiveLayer = pLayer;
			if (bCallUI)
			{
				MainInterface::getInstance().OnSetActiveLayer_Internal(pLayer);
			}
		}
		UpdateActiveLayer(pLayer);
	}

}

list<GObject*> * GObjectManager::GetSelectedNodes()
{
    return MainInterface::getInstance().OnGetSelectedNodes();
    /*
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
    */
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

GLayer * GObjectManager::GetActiveLayerFromUI()
{
    list<GObject *> * selectednodes = GetSelectedNodes();
	GLayer * pLayer = NULL;
    for (list<GObject *>::iterator it=selectednodes->begin(); it!=selectednodes->end(); ++it)
	{
		pLayer = (GLayer *)(*it)->GetLayer();
		if (pLayer)
		{
			return pLayer;
//			break;
		}
	}
	ASSERT(pActiveLayer);
	return pActiveLayer;
	/*
	if (pLayer)
	{
		SetActiveLayer_Internal(pLayer, false);
	}
	*/
}

void GObjectManager::UpdateActiveLayer( GLayer * pLayer/*=NULL*/, bool bothtoactive/*=false*/ )
{
	if (!pLayer)
	{
		pLayer = GetActiveLayer();
	}
	if (bothtoactive)
	{
		pLastActiveLayer = pLayer;
	}
	else
	{
		pLastActiveLayer = pActiveLayer;
	}
	pActiveLayer = pLayer;
	
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

void GObjectManager::SetLockTreeChange()
{
	nLockTreeChangeState = GMLOCKTREESTATE_REQUIRELOCK;
}

int GObjectManager::PushMoveNodeByOffsetForBatchCommand( GObject* pObj, float xoffset, float yoffset )
{
	bool dobatch = false;
	if (!pushedmovenodebyoffset.empty())
	{
		list<MoveNodeByOffsetInfo>::iterator it=pushedmovenodebyoffset.begin();
		if (!(it->IsSameOffset(xoffset, yoffset)))
		{
			dobatch = true;
		}
		if (!dobatch)
		{
			for (; it!=pushedmovenodebyoffset.end(); ++it)
			{
				if (it->IsDescendantOf(pObj))
				{
					dobatch = true;
					break;
				}
			}
		}
	}
	if (dobatch)
	{
		DoMoveNodeByOffsetBatch();
	}
	MoveNodeByOffsetInfo _mnboi(pObj, xoffset, yoffset);
	pushedmovenodebyoffset.push_back(_mnboi);
	return pushedmovenodebyoffset.size();
}

void GObjectManager::DoMoveNodeByOffsetBatch()
{
	if (pushedmovenodebyoffset.empty())
	{
		return;
	}
	list<MoveNodeByOffsetInfo>::iterator it=pushedmovenodebyoffset.begin();
	float xoffset = it->GetXOffset();
	float yoffset = it->GetYOffset();

	MainInterface::getInstance().OnCommandWithParam(
		COMM_MOVENODEBYOFFSET_BATCH,
		CCCWPARAM_F(xoffset),
		CCCWPARAM_F(yoffset),
		NULL
		);

	for (; it!=pushedmovenodebyoffset.end(); ++it)
	{
		GObject * pObj = it->GetObj();
		ASSERT(pObj);
		MainInterface::getInstance().OnCommandSingleParamI(pObj->getID());
		/*
		MainInterface::getInstance().OnCommandWithParam(
			COMM_MOVENODE,
			CCCWPARAM_I(pObj->getID()),
			CCCWPARAM_F(pObj->getX()+xoffset),
			CCCWPARAM_F(pObj->getY()+yoffset),
			NULL
			);
		Command * pcommand = &Command::getInstance();
		while (!pcommand->canCommandDone())
		{
			pcommand->ProcessCommand();
		}
		pcommand->ProcessCommand();
		*/
	}
	MainInterface::getInstance().OnCommandSingleParamI(-1);
	pushedmovenodebyoffset.clear();

	Command * pcommand = &Command::getInstance();
	while (!pcommand->canCommandDone())
	{
		pcommand->ProcessCommand();
	}
	pcommand->ProcessCommand();

}

void GObjectManager::OnDeleteNode( GObject * node )
{
	if (pushedmovenodebyoffset.empty())
	{
		return;
	}
	for (list<MoveNodeByOffsetInfo>::iterator it=pushedmovenodebyoffset.begin(); it!=pushedmovenodebyoffset.end();)
	{
		if (it->GetObj() == node)
		{
			it = pushedmovenodebyoffset.erase(it);
		}
		else
		{
			++it;
		}
	}
}
