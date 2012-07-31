#include "StdAfx.h"
#include "URManager.h"


URBaseNodeInfo::URBaseNodeInfo( GBaseNode * pBase )
{
	ASSERT(pBase);
	pBase->CopyBaseTo(&base);
}

URBaseNodeInfo::~URBaseNodeInfo()
{
	base.RemoveAllChildren(true);
}


URManager::URManager(void)
{
	index = 0;
}


URManager::~URManager(void)
{
	Release();
}

void URManager::Init( GBaseNode * pBase )
{
	Release();
	PushUnDo(pBase);
	index = 0;
}

void URManager::Release()
{
	for (vector<URBaseNodeInfo *>::iterator it=stack.begin(); it!=stack.end(); ++it)
	{
		delete *it;
	}
	stack.clear();
	index = 0;
}

int URManager::PushUnDo( GBaseNode * pBase )
{
	URBaseNodeInfo * _urnode = new URBaseNodeInfo(pBase);
	stack.push_back(_urnode);
	index++;
	return index-1;
}

bool URManager::UnDo( GBaseNode * pBase )
{
	ASSERT(pBase);
	if (CanUnDo())
	{
		index--;
		/*
		list<GObject *>dellist;
		for (list<GObject *>::iterator it=pBase->getChildren()->begin(); it!=pBase->getChildren()->end(); ++it)
		{
			dellist.push_back(*it);
		}
		*/
		pBase->RestoreBaseFrom(stack[index]->getBase());
		/*
		for (list<GObject *>::iterator it=dellist.begin(); it!=dellist.end(); ++it)
		{
			pBase->RemoveChild(*it, true);
		}
		*/
		return true;
	}
	return false;
}

bool URManager::ReDo( GBaseNode * pBase )
{
	ASSERT(pBase);
	if (CanReDo())
	{
		index++;
		/*
		list<GObject *>dellist;
		for (list<GObject *>::iterator it=pBase->getChildren()->begin(); it!=pBase->getChildren()->end(); ++it)
		{
			dellist.push_back(*it);
		}
		*/
		pBase->RestoreBaseFrom(stack[index]->getBase());
		/*
		for (list<GObject *>::iterator it=dellist.begin(); it!=dellist.end(); ++it)
		{
			pBase->RemoveChild(*it, true);
		}
		*/
		return true;
	}
	return false;
}

int URManager::ClearReDo()
{
	int ntodelete = (stack.size()-1)-index;
	for (int i=0; i<ntodelete; i++)
	{
		delete stack.back();
		stack.pop_back();
	}
	return ntodelete;
}

bool URManager::CanUnDo()
{
	return index > 0;
}

bool URManager::CanReDo()
{
	return index < stack.size()-1;
}