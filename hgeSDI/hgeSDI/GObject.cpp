#include "StdAfx.h"
#include "GObject.h"

#include "GObjectManager.h"

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
		children.push_back(child);
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
	// Do not use FOREACH
	if (!children.empty())
	{
		for (list<GObject*>::iterator it=children.begin(); it!=children.end();)
		{
			if ((*it)->ID == _ID)
			{
				(*it)->OnRelease();
				it = children.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	return children.size();
}

int GObject::RemoveChild( GObject * child )
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
				child->OnRelease();
				it = children.erase(it);
				break;
			}
		}
	}
	return children.size();
}
int GObject::RemoveFromParent()
{
	if (parent)
	{
		return parent->RemoveChild(this);
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
	bModified = false;
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
	GObjectManager::getInstance().AddNotToDelete(this);
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

void GObject::SetMotifyParent( bool bToModify )
{
	bModifyParent = bToModify;
}