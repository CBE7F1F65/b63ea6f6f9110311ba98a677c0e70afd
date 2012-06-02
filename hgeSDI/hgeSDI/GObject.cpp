#include "StdAfx.h"
#include "GObject.h"

#include "GObjectManager.h"
#include "StringManager.h"

GObject::GObject(void)
{
	parent = NULL;
	bModified = false;
	SetID();
	OnInit();
}


GObject::~GObject(void)
{
}

#define _FOREACH_L(T, IT, L)	\
	for (list<T>::iterator IT=L.begin(); IT!=L.end(); ++IT)
#define FOREACH_GOBJ_CHILDREN_IT()	\
	_FOREACH_L(GObject*, it, children)


void GObject::SetID( int _ID/*=-1*/ )
{
	ID = _ID;
}

int GObject::AddChild( GObject * child )
{
	if (child)
	{
		child->SetParent(this);
		child->OnEnter();

		if (child->ID < 0)
		{
			child->SetID();
			int maxid = 0;
			FOREACH_GOBJ_CHILDREN_IT()
			{
				if ((*it)->ID > maxid)
				{
					maxid = (*it)->ID;
				}
			}
			child->SetID(maxid+1);
		}

		child->OnModify();

		children.push_back(child);

		GObjectManager::getInstance().OnTreeChanged(this);

		return child->ID;
	}
	return -1;
}

void GObject::SetParent( GObject * _parent )
{
	assert(parent==NULL);
	parent = _parent;
	// Do not Add Children
}

int GObject::RemoveChild( int _ID )
{
	return _RemoveChild(_ID);
}

int GObject::RemoveChild( GObject * child )
{
	return _RemoveChild(child);
}

int GObject::_RemoveChild( int _ID, bool bRelease/*=true*/ )
{
	// Do not use FOREACH
	if (!children.empty())
	{
		for (list<GObject*>::iterator it=children.begin(); it!=children.end();)
		{
			if ((*it)->ID == _ID)
			{
				if (bRelease)
				{
					(*it)->OnRelease();
				}
				it = children.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	OnModify();
	GObjectManager::getInstance().OnTreeChanged(this);
	return children.size();
}

int GObject::_RemoveChild( GObject * child, bool bRelease/*=true*/ )
{
	if (!child)
	{
		return -1;
	}
	if (!children.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it) == child)
			{
				if (bRelease)
				{
					child->OnRelease();
				}
				it = children.erase(it);
				break;
			}
		}
	}
	OnModify();
	GObjectManager::getInstance().OnTreeChanged(this);
	return children.size();
}
int GObject::RemoveFromParent()
{
	return _RemoveFromParent();
}

int GObject::_RemoveFromParent( bool bRelease/*=true*/ )
{
	if (parent)
	{
		return parent->_RemoveChild(this, bRelease);
	}
	return -1;
}

int GObject::RemoveAllChildren()
{
	// Do not use FOREACH
	if (!children.empty())
	{
		for (list<GObject*>::iterator it=children.begin(); it!=children.end();)	
		{
			if ((*it))
			{
				(*it)->OnRelease();
				it = children.erase(it);
			}
		}
	}
	OnModify();
	GObjectManager::getInstance().OnTreeChanged(this);
	return 0;
}

void GObject::OnInit()
{

}

void GObject::OnEnter()
{

}

void GObject::OnUpdate()
{
	if (!children.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it))
			{
				(*it)->OnUpdate();
			}
		}
	}
//	bModified = false;
}

void GObject::OnRender()
{
	if (!children.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it))
			{
				(*it)->OnRender();
			}
		}
	}
}

void GObject::OnRelease()
{
	RemoveAllChildren();
	GObjectManager::getInstance().AddNodeToDelete(this);
}

void GObject::SortChildren()
{
	children.sort();
}

void GObject::OnModify()
{
	bModified = true;
	if (bModifyParent && parent)
	{
		parent->OnModify();
	}
}

void GObject::OnClearModify()
{
	if (!children.empty())
	{
		FOREACH_GOBJ_CHILDREN_IT()
		{
			if ((*it))
			{
				(*it)->OnClearModify();
			}
		}
	}
	bModified = false;
}

void GObject::SetMotifyParent( bool bToModify )
{
	bModifyParent = bToModify;
}

int GObject::Reparent( GObject * newparent )
{
	assert(newparent != NULL);
	_RemoveFromParent(false);
	return newparent->AddChild(this);
}

const char * GObject::GetTypeName()
{
	return StringManager::getInstance().GetNNObjectName();
}