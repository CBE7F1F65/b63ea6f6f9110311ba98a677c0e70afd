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
#include "MarkingManager.h"
#include "DXFWriter.h"
#include "qmaininterface.h"

#include "URManager.h"

#define GMLOCKTREESTATE_NULL			0x00
#define GMLOCKTREESTATE_REQUIRELOCK		0x01
#define GMLOCKTREESTATE_AFTERLOCK		0x02

#define _GMDXFPIECEGROWTH	5


#define _GMLOCKTREENODECOUNT	10

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

	URManager::getInstance().Init(pBaseNode);

	pActiveLayer = pLayer;
	pLastActiveLayer = pLayer;
}

void GObjectManager::Release()
{
	URManager::getInstance().Release();
	nLockTreeChangeState = GMLOCKTREESTATE_REQUIRELOCK;
	if (pBaseNode)
	{
		pBaseNode->RemoveAllChildren(true);
		fakebasenode.RemoveAllChildren(true);
		OnTreeChanged(pBaseNode, pBaseNode, false);
	}
	Delete();
	stackedLayerIndex = 0;
	tarObjs = NULL;
	pActiveLayer = NULL;
	pLastActiveLayer = NULL;
	pushedmovenodebyoffset.clear();
    bRedraw = false;
    bRenderUILayerIndicators = false;
	bHandleVisible = true;
	nMoveActionID = 0;
	nMoveActionType = GMMATYPE_MOVE;
	bTryMove = false;
	bTryMoveBlock = false;

	pwillselfmovelist = NULL;

	bCloning = false;
	bManualCloning = false;
    bIsolateMode = true;
	mapCloneList.clear();
}

void GObjectManager::Update()
{
	pBaseNode->CallBaseUpdate();
	if (!pushedmovenodebyoffset.empty())
	{
		DoMoveNodeByOffsetBatch();
	}

	bool tarupdated=false;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_GOBJECTS, &tarupdated);
    if (bRedraw || tar != tarObjs || tarupdated || pBaseNode->isModified())
	{
		bRedraw = true;
		if (!nLockTreeChangeState)
		{
			tarObjs = tar;
			RenderHelper::getInstance().BeginRenderTar(tar);
			pBaseNode->CallRender();
			RenderHelper::getInstance().EndRenderTar();
			bRedraw = false;
		}
	}
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
}

void GObjectManager::RenderIndication()
{
	if (bRenderUILayerIndicators && !nLockTreeChangeState)
	{
		list<GObject *> * lstSelectedNodes = GetSelectedNodes(true);
		if (lstSelectedNodes)
		{
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

void GObjectManager::AddNodeToDelete( GObject * pDeletedObj )
{
	if (pDeletedObj)
	{
		nodetodelete.push_back(pDeletedObj);
		GObjectManager::getInstance().OnDeleteNode(pDeletedObj);
		MarqueeSelect::getInstance().OnDeleteNode(pDeletedObj);
		GObjectPicker::getInstance().OnDeleteNode(pDeletedObj);
		MarkingManager::getInstance().OnDeleteNode(pDeletedObj);
		QMainInterface::getInstance().OnDeleteNode(pDeletedObj);
	}
}
/*
void GObjectManager::MoveToUnDoList( GObject * node )
{
	if (node)
	{
		node->Reparent(&undobasenode);
		GObjectManager::getInstance().OnDeleteNode(node);
		MarqueeSelect::getInstance().OnDeleteNode(node);
		GObjectPicker::getInstance().OnDeleteNode(node);
		MarkingManager::getInstance().OnDeleteNode(node);
	}
}

GObject * GObjectManager::GetUnDoListFront()
{
	return undobasenode.getNewestChild();
}
*/
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
	if (changingbase->getBase() == (GObject *)pBaseNode)
	{
		pBaseNode->CallResetID();
//		ASSERT(activeitem != NULL);
		/*
		if (!activeitem || activeitem == pBaseNode)
		{
			activeitem = pLastToSetActiveNode;
		}
		*/
//		ASSERT(activeitem);
		if (bSetActiveLayer)
		{
			if (activeitem)
			{
				pActiveLayer = (GLayer *)activeitem->getLayer();
				ASSERT(pActiveLayer);
			}
		}
		if (nLockTreeChangeState)
		{
			return;
		}
		MainInterface::getInstance().OnRebuildLayerTree(changingbase, activeitem);
		pActiveLayer = GetActiveLayerFromUI();
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
	GLayer * pUIActiveLayer = GetActiveLayerFromUI();
	if (pUIActiveLayer)
	{
		SetActiveLayer_Internal(pUIActiveLayer, false);
		return pActiveLayer;
	}
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
		GLayer * pLayer = (GLayer *)pObj->getLayer();
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

list<GObject*> * GObjectManager::GetSelectedNodes( bool bFromUI )
{
	if (bFromUI)
	{
		return MainInterface::getInstance().OnGetSelectedNodes();
	}
	return MarqueeSelect::getInstance().OnGetSelectedNodes();
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
    list<GObject *> * selectednodes = GetSelectedNodes(true);
	GLayer * pLayer = NULL;
    for (list<GObject *>::iterator it=selectednodes->begin(); it!=selectednodes->end(); ++it)
	{
		pLayer = (GLayer *)(*it)->getLayer();
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

void GObjectManager::SetLockTreeChange(int count)
{
	if (count > _GMLOCKTREENODECOUNT || count < 0)
	{
		nLockTreeChangeState = GMLOCKTREESTATE_REQUIRELOCK;
	}
}

int GObjectManager::PushMoveNodeByOffsetForBatchCommand( GObject* pObj, float xoffset, float yoffset )
{
	if (IsTryMoveBlocking())
	{
		return 0;
	}
	if (fabsf(xoffset) < M_FLOATEPS && fabsf(yoffset) < M_FLOATEPS)
	{
		return pushedmovenodebyoffset.size();
	}

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

void GObjectManager::OnDeleteNode( GObject * pDeletedObj )
{
	if (pushedmovenodebyoffset.empty())
	{
		return;
	}
	for (list<MoveNodeByOffsetInfo>::iterator it=pushedmovenodebyoffset.begin(); it!=pushedmovenodebyoffset.end();)
	{
		if (it->GetObj() == pDeletedObj)
		{
			it = pushedmovenodebyoffset.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void GObjectManager::SetHandleVisible( bool bSet )
{
	if (bSet != bHandleVisible)
	{
		bHandleVisible = bSet;
		SetRedraw();
	}
}

int GObjectManager::GetNextMoveActionID( int nMoveType, int nAngle/*=0*/, float fXVal/*=1.0f*/, float fYVal/*=1.0f*/ )
{
	nMoveActionType = nMoveType;
	nMoveActionAngle = nAngle;
	fMoveActionXVal = fXVal;
	fMoveActionYVal = fYVal;
	return nMoveActionID++;
}

void GObjectManager::BeginTryMove()
{
	bTryMove = true;
}

void GObjectManager::EndTryMove()
{
	bTryMove = false;
}

void GObjectManager::BlockTryMove()
{
	bTryMoveBlock = true;
}

void GObjectManager::UnblockTryMove()
{
	bTryMoveBlock = false;
}

void GObjectManager::CancelTryMove()
{
	EndTryMove();
	UnblockTryMove();
	pBaseNode->CallCancelTryMove();
}

int GObjectManager::GetMoveTypeInfo( int * pAngle/*=NULL*/, float * pScaleX/*=NULL*/, float * pScaleY/*=NULL*/ )
{
	if (pAngle)
	{
		*pAngle = nMoveActionAngle;
	}
	if (pScaleX)
	{
		*pScaleX = fMoveActionXVal;
	}
	if (pScaleY)
	{
		*pScaleY = fMoveActionYVal;
	}
	return nMoveActionType;
}

bool GObjectManager::BeginClone()
{
	if (bCloning)
	{
		return false;
	}
	bCloning = true;
//	SetLockTreeChange();
	return true;
}

void GObjectManager::PushClone( GObject * pOri, GObject * pCloned )
{
	ASSERT(bCloning);
	mapCloneList.insert(pair<GObject *, GObject *>(pOri, pCloned));
}

void GObjectManager::EndClone()
{
	ASSERT(bCloning);
	bCloning = false;

	if (!mapCloneList.empty())
	{
		// Adjust Relationship here!
		for (map<GObject *, GObject *>::iterator it=mapCloneList.begin(); it!=mapCloneList.end(); ++it)
		{
			GNodeRelationshipGroup * pnrg = it->first->CreateRelationshipGroup(false, true);
			if (pnrg)
			{
				list<GNodeRelationship *> * plstRel = pnrg->GetRelList();
				for (list<GNodeRelationship *>::iterator pRelIt = plstRel->begin(); pRelIt!=plstRel->end(); ++pRelIt)
				{
					GNodeRelationship * pRel = *pRelIt;
					for (map<GObject *, GObject *>::iterator jt=mapCloneList.begin(); jt!=mapCloneList.end(); ++jt)
					{
						if (pRel->GetOther() == jt->first)
						{
							pRel->RestoreTo(it->second, jt->second);
							break;
						}
					}
				}
				delete pnrg;
			}
		}

		mapCloneList.clear();
    }
}

void GObjectManager::SetPreviewPrintMode(bool bSet)
{
	RenderHelper * prh = &RenderHelper::getInstance();
	if (bSet != prh->IsPreviewPrintMode())
	{
		prh->SetPreviewPrintMode(bSet);
		pBaseNode->CallRedrawModify();
	}
}

bool GObjectManager::IsPreviewPrintMode()
{
    return RenderHelper::getInstance().IsPreviewPrintMode();
}

bool GObjectManager::ReadXML( QXmlStreamReader * pqsr )
{
	if (!pqsr || !pBaseNode)
	{
		return false;
	}
	return pBaseNode->CallReadXML(*pqsr);
}

bool GObjectManager::WriteXML( QXmlStreamWriter * pqsw )
{
	if (!pqsw || !pBaseNode)
	{
		return false;
	}
	return pBaseNode->CallWriteXML(*pqsw);
}

bool GObjectManager::Dump( list<GObject *>& lobjs, bool bImages )
{
	if (lobjs.empty())
	{
		return false;
	}
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); )
	{
		if (!(*it)->isDisplayVisible())
		{
			it = lobjs.erase(it);
		}
		else
		{
			++it;
		}
	}

	list<GPieceBoundaryInfo> lstpbi;

	GObject * pfront = lobjs.front();
	float lside, tside, rside, bside;
	if (!pfront->GetBoundingBox(&lside, &tside, &rside, &bside))
	{
		ASSERT(false);
		return false;
	}

	GPieceBoundaryInfo _pbi(pfront);
	lstpbi.push_back(_pbi);
	
	float tlside, ttside, trside, tbside;
	for (list<GObject *>::iterator it=++(lobjs.begin()); it!=lobjs.end(); ++it)
	{
		GObject * pObj = *it;
		if (!pObj->GetBoundingBox(&tlside, &ttside, &trside, &tbside))
		{
			ASSERT(false);
			return false;
		}
		lside = min(tlside, lside);
		tside = min(ttside, tside);
		rside = max(trside, rside);
		bside = max(tbside, bside);

		bool bAdded = false;
		for (list<GPieceBoundaryInfo>::iterator jt=lstpbi.begin(); jt!=lstpbi.end(); ++jt)
		{
			bAdded = (*jt).AddObj(pObj);
			if (bAdded)
			{
				break;
			}
		}
		if (!bAdded)
		{
			GPieceBoundaryInfo _tpbi(pObj);
			lstpbi.push_back(_tpbi);
		}
	}

	RenderHelper * prh = &RenderHelper::getInstance();
	bool bOPreviewMode = prh->IsPreviewPrintMode();
	prh->SetPreviewPrintMode(true);

	MainInterface * pmain = &MainInterface::getInstance();

	QString qsdumpbasename = pmain->GetSaveFileName();
	if (qsdumpbasename.isEmpty())
	{
		qsdumpbasename = "Dump";
	}
	QFileInfo qf(qsdumpbasename);
	qsdumpbasename = qf.absolutePath()+"\\"+qf.completeBaseName();
	if (!QDir(qsdumpbasename).exists())
	{
		QDir().mkdir(qsdumpbasename);
	}
	qsdumpbasename += "\\";
	qsdumpbasename += qf.completeBaseName();

	float fprintmul = pmain->GetPrintMul();

	int printw = pmain->GetPrintsize_W();
	int printh = pmain->GetPrintsize_H();
	int printmargin = pmain->GetPrintMargin();
	float fdisplaymul = pmain->GetDisplayMul();

	float canvasx = (rside-lside)*fprintmul;
	float canvasy = (bside-tside)*fprintmul;
	float ocanvasx = canvasx*fdisplaymul/fprintmul;
	float ocanvasy = canvasy*fdisplaymul/fprintmul;


	int centerprintw = printw-printmargin*2;
	int centerprinth = printh-printmargin*2;

	int xrow = ((int)canvasx)/(centerprintw)+1;
	int yrow = ((int)canvasy)/(centerprinth)+1;

	canvasx = xrow * printw;
	canvasy = yrow * printh;

	QPixmap * ppixmap = NULL;
	QPainter * ppainter = NULL;
//	QPainterPath * ppath = NULL;

	if (bImages)
	{
		ppixmap = new QPixmap(canvasx, canvasy);
		ppixmap->fill();
		ppainter = new QPainter(ppixmap);
//		ppath = new QPainterPath();
	}

	GUICoordinate * pguic = &GUICoordinate::getInstance();
	pguic->EnterPrintMode();
	prh->SetPrintMode(ppainter, pguic->CtoSx(-lside)+printmargin, pguic->CtoSy(-tside)+printmargin, fprintmul/fdisplaymul);

	DXFWriter * pdxfw = &DXFWriter::getInstance();
	pdxfw->SetBaseName(qsdumpbasename.toUtf8(), 1.0f/(fdisplaymul*fdisplaymul));
	pdxfw->SetPieceInfo(ocanvasx, ocanvasy);
	pdxfw->WriteHeader();
	pdxfw->WriteTables();
	pdxfw->WriteBlocksBegin();

	pdxfw->WriteFrame();
	pdxfw->WriteGrain();
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
	{
		(*it)->CallRender();
	}

	pdxfw->WriteBlocksEnd();
	pdxfw->WriteEntities();
	pdxfw->WriteEOF();
	pdxfw->SetBaseName();


	for (list<GPieceBoundaryInfo>::iterator it=lstpbi.begin(); it!=lstpbi.end(); ++it)
	{
		GPieceBoundaryInfo * ppbi = &(*it);
		QString tlayername = ppbi->GetLayer()->getDisplayName();
		tlayername = tlayername.replace(' ', '_');
		pdxfw->SetBaseName((qsdumpbasename+tlayername).toUtf8(), 1/fdisplaymul);
		float xl;
		float yt;
		float xr;
		float yb;
		ppbi->GetBoundingBox(xl, yt, xr, yb);
		pdxfw->SetPieceInfo(xr-xl, yb-yt);
		pdxfw->WriteHeader();
		pdxfw->WriteTables();
		pdxfw->WriteBlocksBegin();
		pdxfw->WriteGrain();
		pdxfw->WriteFrame(_GMDXFPIECEGROWTH);

		ppbi->WriteDXFLines(pdxfw, 1.0f/fdisplaymul);


		pdxfw->WriteBlocksEnd();
		pdxfw->WriteEntities();
		pdxfw->WriteEOF();
		pdxfw->SetBaseName();
	}

	prh->SetPrintMode();
	pguic->ExitPrintMode();


	if (bImages && ppixmap && ppainter)
	{
		for (int i=0; i<xrow; i++)
		{
			for (int j=0; j<yrow; j++)
			{
				QPixmap pixtemp = ppixmap->copy(i*centerprintw, j*centerprinth, printw, printh);

				QPainter tpainter(&pixtemp);
				tpainter.setPen(Qt::lightGray);
				tpainter.drawRect(printmargin, printmargin, centerprintw, centerprinth);
				tpainter.setPen(Qt::darkGray);
				for (int k=0; k<printw/printmargin; k++)
				{
					if (k&1)
					{
						continue;
					}
					tpainter.drawLine(k*printmargin, printmargin, (k+1)*printmargin, printmargin);
					tpainter.drawLine(k*printmargin, printh-printmargin, (k+1)*printmargin, printh-printmargin);
				}
				for (int k=0; k<printh/printmargin; k++)
				{
					if (k&1)
					{
						continue;
					}
					tpainter.drawLine(printmargin, k*printmargin, printmargin, (k+1)*printmargin);
					tpainter.drawLine(printw-printmargin, k*printmargin, printw-printmargin, (k+1)*printmargin);
				}

				QString extname = "_";
				extname += QString().sprintf("%04d", i*yrow+j)+".png";
				tpainter.drawText(0, printh-printmargin, printw-printmargin/2, printmargin, Qt::AlignRight|Qt::AlignVCenter, qf.completeBaseName()+extname);
				pixtemp.save(qsdumpbasename+extname);
			}
		}

		ppainter->drawText(printmargin/2, 0, canvasx-printmargin, printmargin, Qt::AlignLeft|Qt::AlignVCenter, qf.completeBaseName());
		ppixmap->save( qsdumpbasename+".png" );

	}
	if (ppainter)
	{
		delete ppainter;
	}
	if (ppixmap)
	{
		delete ppixmap;
	}

	prh->SetPreviewPrintMode(bOPreviewMode);

	return true;
}


void GObjectManager::SetWillSelfMoveList( list<GObject *> * pobjs )
{
	pwillselfmovelist = pobjs;
}

bool GObjectManager::WillSelfMove( GObject * pObj )
{
	if (!pwillselfmovelist)
	{
		return false;
	}
	for (list<GObject *>::iterator it=pwillselfmovelist->begin(); it!=pwillselfmovelist->end(); ++it)
	{
		if (pObj==*it || pObj->isDescendantOf(*it))
		{
			return true;
		}
	}
	return false;
}

bool GObjectManager::AddCopyNode( GObject * pObj, int relid/*=-1*/ )
{
	if (IsInCopyList(pObj))
	{
		return true;
	}
	GObjectCopyInfo * pci = NULL;
	if (pObj->isRepresentablePoint())
	{
		if (pObj->isNotch())
		{
			GNotch * pNotch = (GNotch *) pObj;
			GObjectCopyInfo _oci(pNotch, *pNotch->getClingInfo(), pNotch->getX(), pNotch->getY(), relid);
			pci = &_oci;
		}
	}
	else if (pObj->isRepresentableLine())
	{
		GLine * pLine = (GLine *) pObj;
		GAnchorPoint * pBeginPoint = pLine->GetBeginPoint();
		GAnchorPoint * pEndPoint = pLine->GetEndPoint();
		if (pLine->isStraightLine())
		{
			GObjectCopyInfo _oci(pLine, pBeginPoint->getX(), pBeginPoint->getY(), pEndPoint->getX(), pEndPoint->getY(), pLine->GetSAInfo());
			pci = &_oci;
		}
		else
		{
			GHandlePoint * pBeginHandle = pBeginPoint->GetHandle();
			GHandlePoint * pEndHandle = pEndPoint->GetHandle();
			GObjectCopyInfo _oci(pLine, pBeginPoint->getX(), pBeginPoint->getY(), pEndPoint->getX(), pEndPoint->getY(), pBeginHandle->getX(), pBeginHandle->getY(), pEndHandle->getX(), pEndHandle->getY(), pLine->GetSAInfo());
			pci = &_oci;
		}
		if (pLine->GetSAInfo())
		{
		}

	}
	if (!pci)
	{
		return false;
	}
	lstcopy.push_back(*pci);

	if (pObj->isLine())
	{
		GLine * pLine = (GLine *)pObj;
		list<GPoint *> * pClingByList = pLine->getClingBy();
		if (pClingByList && !pClingByList->empty())
		{
			int relid = lstcopy.size();
			for (list<GPoint *>::iterator it=pClingByList->begin(); it!=pClingByList->end(); ++it)
			{
				GPoint * pClingByPoint = *it;
				if (pClingByPoint->isNotch())
				{
					AddCopyNode(pClingByPoint, relid-1);
				}
			}
		}
	}
	return true;
}

void GObjectManager::ClearCopiedNodes()
{
	lstcopy.clear();
}

bool GObjectManager::PasteNodes()
{
	if (lstcopy.empty())
	{
		return false;
	}

	MarqueeSelect * pmarq = &MarqueeSelect::getInstance();
	pmarq->DeSelectAll();

	GLayer * pLayer = GetActiveLayer();
	if (pLayer->isDisplayLocked())
	{
		return false;
	}
	SetLockTreeChange(lstcopy.size());
	for (list<GObjectCopyInfo>::iterator it=lstcopy.begin(); it!=lstcopy.end(); ++it)
	{
		GObjectCopyInfo * pci = &(*it);

		switch (pci->type)
		{
		case GOBJCOPY_POINT:
			{
				break;
			}
		case GOBJCOPY_NOTCH:
			{
				GObject * pNewRel = NULL;
				int nowrelid = 0;
				for (list<GObjectCopyInfo>::iterator jt=lstcopy.begin(); jt!=lstcopy.end(); ++jt)
				{
					if (nowrelid == pci->relid)
					{
						pNewRel = jt->pNewRel;
						break;
					}
					nowrelid++;
				}
				if (!pNewRel || !pNewRel->isLine())
				{
					ASSERT(false);
					return false;
				}
				pci->clinfo.SetClingTo((GLine *)pNewRel, pci->clinfo.GetClingVal(), 0, pci->clinfo.GetClingType());
				GNotch * pNotch = new GNotch(pLayer, &(pci->clinfo));
				if (!pNotch)
				{
					return false;
				}
				pci->SetNewRel(pNotch);
				break;
			}
		case GOBJCOPY_LINE:
			{
				GBezierLine * pBezier = new GBezierLine(pLayer, PointF2D(pci->xb, pci->yb), PointF2D(pci->xbh, pci->ybh), PointF2D(pci->xeh, pci->yeh), PointF2D(pci->xe, pci->ye));
				if (!pBezier)
				{
					return false;
				}
				pBezier->SetSA(&(pci->sainfo));
				pci->SetNewRel(pBezier);
				pmarq->AddSelect(pBezier);
				break;
			}
		}
	}
	return true;
}

bool GObjectManager::IsInCopyList( GObject * pObj )
{
	if (lstcopy.empty())
	{
		return false;
	}
	for (list<GObjectCopyInfo>::iterator it=lstcopy.begin(); it!=lstcopy.end(); ++it)
	{
		if (it->pObj == pObj)
		{
			return true;
		}
	}
	return false;
}

void GObjectManager::SaveSelectState()
{
	GBaseNode * pLastBase = URManager::getInstance().GetLastBase();
	if (pLastBase)
	{
		pLastBase->SaveSelectState();
	}
}

void GObjectManager::ReSelect( list<int> lstselect, int activelayerID )
{
	if (!lstselect.empty())
	{
		for (list<int>::iterator it=lstselect.begin(); it!=lstselect.end(); ++it)
		{
			GObject * pObj = FindObjectByID(*it);
			MarqueeSelect::getInstance().AddSelect(pObj);
		}
	}
	if (activelayerID >= 0)
	{
		GObject * pObj = FindObjectByID(activelayerID);
		ASSERT(pObj->isLayer());
		GLayer * pLayer = (GLayer *)pObj;
		SetActiveLayer_Internal(pLayer);
	}
}

bool GPieceBoundaryInfo::AddObj( GObject * pObj )
{
	if (!pObj->isDescendantOf(pLayer))
	{
		return false;
	}
	for (list<GObject *>::iterator it=lstObjs.begin(); it!=lstObjs.end(); ++it)
	{
		if (pObj == *it)
		{
			return true;
		}
	}
	lstObjs.push_back(pObj);
	float txl;
	float txr;
	float tyt;
	float tyb;
	pObj->GetBoundingBox(&txl, &tyt, &txr, &tyb);
	xl = min(txl, xl);
	yt = min(tyt, yt);
	xr = max(txr, xr);
	yb = max(tyb, yb);
	return true;
}

GPieceBoundaryInfo::GPieceBoundaryInfo( GObject * pObj )
{
	pLayer = pObj->getLayer();
	pObj->GetBoundingBox(&xl, &yt, &xr, &yb);
	lstObjs.push_back(pObj);
}

void GPieceBoundaryInfo::WriteDXFLines( DXFWriter * pdxfw, float fmul/*=1.0f*/ )
{
	if (!pdxfw)
	{
		return;
	}
	int layerID=DXFLAYER_INTERNAL;
	int qualityID=DXFLAYER_INTERNALQUALITYC;
	for (list<GObject *>::iterator it=lstObjs.begin(); it!=lstObjs.end(); ++it)
	{
		GObject * pObj = *it;
		if (pObj->isRepresentableLine())
		{
			GLine * pLine = (GLine *)pObj;
			/*
			if (pLine->GetSAInfo()->GetRawSA())
			{
				layerID = DXFLAYER_BOUNDARY;
				qualityID = DXFLAYER_QUALITYVALIDATIONC;
			}
			*/
			if (pLine->isStraightLine())
			{
				pdxfw->WriteLine(pLine->GetBeginPoint()->getX()-xl, pLine->GetBeginPoint()->getY()-yt, pLine->GetEndPoint()->getX()-xl, pLine->GetEndPoint()->getY()-yt, layerID, qualityID);
			}
			else
			{
				GBezierLine * pBezier = (GBezierLine *)pLine;
				pdxfw->WriteBezier(*(pBezier->getBSInfo()), -xl, -yt, layerID, qualityID);
			}
		}
	}
}
