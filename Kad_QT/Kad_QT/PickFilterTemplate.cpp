#include "StdAfx.h"
#include "PickFilterTemplate.h"



PickFilterTemplate::PickFilterTemplate(void)
{
	pfcb = NULL;
	bInUse = false;
}


PickFilterTemplate::~PickFilterTemplate(void)
{
}

PickFilterCallback PickFilterTemplate::Use()
{
	DASSERT(!bInUse);
	bInUse = true;
	return pfcb;
}

void PickFilterTemplate::Dispose()
{
//	DASSERT(bInUse);
	bInUse = false;
}

/************************************************************************/
/* SingleObj                                                            */
/************************************************************************/

bool PickFilterSingleObj::staticPickFilterCallback(GObject * pObj)
{
	return getInstance().FilterFunc(pObj);
}

PickFilterSingleObj::PickFilterSingleObj()
{
	pfcb = staticPickFilterCallback;
}

PickFilterSingleObj::~PickFilterSingleObj()
{

}

PickFilterCallback PickFilterSingleObj::Use( GObject * pObj, bool bRec/*=true*/ )
{
	pFilterObj = pObj;
	bRecFind = bRec;
	return PickFilterTemplate::Use();
}

bool PickFilterSingleObj::FilterFunc( GObject * pObj )
{
	if (pObj && pFilterObj)
	{
		if (pFilterObj == pObj || bRecFind && pFilterObj->isAncestorOf(pObj))
		{
			return false;
		}
	}
	return true;
}

/************************************************************************/
/* SinglePoint                                                          */
/************************************************************************/

bool PickFilterSinglePoint::staticPickFilterCallback(GObject * pObj)
{
	return getInstance().FilterFunc(pObj);
}

PickFilterSinglePoint::PickFilterSinglePoint()
{
	pfcb = staticPickFilterCallback;
}

PickFilterSinglePoint::~PickFilterSinglePoint()
{

}

PickFilterCallback PickFilterSinglePoint::Use( GPoint * pPoint, bool bEntity/*=true*/, bool bMerge/*=true*/, bool bRec/*=true*/ )
{
	pFilterPoint = pPoint;
	bEntityFind = bEntity;
	bMergeFind = bMerge;
	bRecFind = bRec;
	return PickFilterTemplate::Use();
}

bool PickFilterSinglePoint::FilterFunc( GObject * pObj )
{
	if (!pObj || !pFilterPoint)
	{
		return true;
	}
	GObject * pFilterEntity = pFilterPoint->getEntity();
	if (pFilterPoint == pObj || bRecFind && pFilterPoint->isAncestorOf(pObj) || bEntityFind && (pFilterEntity == pObj || pFilterEntity == pObj->getEntity()))
	{
		return false;
	}
	list <GPoint *> * lMerged = pFilterPoint->getMergeWith();
	if (!lMerged->empty())
	{
		for (list<GPoint *>::iterator it=lMerged->begin(); it!=lMerged->end(); ++it)
		{
			GObject * pItObj = *it;
			if (pItObj == pObj || bRecFind && pItObj->isAncestorOf(pObj) || bEntityFind && (pItObj->getEntity() == pObj || pItObj->getEntity() == pObj->getEntity()))
			{
				return false;
			}
		}
	}
	return true;
}

/************************************************************************/
/* OnLine                                                               */
/************************************************************************/

PickFilterOnLine::PickFilterOnLine()
{
	pfcb = staticPickFilterCallback;
}

PickFilterOnLine::~PickFilterOnLine()
{

}

PickFilterCallback PickFilterOnLine::Use()
{
	return PickFilterTemplate::Use();
}

bool PickFilterOnLine::staticPickFilterCallback( GObject * pObj )
{
	return getInstance().FilterFunc(pObj);
}

bool PickFilterOnLine::FilterFunc( GObject * pObj )
{
	if (!pObj->isLine() && !pObj->isMidPoint())
	{
		return true;
	}
	return false;
}
