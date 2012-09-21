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
#include "GNodeRelationship.h"
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
	/*
	if (!fLength)
	{
		return true;
	}
	*/
	float newx = fLength*cost(angle)+orix;
	float newy = fLength*sint(angle)+oriy;

	return CallMoveTo(pCaller, newx, newy, bTry, moveActionID);
}

bool GPoint::CallFlip( GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = GObjectManager::getInstance().GetNextMoveActionID(GMMATYPE_FLIP, angle);
	}
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptOri = PointF2D(orix, oriy);
	PointF2D ptThis = GetPointF2D();

	float px, py;
	pmh->FindPerpendicularPoint(ptThis.x, ptThis.y, orix, oriy, angle, &px, &py);

	float newx = 2*px-x;
	float newy = 2*py-y;

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
	super::OnRemove();
	DeclingToOther();
	DemergeFrom();
}

void GPoint::ClearClingTo()
{
	clInfo.ClearClingTo();
	/*
	pClingTo = NULL;
	fClingToProportion = 0.0f;
	*/
}

bool GPoint::ClingTo( GLine* pLine, float fVal, int nType/*=GCLING_PROPORTION*/ )
{
	if (!pLine)
	{
		DeclingToOther();
		return true;
	}

	GClingInfo ocli = clInfo;
	if (clInfo.GetClingTo())
	{
		clInfo.GetClingTo()->DeclingByOther(this);
	}
	if (!clInfo.SetClingTo(pLine, fVal, nType))
	{
		clInfo.SetClingTo(ocli.GetClingTo(), ocli.GetClingVal(), ocli.GetClingType());
		return false;
	}
	pLine->AddClingBy(this);
	return true;
}

void GPoint::DeclingToOther()
{
	GLine * pClingToLine = clInfo.GetClingTo();
	if (pClingToLine)
	{
		pClingToLine->DeclingByOther(this);
	}
	else
	{
		ClearClingTo();
	}
}

bool GPoint::isClingTo( GObject * pObj )
{
	return clInfo.isClingTo(pObj);
	/*
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
	*/
}

bool GPoint::MergeWith( GPoint * pPoint, bool bNoBackward/*=false*/ )
{
	if (!pPoint)
	{
		return DemergeFrom();
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

bool GPoint::DemergeFrom( GPoint * pPoint/*=NULL*/, bool bNoBackward/*=false*/ )
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
			DemergeFrom(mergeWithList.front());
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
				pAnotherPoint->DemergeFrom(this, true);
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
	ASSERT(clInfo.GetClingTo());
	PointF2D ptCling;
	float fProp;
	clInfo.GetClingPosition(&ptCling, NULL, NULL, &fProp);
	if (fProp > M_FLOATEXTREMEEPS && fProp < 1-M_FLOATEXTREMEEPS)
	{
		CallMoveTo(clInfo.GetClingTo(), ptCling.x, ptCling.y, bTry, moveActionID);
	}
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
			pPoint->ClingTo(clInfo);
			if (pPoint->canBeMergedWith() && this->canBeMergedWith())
			{
				pPoint->MergeWith(this);
			}
		}

		return true;
	}
	return false;
}

GNodeRelationshipGroup * GPoint::CreateRelationshipGroup( bool bClingBy/*=true*/, bool bOneWay/*=false*/ )
{
	if (!mergeWithList.empty() || clInfo.GetClingTo())
	{
		GNodeRelationshipGroup * pnrg = new GNodeRelationshipGroup(this);
		if (!mergeWithList.empty())
		{
			for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
			{
				GPoint * pOther = *it;
				if (!bOneWay || this<pOther)
				{
					GNodeRelMergeWith * pnrmerg = new GNodeRelMergeWith(mergeWithList.front());
					pnrg->AddRelationship(pnrmerg);
				}
			}
		}
		if (clInfo.GetClingTo())
		{
			GNodeRelClingTo * pnrclingto = new GNodeRelClingTo(clInfo);
			pnrg->AddRelationship(pnrclingto);
		}
		if (pnrg->GetRelList()->empty())
		{
			delete pnrg;
			return NULL;
		}
		return pnrg;
	}
	return NULL;
}

void GPoint::OnIndepend()
{
	super::OnIndepend();
	DemergeFrom();
}

/************************************************************************/
/* GANCHORPOINT                                                         */
/************************************************************************/

GAnchorPoint::GAnchorPoint()
{
	pHandle = NULL;
}

GAnchorPoint::GAnchorPoint(GObject * parent,  float _x, float _y )
{
	ASSERT(parent);

	SetPosition(_x, _y);
	pHandle = new GHandlePoint(this, _x, _y);

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
		if (fabsf(hx-pHandle->getX()) <= fAllowance && fabsf(hy-pHandle->getY()) <= fAllowance)
		{
			return;
		}
	}
	pHandle->CallMoveTo(pHandle, hx, hy, false);
}

bool GAnchorPoint::isHandleIdentical()
{
	if (fabsf(pHandle->getX()-x) < M_FLOATEPS && fabsf(pHandle->getY()-y) < M_FLOATEPS)
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
		if (pHandle)
		{
			int nMoveAngle;
			float fXVal;
			float fYVal;
			int nMoveType = GObjectManager::getInstance().GetMoveTypeInfo(&nMoveAngle, &fXVal, &fYVal);
			if (nMoveType == GMMATYPE_ROTATE)
			{
				if (pHandle->getMoveActionID() != moveActionID)
				{
					pHandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
					bret = pHandle->CallRotate(pCaller, x, y, nMoveAngle, bTry, 0);
					pHandle->CallMoveByOffset(pCaller, 0, 0, bTry, moveActionID);
				}
			}
			else if (nMoveType == GMMATYPE_FLIP)
			{
				if (pHandle->getMoveActionID() != moveActionID)
				{
					pHandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
					bret = pHandle->CallFlip(pCaller, x, y, nMoveAngle, bTry, 0);
					pHandle->CallMoveByOffset(pCaller, 0, 0, bTry, moveActionID);
				}
			}
			else if (nMoveType == GMMATYPE_SCALE)
			{
				if (pHandle->getMoveActionID() != moveActionID)
				{
					pHandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
					bret = pHandle->CallScale(pCaller, x, y, fXVal, fYVal, bTry, 0);
					pHandle->CallMoveByOffset(pCaller, 0, 0, bTry, moveActionID);
				}
			}
			else
			{
				bret = pHandle->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
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
		pAnchor->pHandle = (GHandlePoint *)*it;
		return true;
	}
	return false;
}
/************************************************************************/
/* GMIDPOINT                                                            */
/************************************************************************/

GMidPoint::GMidPoint()
{
//	setDisplayVisible(false);
//	setDisplayLock(true);
}

GMidPoint::GMidPoint( GObject * parent )
{
	ASSERT(parent);
//	setDisplayVisible(false);
//	setDisplayLock(true);
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
			RenderHelper * prh = &RenderHelper::getInstance();
			prh->RenderHandlePoint(x, y, col);

			int nSavedLineStyle = prh->getLineStyle();
			prh->SetLineStyle(getLine()->GetLineRenderStyle());
			RenderHelper::getInstance().RenderLine(x, y, pAnchor->getX(), pAnchor->getY(), col);
			prh->SetLineStyle(nSavedLineStyle);
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

GNodeRelationshipGroup * GHandlePoint::CreateRelationshipGroup( bool bClingBy/*=true*/, bool bOneWay/*=false*/ )
{
	if (pBindWith && (!bOneWay || this<pBindWith))
	{
		GNodeRelationshipGroup * pnrg = new GNodeRelationshipGroup(this);
		GNodeRelBindWith * pnrbind = new GNodeRelBindWith(pBindWith);
		pnrg->AddRelationship(pnrbind);
		return pnrg;
	}
	return NULL;
}

void GHandlePoint::OnIndepend()
{
	super::OnIndepend();
	BindWith();
}

bool GHandlePoint::isIdenticalToAnchor()
{
	return this->GetPointF2D().Equals(GetAnchor()->GetPointF2D());
}

/************************************************************************/
/* GClingInfo                                                           */
/************************************************************************/
bool GClingInfo::SetClingTo( GLine * pTo, float fVal, int nType/*=GCLING_PROPORTION*/ )
{

	if (pTo)
	{
		if (!pTo->canBeClingTo())
		{
			return false;
		}

		float fProp = 0;
		switch (nType)
		{
		case GCLING_PROPORTION:
			fProp = fVal;
			break;
		case GCLING_BEGINOFFSET:
			fProp = fVal/pTo->getLength();
			break;
		case GCLING_ENDOFFSET:
			fProp = 1.0f - fVal/pTo->getLength();
			break;
		default:
			ASSERT(false);
			return false;
		}
		if (fProp > 1-M_FLOATEXTREMEEPS || fProp < M_FLOATEXTREMEEPS)
		{
			return false;
		}
	}


	pClingTo = pTo;
	fClingVal = fVal;
	nClingType = nType;
	return true;
}

void GClingInfo::ClearClingTo()
{
	pClingTo = NULL;
	fClingVal = 0;
	nClingType = GCLING_PROPORTION;
}

bool GClingInfo::GetClingPosition( PointF2D * pptPos, int * isec/*=NULL*/, QuadBezierPointF2D * pQuadHandles/*=NULL*/, float * pProp/*=NULL*/ )
{
	if (!pClingTo)
	{
		return false;
	}

	float fProp = 0;
	bool bRetP = CalculateClingProportion(&fProp);
	if (pProp)
	{
		*pProp = fProp;
	}
	bool bRetC = pClingTo->GetPositionAtProportion(fProp, pptPos, isec, pQuadHandles);
	return bRetP && bRetC;
}

bool GClingInfo::CalculateClingProportion( float * pProp, float fLengthBase/*=-1*/ )
{
	if (!pClingTo)
	{
		return false;
	}

	float fProp = -1;
	if (fLengthBase < 0 && nClingType != GCLING_PROPORTION)
	{
		fLengthBase = pClingTo->getLength();
	}
	switch (nClingType)
	{
	case GCLING_PROPORTION:
		fProp = fClingVal;
		break;
	case GCLING_BEGINOFFSET:
		fProp = fClingVal/fLengthBase;
		break;
	case GCLING_ENDOFFSET:
		fProp = 1.0f-fClingVal/fLengthBase;
		break;
	}
	if (fProp < 0)
	{
		if (pProp)
		{
			*pProp = 0;
		}
		return false;
	}
	else if (fProp > 1)
	{
		if (pProp)
		{
			*pProp = 1;
		}
		return false;
	}
	if (pProp)
	{
		*pProp = fProp;
	}
	return true;
}

bool GClingInfo::isClingTo( GObject * pObj )
{
	if (!pObj)
	{
		return false;
	}
	if (pClingTo == pObj)
	{
		return true;
	}
	return false;
}

bool GClingInfo::ApplyChange( GLine * pLine, float fProportion )
{
	if (!pLine)
	{
		if (!pClingTo)
		{
			ClearClingTo();
			return true;
		}
		return false;
	}
	float fVal = fProportion;
	switch (nClingType)
	{
	case GCLING_PROPORTION:
		break;
	case GCLING_BEGINOFFSET:
		fVal = fProportion * pLine->getLength();
		break;
	case GCLING_ENDOFFSET:
		fVal = (1.0f-fProportion) * pLine->getLength();
	}
	if (!isClingTo(pLine) || fabs(fVal-fClingVal) >= M_FLOATEPS)
	{
		return SetClingTo(pLine, fVal, nClingType);
	}
	return false;
}

bool GClingInfo::ApplyTypeChange( int nType )
{
	if (!pClingTo)
	{
		return false;
	}
	if (nClingType == nType)
	{
		return false;
	}

	float fProportion;
	float fVal;
	if (CalculateClingProportion(&fProportion))
	{
		switch (nType)
		{
		case GCLING_PROPORTION:
			fVal = fProportion;
			break;
		case GCLING_BEGINOFFSET:
			fVal = fProportion * pClingTo->getLength();
			break;
		case GCLING_ENDOFFSET:
			fVal = (1.0f-fProportion) * pClingTo->getLength();
		}
		return SetClingTo(pClingTo, fVal, nType);
	}
	return false;
}