#include "StdAfx.h"
#include "GPoint.h"
#include "StringManager.h"
#include "ColorManager.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "MainInterface.h"
#include "Command.h"
#include "GObjectManager.h"
#include "GLine.h"
/************************************************************************/
/* GPOINT                                                               */
/************************************************************************/

GPoint::GPoint()
{
	x = 0;
	y = 0;
	ClearClingTo();
}
GPoint::~GPoint()
{

}

void GPoint::SetPosition( float _x, float _y )
{
	x = _x;
	y = _y;
	CallModify();
}

const char * GPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNPointName();
}

bool GPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GPoint);
	_GOBJ_CLONE_POST();
}

bool GPoint::MoveTo( float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
{
	if (!canMove())
	{
		return false;
	}
	
	if (moveActionID >= 0)
	{
		if (nMoveActionID == moveActionID)
		{
			return false;
		}
	}
	nMoveActionID = moveActionID;
	/*
	if (fabsf(x-newx) < M_FLOATEPS && fabsf(y-newy) < M_FLOATEPS)
	{
		return false;
	}
	*/

	ToggleTryMoveState(bTry);
	x = newx;
	y = newy;
	
	if (!isAnchorPoint())
	{
		CallModify();
		if (isHandlePoint())
		{
			GLine * pLine = (GLine *)getLine();
			list<GPoint *> * pClingByList = pLine->getClingBy();
			if (!pClingByList->empty())
			{
				for (list<GPoint *>::iterator it=pClingByList->begin(); it!=pClingByList->end(); ++it)
				{
					(*it)->CallClingToMoved(false, moveActionID);
				}
			}
		}
	}

	return true;
}

bool GPoint::CallMoveTo( float newx, float newy, bool bTry, int moveActionID/*=-1*/ )
{
	if (!mergeWithList.empty())
	{
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
		{
			(*it)->MoveTo(newx, newy, bTry, moveActionID);
		}
	}
	return MoveTo(newx, newy, bTry, moveActionID);
}

void GPoint::OnRemove()
{
	DeclingToOther();
	SeperateFrom();
}

void GPoint::ClearClingTo()
{
	pClingTo = NULL;
	fClingToProportion = 0.0f;
}

bool GPoint::ClingTo( GObject * pObj, float fProp )
{
	if (!pObj)
	{
		DeclingToOther();
		return true;
	}
	if (!pObj->canBeClingTo())
	{
		return false;
	}
	if (isClingTo(pObj))
	{
		fClingToProportion = fProp;
		return true;
	}
	ASSERT(pObj->isLine());
	GLine * pLine = (GLine *)pObj;
	if (pLine->AddClingBy(this))
	{
		if (pClingTo)
		{
			DeclingToOther();
		}
		pClingTo = pObj;
		fClingToProportion = fProp;
		return true;
	}
	return false;
}

void GPoint::DeclingToOther()
{
	if (pClingTo)
	{
		((GLine *)pClingTo)->DeclingByOther(this);
	}
	else
	{
		ClearClingTo();
	}
}

bool GPoint::isClingTo( GObject * pObj )
{
	if (!pObj)
	{
		return false;
	}
	if (pClingTo == pObj)
	{
		return true;
	}
	if (pObj->getChildren()->empty())
	{
		return false;
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		if (isClingTo(*it))
		{
			return true;
		}
	}
	return false;
}

bool GPoint::MergeWith( GPoint * pPoint, bool bNoBackward/*=false*/ )
{
	DASSERT(pPoint != this);
	if (pPoint == this)
	{
		return false;
	}
	if (!canAttach() || !canBeMergedWith() || !pPoint->canAttach() || !pPoint->canBeMergedWith())
	{
		return false;
	}
	if (!mergeWithList.empty())
	{
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
		{
			if (*it == pPoint)
			{
				return true;
			}
		}
		if (!bNoBackward)
		{
			for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
			{
				GPoint * pSelfMerged = *it;
				for (list<GPoint *>::iterator jt=pPoint->getMergeWith()->begin(); jt!=pPoint->getMergeWith()->end(); ++jt)
				{
					GPoint * pOtherMerged = *jt;
					pSelfMerged->getMergeWith()->push_back(pOtherMerged);
					pOtherMerged->getMergeWith()->push_back(pSelfMerged);
				}
				pSelfMerged->getMergeWith()->push_back(pPoint);
				pPoint->getMergeWith()->push_back(pSelfMerged);
			}
		}
	}
	for (list<GPoint *>::iterator jt=pPoint->getMergeWith()->begin(); jt!=pPoint->getMergeWith()->end(); ++jt)
	{
		GPoint * pOtherMerged = *jt;
		mergeWithList.push_back(pOtherMerged);
		pOtherMerged->getMergeWith()->push_back(this);
	}
	mergeWithList.push_back(pPoint);
	pPoint->getMergeWith()->push_back(this);
	return true;
}

bool GPoint::isMergeWith( GPoint * pPoint )
{
	if (!pPoint || mergeWithList.empty())
	{
		return false;
	}
	for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
	{
		if (*it == pPoint)
		{
			return true;
		}
	}
	return false;
}

bool GPoint::SeperateFrom( GPoint * pPoint/*=NULL*/, bool bNoBackward/*=false*/ )
{
	DASSERT(pPoint!=this);
	if (pPoint == this)
	{
		return false;
	}
	if (!pPoint)
	{
		while (!mergeWithList.empty())
		{
			SeperateFrom(mergeWithList.front());
		}
		return true;
	}
	else
	{
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end();)
		{
			if (*it == pPoint)
			{
				GPoint * pAnotherPoint = *it;
				it = mergeWithList.erase(it);
				if (!bNoBackward)
				{
					return pAnotherPoint->SeperateFrom(this, true);
				}
				return true;
			}
			else
			{
				++it;
			}
		}
	}
	return false;
}

GObject * GPoint::getLine()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isLine())
		{
			return pObj;
		}
		pObj = pObj->getParent();
	}
	return NULL;
}

GObject * GPoint::getPiece()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isPiece())
		{
			return pObj;
		}
		pObj = pObj->getParent();
	}
	return NULL;
}

void GPoint::CallClingToMoved( bool bTry, int moveActionID )
{
	ASSERT(pClingTo);
	float tox, toy;
	((GLine *)pClingTo)->GetPositionAtProportion(fClingToProportion, &tox, &toy);
	CallMoveTo(tox, toy, bTry, moveActionID);
}

/************************************************************************/
/* GANCHORPOINT                                                         */
/************************************************************************/

GAnchorPoint::GAnchorPoint()
{
	phandle = NULL;
}

GAnchorPoint::GAnchorPoint(GObject * parent,  float _x, float _y )
{
	ASSERT(parent);

	SetPosition(_x, _y);
	phandle = new GHandlePoint(this, _x, _y);

	parent->AddChild(this);
	OnInit();
}

GAnchorPoint::~GAnchorPoint()
{
}

const char * GAnchorPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNAnchorPointName();
}

bool GAnchorPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GAnchorPoint);

    _node->phandle = NULL;

    _GOBJ_CLONE_POST_NORET();
	list<GObject *>::reverse_iterator it=_node->listChildren.rbegin();
	_node->phandle = (GHandlePoint *)*it;
    return true;
}

void GAnchorPoint::SetHandlePosition( float _x, float _y )
{
	phandle->CallMoveTo(_x, _y, false);
}

bool GAnchorPoint::isHandleIdentical()
{
	if (fabsf(phandle->getX()-x) < M_FLOATEPS && fabsf(phandle->getY()-y) < M_FLOATEPS)
	{
		return true;
	}
	return false;
}

bool GAnchorPoint::MoveTo( float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
{
	float xoffset = newx-x;
	float yoffset = newy-y;

	bool bret = GAttributePoint::MoveTo(newx, newy, bTry, moveActionID);

	if (bret)
	{
		if (phandle)
		{
			bret = phandle->CallMoveByOffset(xoffset, yoffset, bTry, moveActionID);

			if (bret)
			{
				GLine * pLine = (GLine *)getLine();
				list<GPoint *> * pClingByList = pLine->getClingBy();
				if (!pClingByList->empty())
				{
					for (list<GPoint *>::iterator it=pClingByList->begin(); it!=pClingByList->end(); ++it)
					{
						(*it)->CallClingToMoved(false, moveActionID);
					}
				}
			}

			return bret;
		}
		else
		{
			CallModify();
		}
	}
	return bret;

}
/************************************************************************/
/* GMIDPOINT                                                            */
/************************************************************************/

GMidPoint::GMidPoint()
{
	setDisplayVisible(false);
	setDisplayLock(true);
}

GMidPoint::GMidPoint( GObject * parent )
{
	ASSERT(parent);
	setDisplayVisible(false);
	setDisplayLock(true);
	parent->AddChild(this);
	OnInit();
}

GMidPoint::~GMidPoint()
{
}

const char * GMidPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNMidPointName();
}

bool GMidPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GMidPoint);
	_GOBJ_CLONE_POST();
}
/************************************************************************/
/* GAttributePoint                                                      */
/************************************************************************/
void GAttributePoint::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper::getInstance().RenderAttributePoint(x, y, col);
}
/************************************************************************/
/* GSubstantivePoint                                                    */
/************************************************************************/
void GSubstantivePoint::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper::getInstance().RenderSubstantivePoint(x, y, col);
}
/************************************************************************/
/* GHANDLEPOINT                                                         */
/************************************************************************/
GHandlePoint::GHandlePoint()
{

}

GHandlePoint::GHandlePoint(GObject * parent, float x, float y)
{
	ASSERT(parent);
	SetPosition(x, y);
	parent->AddChild(this);
	OnInit();
}

GHandlePoint::~GHandlePoint()
{
}

const char * GHandlePoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNHandlePointName();
}

bool GHandlePoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GHandlePoint);
	_GOBJ_CLONE_POST();
}

void GHandlePoint::OnRender( int iHighlightLevel/* =0 */ )
{
	GAnchorPoint * pAnchor = (GAnchorPoint*)GetAnchor();
	if (!pAnchor->isHandleIdentical())
	{
		if (GObjectManager::getInstance().isHandleVisible() || iHighlightLevel)
		{
			DWORD col = getLineColor(iHighlightLevel);
			RenderHelper::getInstance().RenderHandlePoint(x, y, col);
			RenderHelper::getInstance().RenderLine(x, y, pAnchor->getX(), pAnchor->getY(), col);
		}
	}
}
