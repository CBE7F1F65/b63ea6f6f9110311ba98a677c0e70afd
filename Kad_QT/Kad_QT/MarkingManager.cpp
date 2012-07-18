#include "StdAfx.h"
#include "MarkingManager.h"
#include "RenderTargetManager.h"
#include "RenderHelper.h"

MarkingManager::MarkingManager(void)
{
	bRedraw = false;
	tarMarking = NULL;
}


MarkingManager::~MarkingManager(void)
{
	DisableMarking();
}

bool MarkingManager::EnableMarking( MarkingObject * pMarking )
{
	ASSERT(pMarking);

	if (!lstMarkings.empty())
	{
		for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end(); ++it)
		{
			if (*it == pMarking)
			{
				return false;
			}
		}
	}

	lstMarkings.push_back(pMarking);
	return true;
}

bool MarkingManager::DisableMarking( MarkingObject * pMarking/*=NULL*/ )
{
	if (!pMarking)
	{
		while (!lstMarkings.empty())
		{
			DisableMarking(lstMarkings.front());
		}
		return true;
	}
	if (!lstMarkings.empty())
	{
		for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end(); ++it)
		{
			if (*it == pMarking)
			{
				delete pMarking;
				lstMarkings.erase(it);
				SetRedraw();
				return true;
			}
		}
	}
	DASSERT(true);
	return false;
}

MarkingObject * MarkingManager::FindMarking( GObject * pObj, int nMarkFlag/*=-1*/ )
{
	if (!lstMarkings.empty())
	{
		for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end(); ++it)
		{
			MarkingObject * pMarking = *it;
			if (pMarking->getTargetObj() == pObj)
			{
				if (nMarkFlag < 0 || nMarkFlag == pMarking->getMarkFlag())
				{
					return pMarking;
				}
			}
		}
	}
	return NULL;
}

void MarkingManager::Update()
{
	if (!lstMarkings.empty())
	{
		for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end(); ++it)
		{
			MarkingObject * pMarking = *it;
			pMarking->CallUpdate();
		}
	}

	bool bTarUpdated;
	HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_MEASURE, &bTarUpdated);
	if (bRedraw || bTarUpdated || tar!=tarMarking)
	{
		if (!lstMarkings.empty())
		{
			tarMarking = tar;
			RenderHelper::getInstance().BeginRenderTar(tar);
			for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end(); ++it)
			{
				MarkingObject * pMarking = *it;
				pMarking->CallRender();
			}
			RenderHelper::getInstance().EndRenderTar();
		}
		else
		{
			tarMarking = NULL;
		}
		bRedraw = false;
	}
}

void MarkingManager::Render()
{
	if (tarMarking)
	{
		RenderHelper::getInstance().TargetQuadRender_S(tarMarking, 0, 0, 0xffffffff);
	}
}

void MarkingManager::SetRedraw()
{
	bRedraw = true;
}

void MarkingManager::OnDeleteNode( GObject * pDeletedObj )
{
	if (!lstMarkings.empty())
	{
		for (list<MarkingObject *>::iterator it=lstMarkings.begin(); it!=lstMarkings.end();)
		{
			MarkingObject * pMarking = *it;
			if (pMarking->getTargetObj() == pDeletedObj)
			{
				delete pMarking;
				it = lstMarkings.erase(it);
				SetRedraw();
			}
			else
			{
				++it;
			}
		}
	}
}