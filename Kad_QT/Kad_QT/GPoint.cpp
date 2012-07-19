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
	
	if (x == newx && y == newy)
	{
		return true;
	}
	

	ToggleTryMoveState(bTry);
	x = newx;
	y = newy;
	
	if (!isAnchorPoint())
	{
		CallModify();
		if (isHandlePoint())
		{
			GLine * pLine = getLine();
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
					if (!pSelfMerged->isMergeWith(pOtherMerged))
					{
						pSelfMerged->getMergeWith()->push_back(pOtherMerged);
						if (!pOtherMerged->isMergeWith(pSelfMerged))
						{
							pOtherMerged->getMergeWith()->push_back(pSelfMerged);
						}
						else
						{
							ASSERT(false);
						}
					}
					else
					{
						ASSERT(false);
					}
				}
				if (!pSelfMerged->isMergeWith(pPoint))
				{
					pSelfMerged->getMergeWith()->push_back(pPoint);
					if (!pPoint->isMergeWith(pSelfMerged))
					{
						pPoint->getMergeWith()->push_back(pSelfMerged);
					}
					else
					{
						ASSERT(false);
					}
				}
				else
				{
					ASSERT(false);
				}
			}
		}
	}
	for (list<GPoint *>::iterator jt=pPoint->getMergeWith()->begin(); jt!=pPoint->getMergeWith()->end(); ++jt)
	{
		GPoint * pOtherMerged = *jt;
		if (!isMergeWith(pOtherMerged))
		{
			mergeWithList.push_back(pOtherMerged);
			if (!pOtherMerged->isMergeWith(this))
			{
				pOtherMerged->getMergeWith()->push_back(this);
			}
			else
			{
				ASSERT(false);
			}
		}
		else
		{
			ASSERT(false);
		}
	}
	if (!isMergeWith(pPoint))
	{
		mergeWithList.push_back(pPoint);
		if (!pPoint->isMergeWith(this))
		{
			pPoint->getMergeWith()->push_back(this);
		}
		else
		{
			ASSERT(false);
		}
	}
	else
	{
		ASSERT(false);
	}
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
				//////////////////////////////////////////////////////////////////////////
				if (this->isAnchorPoint() && pAnotherPoint->isAnchorPoint())
				{
					((GAnchorPoint *)this)->GetHandle()->UnbindTo(((GAnchorPoint *)pAnotherPoint)->GetHandle());
				}
				//////////////////////////////////////////////////////////////////////////
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

GLine * GPoint::getLine()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isLine())
		{
			return (GLine *)pObj;
		}
		pObj = pObj->getParent();
	}
	return NULL;
}

GPiece * GPoint::getPiece()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isPiece())
		{
			return (GPiece *)pObj;
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

void GAnchorPoint::SetHandlePosition( float hx, float hy, float fAllowance/*=-1 */ )
{
	if (fAllowance >= 0)
	{
		if (fabsf(hx-x) <= fAllowance && fabsf(hy-y) <= fAllowance)
		{
			return;
		}
	}
	phandle->CallMoveTo(hx, hy, false);
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
				GLine * pLine = getLine();
				list<GPoint *> * pClingByList = pLine->getClingBy();
				if (!pClingByList->empty())
				{
					for (list<GPoint *>::iterator it=pClingByList->begin(); it!=pClingByList->end(); ++it)
					{
						(*it)->CallClingToMoved(false, -1);
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
    pBindWith = NULL;
}

GHandlePoint::GHandlePoint(GObject * parent, float x, float y)
{
	ASSERT(parent);
    pBindWith = NULL;
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

bool GHandlePoint::BindWith( GHandlePoint * pHandle/*=NULL*/ )
{
	if (!pHandle)
	{
        if (pBindWith)
		{
            pBindWith->pBindWith = NULL;
            pBindWith = NULL;
		}
		return true;
	}
    if (isBindWith(pHandle))
	{
		return false;
	}
    if (pBindWith)
	{
        BindWith();
	}

	pHandle->BindWith();
    pHandle->CallMoveTo(2*GetAnchor()->getX()-x, 2*GetAnchor()->getY()-y, false);
    pBindWith = pHandle;
    pHandle->pBindWith = this;


	return true;
}

bool GHandlePoint::MoveTo( float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
{
	float xoffset = newx - x;
	float yoffset = newy - y;

	bool bret = GAttributePoint::MoveTo(newx, newy, bTry, moveActionID);
	if (bret)
	{
        if (pBindWith)
		{
            if (GetAnchor()->nMoveActionID != moveActionID && pBindWith->GetAnchor()->nMoveActionID != moveActionID)
			{
                pBindWith->CallMoveByOffset(-xoffset, -yoffset, false, moveActionID);
			}
			return true;
		}
	}
	return bret;
}

bool GHandlePoint::UnbindTo( GHandlePoint * pHandle )
{
	DASSERT(pHandle);
	if (!pHandle)
	{
		return false;
	}
    if (!isBindWith(pHandle))
	{
		return false;
	}
    BindWith();
    pHandle->BindWith();
	return true;
}
