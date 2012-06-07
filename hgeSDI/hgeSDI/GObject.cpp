#include "StdAfx.h"
#include "GObject.h"

#include "GObjectManager.h"
#include "StringManager.h"

GObject::GObject(void)
{
	pParent = NULL;
	bModified = false;
	bModifyParent = true;
	bAttributeNode = false;
	nNonAttributeChildrenCount = 0;

	bDisplayFolded = false;
	nDisplayState = GOBJ_DISPSTATE_NORMAL;
	dwLineColor = 0;

	_SetID();
	OnInit();
}

GObject::~GObject(void)
{
}

#define _FOREACH_L(T, IT, L)	\
	for (list<T>::iterator IT=L.begin(); IT!=L.end(); ++IT)
#define FOREACH_GOBJ_CHILDREN_IT()	\
	_FOREACH_L(GObject*, it, listChildren)

#define _FOREACHREV_L(T, IT, L)	\
	for (list<T>::reverse_iterator IT=L.rbegin(); IT!=L.rend(); ++IT)
#define FOREACHREV_GOBJ_CHILDREN_IT()	\
	_FOREACHREV_L(GObject*, it, listChildren)

int GObject::_ActualAddChildAfterObj( GObject * child, GObject * afterobj )
{
	if (child)
	{
		child->_SetParent(this);
		child->OnEnter();

/*
		if (child->nID < 0)
		{
			child->_SetID();
			int maxid = 0;
			FOREACH_GOBJ_CHILDREN_IT()
			{
				if ((*it)->nID > maxid)
				{
					maxid = (*it)->nID;
				}
			}
			child->_SetID(maxid+1);
		}*/

		child->CallModify();

		//
		if (!afterobj)
		{
			listChildren.push_front(child);
		}
		else
		{
			bool bdone = false;
			FOREACH_GOBJ_CHILDREN_IT()
			{
				if ((*it) == afterobj)
				{
					++it;
					if (it!=listChildren.end())
					{
						listChildren.insert(it, child);
						bdone = true;
					}
					break;
				}
			}
			if (!bdone)
			{
				listChildren.push_back(child);
			}
		}

		if (!child->bAttributeNode)
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
	if (!(*it)->bAttributeNode)
	{
		_ModifyNonAttributeChildrenCount(-1);
	}
	if (bRelease)
	{
		(*it)->OnRelease();
	}
	(*it)->pParent = NULL;
	it = listChildren.erase(it);

	// Add OnModify and OnTreeChanged after all operation done!!
//	OnModify();
//	GObjectManager::getInstance().OnTreeChanged(this);

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
	int ret = _RemoveChild(_ID, bRelease);
	_CallTreeChanged(this, this);
	return ret;
}

int GObject::RemoveChild( GObject * child, bool bRelease )
{
	int ret = _RemoveChild(child, bRelease);
	_CallTreeChanged(this, this);
	return ret;
}


int GObject::_RemoveChild( int _ID, bool bRelease )
{
	// Do not use FOREACH
	if (!listChildren.empty())
	{
		for (list<GObject*>::iterator it=listChildren.begin(); it!=listChildren.end();)
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
//	GObjectManager::getInstance().OnTreeChanged(this, this);
	return listChildren.size();
}

int GObject::_RemoveChild( GObject * child, bool bRelease )
{
	if (!child)
	{
		return -1;
	}
	if (!listChildren.empty())
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
//	GObjectManager::getInstance().OnTreeChanged(this, this);
	return listChildren.size();
}
int GObject::RemoveFromParent( bool bRelease )
{
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
	if (!listChildren.empty())
	{
		for (list<GObject*>::iterator it=listChildren.begin(); it!=listChildren.end();)
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
//	GObjectManager::getInstance().OnTreeChanged(this, this);
	return 0;
}

void GObject::OnInit()
{
}

void GObject::OnEnter()
{
	ASSERT(pParent != NULL);

	if (!dwLineColor)
	{
		GObject * pLayer = GetLayer();
		if (pLayer)
		{
			setLineColor(pLayer->dwLineColor);
		}
		else
		{
			setLineColor(pParent->dwLineColor);
		}
	}
}

void GObject::OnUpdate()
{
}

void GObject::OnRender()
{
}

void GObject::OnRelease()
{
	RemoveAllChildren(true);
	GObjectManager::getInstance().AddNodeToDelete(this);
}

void GObject::OnModify()
{
	bModified = true;
}

void GObject::OnClearModify()
{
	bModified = false;
}

void GObject::setMotifyParent( bool bToModify )
{
	bModifyParent = bToModify;
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

int GObject::ReparentAfterObject( GObject * newparent, GObject * afterobj )
{
	ASSERT(newparent != NULL);
	bool calltreechange = false;
	if (newparent->GetBase() != GetBase())
	{
		calltreechange = true;
	}
	GObject * _pParent = pParent;
	_RemoveFromParent(false);
	int ret = newparent->AddChildAfterObj(this, afterobj);
	if (calltreechange)
	{
		_CallTreeChanged(_pParent, _pParent);
	}
	return ret;
}

const char * GObject::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNObjectName();
}

void GObject::setIsAttributeNode( bool bToAttribute/*=true*/ )
{
	if (bToAttribute && !bAttributeNode)
	{
		_ModifyNonAttributeChildrenCount(1);
	}
	else if (!bToAttribute && bAttributeNode)
	{
		_ModifyNonAttributeChildrenCount(-1);
	}
	bAttributeNode = bToAttribute;
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
//		GObjectManager::getInstance().OnTreeChanged(this);
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
		if (!listChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->OnParentToggleDisplayVisible(toDisplayVisible);
			}
		}
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
		if (!listChildren.empty())
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

void GObject::setLineColor( DWORD col )
{
	dwLineColor = col;
	FOREACH_GOBJ_CHILDREN_IT()
	{
		(*it)->setLineColor(col);
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

void GObject::CallRender()
{
	if (canRender())
	{
		OnRender();
		if (!listChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->CallRender();
			}
		}
	}
}

void GObject::CallUpdate()
{
	if (canUpdate())
	{
		OnUpdate();
		if (!listChildren.empty())
		{
			FOREACH_GOBJ_CHILDREN_IT()
			{
				(*it)->CallUpdate();
			}
		}
	}
}

void GObject::CallModify()
{
	OnModify();
	if (bModifyParent && pParent)
	{
		pParent->CallModify();
	}
}

void GObject::CallClearModify()
{
	if (!listChildren.empty())
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

GObject * GObject::GetLayer()
{
	GObject * pobj = this;
	while (pobj)
	{
		if (pobj->isLayer())
		{
			return pobj;
		}
		pobj = pobj->pParent;
	}
	return NULL;
}

GObject * GObject::GetBase()
{
	GObject * pobj = this;
	while (pobj->pParent)
	{
		pobj = pobj->pParent;
	}
	return pobj;
}

bool GObject::isRecDisplayFolded()
{
	GObject * pobj = this;
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
	GObjectManager::getInstance().OnTreeChanged(changebase, activenode);
}

int GObject::_CallResetID( int resetbase )
{
	_SetID(resetbase);
	resetbase++;
	FOREACH_GOBJ_CHILDREN_IT()
	{
		resetbase = (*it)->_CallResetID(resetbase);
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
	if (!listChildren.empty())
	{
		if (listChildren.front()->getID() > id)
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
			if (it!=listChildren.end())
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
			if (it!=listChildren.rend())
			{
				return (*it);
			}
			break;
		}
	}
	return NULL;

}