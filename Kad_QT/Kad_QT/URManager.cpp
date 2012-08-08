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

bool URManager::UnDo( GBaseNode * pBase, int step/*=1*/ )
{
	ASSERT(pBase);
	if (CanUnDo(step))
	{
		index-=step;
		pBase->RestoreBaseFrom(stack[index]->getBase());
		return true;
	}
	return false;
}

bool URManager::ReDo( GBaseNode * pBase, int step/*=1*/ )
{
	ASSERT(pBase);
	if (CanReDo(step))
	{
		index+=step;
		pBase->RestoreBaseFrom(stack[index]->getBase());
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

bool URManager::CanUnDo( int step/*=1*/ )
{
	if (step < 1)
	{
		return false;
	}
	return index >= step && index > 1;
}

bool URManager::CanReDo( int step/*=1*/ )
{
	if (step < 1)
	{
		return false;
	}
	return index < stack.size()-step;
}