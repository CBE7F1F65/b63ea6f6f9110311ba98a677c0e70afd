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

GObject * GPoint::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GPoint);
	_GOBJ_CLONE_POST();
}

bool GPoint::MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
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

bool GPoint::CallMoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = GObjectManager::getInstance().GetNextMoveActionID(GMMATYPE_MOVE);
	}

	if (!mergeWithList.empty())
	{
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
		{
			(*it)->MoveTo(pCaller, newx, newy, bTry, moveActionID);
		}
	}
	return MoveTo(pCaller, newx, newy, bTry, moveActionID);
}

bool GPoint::CallRotate( GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = GObjectManager::getInstance().GetNextMoveActionID(GMMATYPE_ROTATE, angle);
	}
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptOri = PointF2D(orix, oriy);
	PointF2D ptThis = GetPointF2D();
	angle += pmh->GetLineAngle(ptOri, ptThis);
	float fLength = pmh->LineSegmentLength(ptOri, ptThis);

	if (!fLength)
	{
		return true;
	}
	float newx = fLength*cost(angle)+orix;
	float newy = fLength*sint(angle)+oriy;

	return CallMoveTo(pCaller, newx, newy, bTry, moveActionID);
}

bool GPoint::CallScale( GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = GObjectManager::getInstance().GetNextMoveActionID(GMMATYPE_SCALE, 0, fScaleX, fScaleY);
	}
	MathHelper * pmh = &MathHelper::getInstance();

	float newx = (x-orix)*fScaleX+orix;
	float newy = (y-oriy)*fScaleY+oriy;

	return CallMoveTo(pCaller, newx, newy, bTry, moveActionID);
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
		pClingTo = pLine;
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
	if (!pClingTo)
	{
		return false;
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
	if (!pPoint)
	{
		return SeperateFrom();
	}
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
//			ASSERT(false);
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
		if (!isMergeWith(pPoint))
		{
			return false;
		}
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end();)
		{
			GPoint * pAnotherPoint = *it;
			//////////////////////////////////////////////////////////////////////////
			if (this->isAnchorPoint() && pAnotherPoint->isAnchorPoint())
			{
				((GAnchorPoint *)this)->GetHandle()->UnbindTo(((GAnchorPoint *)pAnotherPoint)->GetHandle());
			}
			//////////////////////////////////////////////////////////////////////////
			if (!bNoBackward)
			{
				pAnotherPoint->SeperateFrom(this, true);
				++it;
			}
			else
			{
				if (pAnotherPoint == pPoint)
				{
					it = mergeWithList.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		if (!bNoBackward)
		{
			this->mergeWithList.clear();
		}
		return true;
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
	PointF2D ptCling;
	((GLine *)pClingTo)->GetPositionAtProportion(fClingToProportion, &ptCling);
	CallMoveTo(pClingTo, ptCling.x, ptCling.y, bTry, moveActionID);
}

bool GPoint::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent))
	{
		GPoint * pPoint = (GPoint *)pClone;
		pPoint->x = x;
		pPoint->y = y;

		if (!bNoRelationship)
		{
			pPoint->ClingTo(pClingTo, fClingToProportion);
			if (pPoint->canBeMergedWith() && this->canBeMergedWith())
			{
				pPoint->MergeWith(this);
			}
		}

		return true;
	}
	return false;
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

GObject * GAnchorPoint::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GAnchorPoint);
    _GOBJ_CLONE_POST();
}

void GAnchorPoint::SetHandlePosition( float hx, float hy, float fAllowance/*=-1 */ )
{
	if (fAllowance >= 0)
	{
		if (fabsf(hx-phandle->getX()) <= fAllowance && fabsf(hy-phandle->getY()) <= fAllowance)
		{
			return;
		}
	}
	phandle->CallMoveTo(phandle, hx, hy, false);
}

bool GAnchorPoint::isHandleIdentical()
{
	if (fabsf(phandle->getX()-x) < M_FLOATEPS && fabsf(phandle->getY()-y) < M_FLOATEPS)
	{
		return true;
	}
	return false;
}

bool GAnchorPoint::MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
{
	float xoffset = newx-x;
	float yoffset = newy-y;

	bool bret = super::MoveTo(pCaller, newx, newy, bTry, moveActionID);

	if (bret)
	{
		if (phandle)
		{
			int nMoveAngle;
			float fScaleX;
			float fScaleY;
			int nMoveType = GObjectManager::getInstance().GetMoveTypeInfo(&nMoveAngle, &fScaleX, &fScaleY);
			if (nMoveType == GMMATYPE_ROTATE)
			{
				if (phandle->getMoveActionID() != moveActionID)
				{
					phandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
					bret = phandle->CallRotate(pCaller, x, y, nMoveAngle, bTry, 0);
					phandle->CallMoveByOffset(pCaller, 0, 0, bTry, moveActionID);
				}
			}
			else if (nMoveType == GMMATYPE_SCALE)
			{
				if (phandle->getMoveActionID() != moveActionID)
				{
					phandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
					bret = phandle->CallScale(pCaller, x, y, fScaleX, fScaleY, bTry, 0);
					phandle->CallMoveByOffset(pCaller, 0, 0, bTry, moveActionID);
				}
			}
			else
			{
				bret = phandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
			}

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

bool GAnchorPoint::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent))
	{
		GAnchorPoint * pAnchor = (GAnchorPoint *)pClone;
		list<GObject *>::reverse_iterator it=pAnchor->lstChildren.rbegin();
		pAnchor->phandle = (GHandlePoint *)*it;
		return true;
	}
	return false;
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

GObject * GMidPoint::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
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

GObject * GHandlePoint::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
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

	if (getLine()->isStraightLine() || pHandle->getLine()->isStraightLine())
	{
		return false;
	}

    if (pBindWith)
	{
        BindWith();
	}

	pHandle->BindWith();

//    pHandle->CallMoveTo(2*GetAnchor()->getX()-x, 2*GetAnchor()->getY()-y, false);
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptAnchor = this->GetAnchor()->GetPointF2D();
	int angle = pmh->GetLineAngle(this->GetPointF2D(), ptAnchor);
	if (ptAnchor.Equals(pHandle->GetPointF2D()))
	{
		pHandle->CallMoveTo(pHandle, 2*ptAnchor.x-x, 2*ptAnchor.y-y, false);
	}
	else
	{
		float l = pmh->LineSegmentLength(pHandle->GetPointF2D(), ptAnchor);
		pHandle->CallMoveTo(pHandle, l*cost(angle)+ptAnchor.x, l*sint(angle)+ptAnchor.y, false);
	}

    pBindWith = pHandle;
    pHandle->pBindWith = this;


	return true;
}

bool GHandlePoint::MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1 */ )
{
	/*
	int nMoveAngle;
	float fScaleX;
	float fScaleY;
	int nMoveType = GObjectManager::getInstance().GetMoveTypeInfo(&nMoveAngle, &fScaleX, &fScaleY);
	if (nMoveType == GMMATYPE_ROTATE)
	{
		if (nMoveAngle)
		{
			MathHelper * pmh = &MathHelper::getInstance();
			PointF2D ptOri = GetAnchor()->GetPointF2D();
			PointF2D ptThis = GetPointF2D();
			nMoveAngle += pmh->GetLineAngle(ptOri, ptThis);
			float fLength = pmh->LineSegmentLength(ptOri, ptThis);

			if (!fLength)
			{
				return true;
			}
			newx = fLength*cost(nMoveAngle)+ptOri.x;
			newy = fLength*sint(nMoveAngle)+ptOri.y;
		}
	}
	else if (nMoveType == GMMATYPE_SCALE)
	{
		if (fScaleX!=1.0f || fScaleY!=1.0f)
		{
			MathHelper * pmh = &MathHelper::getInstance();
			PointF2D ptOri = GetAnchor()->GetPointF2D();
			PointF2D ptThis = GetPointF2D();
			int angle = pmh->GetLineAngle(ptOri, ptThis);
			float fLength = pmh->LineSegmentLength(ptOri, ptThis);

			if (!fLength)
			{
				return true;
			}

			fLength *= fScaleX;
			newx = fLength*cost(angle)+ptOri.x;
			newy = fLength*sint(angle)+ptOri.y;
		}
	}
	*/

	bool bret = super::MoveTo(pCaller, newx, newy, bTry, moveActionID);
	if (bret)
	{
		if (pCaller && pCaller->isHandlePoint())
		{
			if (pBindWith)
			{
				if (GetAnchor()->getMoveActionID() != moveActionID && pBindWith->GetAnchor()->getMoveActionID() != moveActionID)
				{
					MathHelper * pmh = &MathHelper::getInstance();
					PointF2D ptAnchor = this->GetAnchor()->GetPointF2D();
					int angle = pmh->GetLineAngle(this->GetPointF2D(), ptAnchor);
					float l = pmh->LineSegmentLength(pBindWith->GetPointF2D(), ptAnchor);
					pBindWith->CallMoveTo(pBindWith, l*cost(angle)+ptAnchor.x, l*sint(angle)+ptAnchor.y, false, moveActionID);
				}
				return true;
			}
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

bool GHandlePoint::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent))
	{
		return true;
	}
	return false;
}