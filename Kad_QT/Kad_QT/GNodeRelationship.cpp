#include "StdAfx.h"
#include "GNodeRelationship.h"


GNodeRelationship::GNodeRelationship(void)
{
	pOther = NULL;
	nRelType = GREL_NULL;
}


GNodeRelationship::~GNodeRelationship(void)
{
}
/************************************************************************/
/* Merge                                                                */
/************************************************************************/
GNodeRelMergeWith::GNodeRelMergeWith( /*GPoint * pPtSelf, */GPoint * pPtOther )
{
	nRelType = GREL_MERGEWITH;
	pOther = pPtOther;
	ASSERT(pOther);
}

bool GNodeRelMergeWith::CanAddRelTo( GObject * pObj )
{
	return pObj->canBeMergedWith();
}

bool GNodeRelMergeWith::RestoreTo( GObject * pObj, GObject * pFrom/*=NULL*/ )
{
	ASSERT(pObj);
	ASSERT(pObj->canBeMergedWith());
	GPoint * pPoint = (GPoint *)pObj;
	if (!pFrom)
	{
		pFrom = pOther;
	}
	pPoint->MergeWith((GPoint *)pFrom);
	return true;
}
/************************************************************************/
/* Bind                                                                 */
/************************************************************************/
GNodeRelBindWith::GNodeRelBindWith( /*GHandlePoint * pHdlSelf, */GHandlePoint * pHdlOther )
{
	nRelType = GREL_BINDWITH;
	pOther = pHdlOther;
	ASSERT(pOther);
}

bool GNodeRelBindWith::CanAddRelTo( GObject * pObj )
{
	return pObj->isHandlePoint();
}

bool GNodeRelBindWith::RestoreTo( GObject * pObj, GObject * pFrom/*=NULL*/ )
{
	ASSERT(pObj);
	ASSERT(pObj->isHandlePoint());
	GHandlePoint * pHandle = (GHandlePoint *)pObj;
	if (!pFrom)
	{
		pFrom = pOther;
	}
	pHandle->BindWith((GHandlePoint *)pFrom);
	return true;
}
/************************************************************************/
/* Cling To                                                             */
/************************************************************************/
GNodeRelClingTo::GNodeRelClingTo( /*GPoint * pPtSelf, */GLine * pLnOther, float fProp )
{
	nRelType = GREL_CLINGTO;
	pOther = pLnOther;
	ASSERT(pOther);
	fClingProp = fProp;
}

bool GNodeRelClingTo::CanAddRelTo( GObject * pObj )
{
	return pObj->canAttach();
}

bool GNodeRelClingTo::RestoreTo( GObject * pObj, GObject * pFrom/*=NULL*/ )
{
	ASSERT(pObj);
	ASSERT(pObj->canAttach());
	GPoint * pPoint = (GPoint *)pObj;
	if (!pFrom)
	{
		pFrom = pOther;
	}
	pPoint->ClingTo(pFrom, fClingProp);
	return true;
}
/************************************************************************/
/* Cling By                                                             */
/************************************************************************/
GNodeRelClingBy::GNodeRelClingBy( /*GLine * pLnSelf, */GPoint * pPtOther, float fProp )
{
	nRelType = GREL_CLINGBY;
	pOther = pPtOther;
	ASSERT(pOther);
	fClingProp = fProp;
}

bool GNodeRelClingBy::CanAddRelTo( GObject * pObj )
{
	return pObj->canBeClingTo();
}

bool GNodeRelClingBy::RestoreTo( GObject * pObj, GObject * pFrom/*=NULL*/ )
{
	ASSERT(pObj);
	ASSERT(pObj->canBeClingTo());
	if (!pFrom)
	{
		pFrom = pObj;
	}
	((GPoint *)pFrom)->ClingTo(pObj, fClingProp);
	return true;
}
/************************************************************************/
/* Group                                                                */
/************************************************************************/
GNodeRelationshipGroup::GNodeRelationshipGroup(GObject * pObj)
{
	pSelf = pObj;
}

GNodeRelationshipGroup::~GNodeRelationshipGroup()
{
	RemoveRelationship();
}

bool GNodeRelationshipGroup::AddRelationship( GNodeRelationship * pRel )
{
	ASSERT(pRel);

	if (pRel->CanAddRelTo(pSelf))
	{
		GObject * pOther = pRel->GetOther();
		ASSERT(pOther);
		for (list<GNodeRelationship *>::iterator it=lstRels.begin(); it!=lstRels.end(); ++it)
		{
			if ((*it)->GetOther() == pOther)
			{
				return true;
			}
		}
		lstRels.push_back(pRel);
		return true;
	}
	return false;
}

bool GNodeRelationshipGroup::RemoveRelationship( GObject * pOther/*=NULL*/ )
{
	if (!pOther)
	{
		while (!lstRels.empty())
		{
			RemoveRelationship(lstRels.front()->GetOther());
		}
	}
	if (!lstRels.empty())
	{
		for (list<GNodeRelationship *>::iterator it=lstRels.begin(); it!=lstRels.end(); ++it)
		{
			GNodeRelationship * pRel = *it;
			if (pRel->GetOther() == pOther)
			{
				delete pRel;
				lstRels.erase(it);
				return true;
			}
		}
	}
	return false;
}

bool GNodeRelationshipGroup::RestoreTo( GObject * pObj )
{
	for (list<GNodeRelationship *>::iterator it=lstRels.begin(); it!=lstRels.end(); ++it)
	{
		(*it)->RestoreTo(pObj);
	}
	return true;
}