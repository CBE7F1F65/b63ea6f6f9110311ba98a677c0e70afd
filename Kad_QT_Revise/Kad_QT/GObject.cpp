#include "StdAfx.h"
#include "GObject.h"

#include "GObjectManager.h"
#include "StringManager.h"
#include "ColorManager.h"
#include "RenderHelper.h"

//#include "MainInterface.h"
//#include "Command.h"

#define _GOBJFONTSIZE	20

GObject * GObject::pTreeBase=NULL;
GObjectManager * GObject::pgm=&GObjectManager::getInstance();


GObject::GObject(void)
{
	pParent = NULL;
	bModified = false;
	nNonAttributeChildrenCount = 0;

	bDisplayFolded = false;
	nDisplayState = GOBJ_DISPSTATE_NORMAL;

	nTryState = 0;
	fTryMove_bx = 0;
	fTryMove_by = 0;
	
    bCloning = false;
	
	nMoveActionID = -1;
	nUpdateMoveActionID = -1;

	nModifyVersion = 0;
	/*
	int tsize = 0;
	tsize += sizeof(nMoveActionID);
	tsize += sizeof(nUpdateMoveActionID);
	tsize += sizeof(nNonAttributeChildrenCount);
	tsize += sizeof(strDisplayName);
	tsize += sizeof(nDisplayState);
	tsize += sizeof(lsLineColorSet);
	tsize += sizeof(nID);
	tsize += sizeof(bModified);
	tsize += sizeof(nModifyVersion);
	tsize += sizeof(nTryState);
	tsize += sizeof(fTryMove_bx);
	tsize += sizeof(fTryMove_by);
	tsize += sizeof(bCloning);
	tsize += sizeof(pParent);
	tsize += sizeof(listChildren);
	int size = sizeof(GObject);
	*/
	_SetID();
	OnInit();
}

GObject::~GObject(void)
{
//	RemoveAllChildren(true);
}

#define _FOREACH_L(T, IT, L)	\
	for (list<T>::iterator IT=L.begin(); IT!=L.end(); ++IT)
#define FOREACH_GOBJ_CHILDREN_IT()	\
	_FOREACH_L(GObject*, it, lstChildren)

#define _FOREACHREV_L(T, IT, L)	\
	for (list<T>::reverse_iterator IT=L.rbegin(); IT!=L.rend(); ++IT)
#define FOREACHREV_GOBJ_CHILDREN_IT()	\
	_FOREACHREV_L(GObject*, it, lstChildren)

int GObject::_ActualAddChildAfterObj( GObject * child, GObject * afterobj )
{
	if (child)
	{
		ASSERT(child != this);
		ASSERT(child != afterobj);
		ASSERT(!child->isAncestorOf(this));

		OnAddChildAfterObj(child, afterobj);

		child->_SetParent(this);
		child->OnEnter();

		child->CallModify();

		//
		if (!afterobj)
		{
			lstChildren.push_front(child);
		}
		else
		{
			bool bdone = false;
			FOREACH_GOBJ_CHILDREN_IT()
			{
				if ((*it) == afterobj)
				{
					++it;
					if (it!=lstChildren.end())
					{
						lstChildren.insert(it, child);
						bdone = true;
					}
					break;
				}
			}
			if (!bdone)
			{
				lstChildren.push_back(child);
			}
		}

		if (!child->isAttributeNode())
		{
			_ModifyNonAttributeChildrenCount(1);
		}

		_CallTreeChanged(this, child);

		return child->nID;
	}
	return -1;
}

list<GObject *>::iterator GObject::_ActualRemoveChild( list<GObject *>::iterator it, bool bRelease )
{
	GObject * child = *it;
	if (!child->isAttributeNode())
	{
		_ModifyNonAttributeChildrenCount(-1);
	}
	OnRemoveChild(child, bRelease);

	child->CallRemove();
	if (bRelease)
	{
		child->CallRelease();
	}
	child->pParent = NULL;
	it = lstChildren.erase(it);

	// Add OnModify and OnTreeChanged after all operation done!!
//	OnModify();
//	pgm->OnTreeChanged(this);

	return it;
}

void GObject::_SetID( int _ID/*=-1*/ )
{
	nID = _ID;
}

int GObject::AddChild( GObject * child )
{
	return _ActualAddChildAfterObj(child, NULL);
}

int GObject::AddChildAfterObj( GObject* child, GObject*afterobj )
{
	return _ActualAddChildAfterObj(child, afterobj);
}

void GObject::_SetParent( GObject * _parent )
{
	ASSERT(pParent==NULL);
	pParent = _parent;
	// Do not Add Children
}

int GObject::RemoveChild( int _ID, bool bRelease )
{
	_CallTreeWillChange();
	int ret = _RemoveChild(_ID, bRelease);
	_CallTreeChanged(this, this);
	return ret;
}

int GObject::RemoveChild( GObject * child, bool bRelease )
{
	_CallTreeWillChange();
	int ret = _RemoveChild(child, bRelease);
	_CallTreeChanged(this, this);
	return ret;
}


int GObject::_RemoveChild( int _ID, bool bRelease )
{
	// Do not use FOREACH
	if (!lstChildren.empty())
	{
		for (list<GObject*>::iterator it=lstChildren.begin(); it!=lstChildren.end();)
		{
			if ((*it)->nID == _ID)
			{
				it = _ActualRemoveChild(it, bRelease);
				/*
				if (bRelease)
				{
					(*it)->OnRelease();
				}
				it = children.erase(it);
				*/
			}
			else
			{
				++it;
			}
		}
	}
	CallModify();
//	_CallTreeChanged(this, this);
//	pgm->OnTreeChanged(this, this);
	return lstChildren.size();
}

int GObject::_RemoveChild( GObject * child, bool bRelease )
{
	if (!child)
	{
		return -1;
	}
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it) == child)
			{
				_ActualRemoveChild(it, bRelease);
				/*
				if (bRelease)
				{
					child->OnRelease();
				}
				it = children.erase(it);
				*/
				break;
			}
		}
	}
	CallModify();
//	_CallTreeChanged(this, this);
//	pgm->OnTreeChanged(this, this);
	return lstChildren.size();
}
int GObject::RemoveFromParent( bool bRelease )
{
	_CallTreeWillChange();
	GObject * _pParent = pParent;
	int ret = _RemoveFromParent(bRelease);
	_CallTreeChanged(_pParent, _pParent);
	return ret;
}

int GObject::_RemoveFromParent( bool bRelease )
{
	if (pParent)
	{
		return pParent->_RemoveChild(this, bRelease);
	}
	return -1;
}

int GObject::RemoveAllChildren( bool bRelease )
{
	// Do not use FOREACH
	_CallTreeWillChange();
	if (!lstChildren.empty())
	{
		for (list<GObject*>::iterator it=lstChildren.begin(); it!=lstChildren.end();)
		{
			it = _ActualRemoveChild(it, bRelease);
			/*
			if ((*it))
			{
				(*it)->OnRelease();
				it = children.erase(it);
			}
			*/
		}
	}
	CallModify();
	_CallTreeChanged(this, this);
//	pgm->OnTreeChanged(this, this);
	return 0;
}

void GObject::OnInit()
{
}

void GObject::OnEnter()
{
	ASSERT(pParent != NULL);

	if (!lsLineColorSet.IsColorSet())
	{
		GObject * pLayer = getLayer();
		if (pLayer)
		{
			setLineColor(pLayer->lsLineColorSet);
		}
		else
		{
			setLineColor(pParent->lsLineColorSet);
		}
	}
}

void GObject::CallRelease()
{
	OnRelease();
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			(*it)->CallRelease();
		}
	}
    pgm->AddNodeToDelete(this);
}

void GObject::OnRelease()
{
}

void GObject::CallRemove()
{
	OnRemove();
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			(*it)->CallRemove();
		}
	}
}

void GObject::OnRemove()
{

}

void GObject::CallIndepend()
{
	OnIndepend();
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			(*it)->CallIndepend();
		}
	}
}

void GObject::OnIndepend()
{

}

void GObject::OnPrecisionChanged(float fPrecision)
{
}

bool GObject::OnUpdate()
{
	if (nTryState)
	{
		if (nTryState == GOBJTRYSTATE_MOVE_REQUIREUPDATE)
		{
			nTryState = GOBJTRYSTATE_MOVE_AFTERUPDATE;
		}
		else if (nTryState == GOBJTRYSTATE_MOVE_AFTERUPDATE)
		{
			if (!pgm->IsTryMoving())
			{
				float tx = getX();
				float ty = getY();
				// Directly Move!!
				MoveTo(NULL, fTryMove_bx, fTryMove_by, false, nUpdateMoveActionID);
//				pgm->PushMoveNodeByOffsetForBatchCommand(this, tx-fTryMove_bx, ty-fTryMove_by);
				nTryState = GOBJTRYSTATE_MOVE_NULL;
			}
		}
	}
	return true;
}

void GObject::OnCancelTryMove()
{
	if (nTryState)
	{
		// Directly Move!!
		MoveTo(NULL, fTryMove_bx, fTryMove_by, false, nUpdateMoveActionID);
		nTryState = GOBJTRYSTATE_MOVE_NULL;
	}
}

void GObject::OnRender( int iHighlightLevel/*=0*/ )
{
	if (!isLayer() && strDisplayName.length())
	{
		DWORD col = getLineColor(iHighlightLevel);
		RenderHelper::getInstance().RenderString(strDisplayName.c_str(), getX(), getY(), _GOBJFONTSIZE, col);
	}
}

void GObject::OnModify()
{
	bModified = true;
	nModifyVersion++;
}

void GObject::OnClearModify()
{
	bModified = false;
}

void GObject::OnClone( bool bBegin )
{
}

int GObject::Reparent( GObject * newparent )
{
	/*
	ASSERT(newparent != NULL);
	_RemoveFromParent(false);
	return newparent->AddChild(this);
	*/
	return ReparentAfterObject(newparent, NULL);
}

int GObject::_ReparentAfterObject( GObject * newparent, GObject * afterobj )
{
	ASSERT(newparent != NULL);
	ASSERT(newparent != pParent);

	_CallTreeWillChange();

	GObject * _pParent = pParent;

	_RemoveFromParent(false);
	if (!isLayer())
	{
		clearLineColor();
	}

	int ret = newparent->AddChildAfterObj(this, afterobj);

	_CallTreeChanged(_pParent, _pParent);
	_CallTreeChanged(newparent, this);

	return ret;
}

int GObject::ReparentAfterObject( GObject * newparent, GObject * afterobj )
{
	return _ReparentAfterObject(newparent, afterobj);
}


int GObject::ReparentBeforeObject( GObject * newparent, GObject * beforeobj )
{
	ASSERT(newparent != NULL);
	ASSERT(newparent != pParent);
	GObject * pAfterObj = NULL;
	if (!beforeobj)
	{
		list<GObject *> * plChildren = newparent->getChildren();
		if (!plChildren->empty())
		{
			pAfterObj = plChildren->back();
		}
	}
	else
	{
		pAfterObj = beforeobj->getYoungerSibling();
	}
	if (pAfterObj == this)
	{
		pAfterObj = pAfterObj->getYoungerSibling();
	}
	return ReparentAfterObject(newparent, pAfterObj);
}

const char * GObject::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNObjectName();
}

void GObject::_ModifyNonAttributeChildrenCount( int countchange )
{
	nNonAttributeChildrenCount += countchange;
	ASSERT(nNonAttributeChildrenCount >= 0);
}

// Can only be called by UI
void GObject::setDisplayFold( bool toDisplayFold )
{
	if (bDisplayFolded != toDisplayFold)
	{
		bDisplayFolded = toDisplayFold;
//		pgm->OnTreeChanged(this);
	}
}

// Can only be called by UI
void GObject::setDisplayVisible( bool toDisplayVisible )
{
	bool changed = false;
	if (toDisplayVisible && (nDisplayState & GOBJ_DISPSTATE_INVISIBLE))
	{
		nDisplayState &= ~GOBJ_DISPSTATE_INVISIBLE;
		changed = true;
	}
	else if (!toDisplayVisible && !(nDisplayState & GOBJ_DISPSTATE_INVISIBLE))
	{
		nDisplayState |= GOBJ_DISPSTATE_INVISIBLE;
		changed = true;
	}

	if (changed)
	{
		if (!lstChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->OnParentToggleDisplayVisible(toDisplayVisible);
			}
		}
		CallRedrawModify();
	}
}

// Can only be called by UI
void GObject::setDisplayLock( bool toDisplayLock )
{
	bool changed = false;
	if (toDisplayLock && !(nDisplayState & GOBJ_DISPSTATE_LOCKED))
	{
		nDisplayState |= GOBJ_DISPSTATE_LOCKED;
		changed = true;
	}
	else if (!toDisplayLock && (nDisplayState & GOBJ_DISPSTATE_LOCKED))
	{
		nDisplayState &= ~GOBJ_DISPSTATE_LOCKED;
		changed = true;
	}

	if (changed)
	{
		if (!lstChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->OnParentToggleDisplayLocked(toDisplayLock);
			}
		}
	}
}

bool GObject::canRender()
{
	if (isDisplayVisible())
	{
		return true;
	}
	return false;
}

bool GObject::canUpdate()
{
	return true;
}

bool GObject::isDisplayVisible()
{
	if ((nDisplayState & GOBJ_DISPSTATE_INVISIBLE) || (nDisplayState & GOBJ_DISPSTATE_RECINVISIBLE))
	{
		return false;
	}
	return true;
}

bool GObject::isRecDisplayVisible()
{
	if (nDisplayState & GOBJ_DISPSTATE_RECINVISIBLE)
	{
		return false;
	}
	return true;
}

bool GObject::isDisplayLocked()
{
	if ((nDisplayState & GOBJ_DISPSTATE_LOCKED) || (nDisplayState & GOBJ_DISPSTATE_RECLOCKED))
	{
		return true;
	}
	return false;
}

bool GObject::isRecDisplayLocked()
{
	if (nDisplayState & GOBJ_DISPSTATE_RECLOCKED)
	{
		return true;
	}
	return false;
}

void GObject::setLineColor( LineColorSet ls )
{
	lsLineColorSet = ls;
	FOREACH_GOBJ_CHILDREN_IT()
	{
		(*it)->setLineColor(ls);
	}
}

void GObject::clearLineColor()
{
	lsLineColorSet.ClearSet();
	FOREACH_GOBJ_CHILDREN_IT()
	{
		(*it)->clearLineColor();
	}
}

void GObject::OnParentToggleDisplayVisible( bool toDisplayVisible )
{
	if (toDisplayVisible)
	{
		nDisplayState &= ~GOBJ_DISPSTATE_RECINVISIBLE;
	}
	else
	{
		nDisplayState |= GOBJ_DISPSTATE_RECINVISIBLE;
	}
	FOREACH_GOBJ_CHILDREN_IT()
	{
		(*it)->OnParentToggleDisplayVisible(toDisplayVisible);
	}
}

void GObject::OnParentToggleDisplayLocked( bool toDisplayLock )
{
	if (toDisplayLock)
	{
		nDisplayState |= GOBJ_DISPSTATE_RECLOCKED;
	}
	else
	{
		nDisplayState &= ~GOBJ_DISPSTATE_RECLOCKED;
	}
	FOREACH_GOBJ_CHILDREN_IT()
	{
		(*it)->OnParentToggleDisplayLocked(toDisplayLock);
	}

}

void GObject::CallCancelTryMove()
{
	OnCancelTryMove();
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			(*it)->CallCancelTryMove();
		}
	}
}

void GObject::CallRender( int iHighlightLevel/*=0*/ )
{
    if (/*iHighlightLevel || */canRender())
    {
        OnRender(iHighlightLevel);

		if (!lstChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->CallRender(iHighlightLevel);
			}
		}
	}
}

bool GObject::CallUpdate()
{
	if (canUpdate())
	{
		nUpdateMoveActionID = getBase()->nUpdateMoveActionID;
		if (OnUpdate())
		{
			if (!lstChildren.empty())
			{
				for(list<GObject *>::iterator it=lstChildren.begin(); it!=lstChildren.end();)
				{
					list<GObject *>::iterator itnext = it;
					++itnext;
					(*it)->CallUpdate();
					it = itnext;
				}
			}
			return true;
		}
	}
	return false;
}
void GObject::CallPrecisionChanged(float fPrecision)
{
    OnPrecisionChanged(fPrecision);
    if (!lstChildren.empty())
    {
        FOREACH_GOBJ_CHILDREN_IT()
        {
            (*it)->CallPrecisionChanged(fPrecision);
        }
    }
}

void GObject::CallModify()
{
    if (!bCloning)
    {
        OnModify();
        if (isModifyParent() && pParent)
        {
            pParent->CallModify();
        }
    }
}

void GObject::CallClearModify()
{
	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it))
			{
				(*it)->CallClearModify();
			}
		}
	}
    OnClearModify();
}

void GObject::CallRedrawModify()
{
    pgm->SetRedraw();
}

GLayer * GObject::getLayer( bool bIncludingSelf/*=true*/ )
{
	GObject * pobj = this;
	while (pobj)
	{
		if (pobj->isLayer())
		{
			if (bIncludingSelf || pobj != this)
			{
				return (GLayer *)pobj;
			}
		}
		pobj = pobj->pParent;
	}
	return NULL;
}

GObject * GObject::getBase()
{
	GObject * pobj = this;
	while (pobj->pParent)
	{
		pobj = pobj->pParent;
	}
	return pobj;
}

GObject * GObject::getNonAttributeParentObj()
{
	GObject * pobj = this;
	while (pobj)
	{
		if (!isAttributeNode())
		{
			return pobj;
		}
		pobj = pobj->pParent;
	}
	return NULL;
}

bool GObject::isRecDisplayFolded()
{
	GObject * pobj = this;
	if (bDisplayFolded)
	{
		return true;
	}
	while (pobj->pParent)
	{
		pobj = pobj->pParent;
		if (pobj->bDisplayFolded)
		{
			return true;
		}
	}
	return false;
}

void GObject::_CallTreeChanged( GObject * changebase, GObject * activenode )
{
	pgm->OnTreeChanged(changebase, activenode);
}

void GObject::_CallTreeWillChange()
{
	if (getBase() == pTreeBase)
	{
		pgm->OnTreeWillChange();
	}
}

int GObject::_CallResetID( int resetbase )
{
	if (getBase() == pTreeBase)
	{
		_SetID(resetbase);
		resetbase++;
		FOREACH_GOBJ_CHILDREN_IT()
		{
			resetbase = (*it)->_CallResetID(resetbase);
		}
	}
	return resetbase;
}

void GObject::CallResetID( int beginindex/*=0*/ )
{
	_CallResetID(beginindex);
}

GObject * GObject::FindNodeByID( int id )
{
	if (nID == id)
	{
		return this;
	}
	if (!lstChildren.empty())
	{
		if (lstChildren.front()->getID() > id)
		{
			return NULL;
		}
		FOREACHREV_GOBJ_CHILDREN_IT()
		{
			GObject * ret = (*it)->FindNodeByID(id);
			if (ret)
			{
				return ret;
			}
		}
	}
	return NULL;
}

GObject * GObject::getOlderSibling()
{
	if (!pParent)
	{
		return NULL;
	}
	return pParent->getOlderSiblingForChild(this);
}

GObject * GObject::getOlderSiblingForChild( GObject* child )
{
	ASSERT(child);
	FOREACH_GOBJ_CHILDREN_IT()
	{
		if ((*it) == child)
		{
			++it;
			if (it!=lstChildren.end())
			{
				return (*it);
			}
			break;
		}
	}
	return NULL;
}

GObject * GObject::getYoungerSibling()
{
	if (!pParent)
	{
		return NULL;
	}
	return pParent->getYoungerSiblingForChild(this);

}

GObject * GObject::getYoungerSiblingForChild( GObject* child )
{
	ASSERT(child);
	FOREACHREV_GOBJ_CHILDREN_IT()
	{
		if ((*it) == child)
		{
			++it;
			if (it!=lstChildren.rend())
			{
				return (*it);
			}
			break;
		}
	}
	return NULL;

}

bool GObject::isAncestorOf( GObject * pObj )
{
	if (!pObj)
	{
		return false;
	}
	GObject * pTemp = pObj;
	while (pTemp)
	{
		pTemp = pTemp->pParent;
		if (pTemp == this)
		{
			return true;
		}
	}
	return false;
}

bool GObject::isDescendantOf( GObject * pObj )
{
	if (!pObj)
	{
		return false;
	}
	return pObj->isAncestorOf(this);
}

DWORD GObject::getLineColor( int iHighlightLevel/*=0*/ )
{
	/*
	if (iHighlightLevel)
	{
		return ColorManager::getInstance().Highlight(dwLineColor, iHighlightLevel);
	}
	*/
	return lsLineColorSet.GetColor(iHighlightLevel);
}

void GObject::ToggleTryMoveState( bool bTry )
{
	if (!nTryState && bTry)
	{
		fTryMove_bx = getX();
		fTryMove_by = getY();
	}
	if (bTry)
	{
		nTryState = GOBJTRYSTATE_MOVE_REQUIREUPDATE;
	}
}

bool GObject::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	pClone->strDisplayName = strDisplayName;
	pClone->lsLineColorSet = lsLineColorSet;

	pClone->nMoveActionID = nMoveActionID;
	pClone->nUpdateMoveActionID = nUpdateMoveActionID;
	pClone->nModifyVersion = nModifyVersion;

	pClone->nTryState = nTryState;
	pClone->fTryMove_bx = fTryMove_bx;
	pClone->fTryMove_by = fTryMove_by;

	pClone->nDisplayState = nDisplayState;
	pClone->bDisplayFolded = bDisplayFolded;
	pClone->bModified = bModified;
	pClone->bCloning = true;

	//////////////////////////////////////////////////////////////////////////
	bool bIsManualCloning = pgm->IsManualCloning();
	if (bIsManualCloning)
	{
		pClone->strDisplayName = getDisplayName();
		pClone->strDisplayName += StringManager::getInstance().GetNodeCloneAppendName();
	}
	if (isLayer())
	{
		pgm->SetManualCloning(false);
	}
	for (list<GObject *>::iterator it=lstChildren.begin(); it!=lstChildren.end(); ++it)
	{
		(*it)->CreateNewClone(pClone);
	}
	if (isLayer())
	{
		pgm->SetManualCloning(bIsManualCloning);
	}

	return true;
}

bool GObject::CallRecCallBack( GObjRecCallBack cb, void * param )
{
	bool bRet = cb(this, param);

	if (!lstChildren.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			(*it)->CallRecCallBack(cb, param);
		}
	}

	return bRet;
}

void GObject::setDisplayName( const char * str )
{
	if (strlen(str))
	{
		if (strDisplayName != str)
		{
			strDisplayName=str;
			CallRedrawModify();
		}
	}

}
