#include "StdAfx.h"
#include "GObjectPicker.h"
#include "GObjectManager.h"
#include "GUICoordinate.h"
#include "MarkingManager.h"
#include "MainInterface.h"


GObjectPicker::GObjectPicker(void)
{
	state = PICKSTATE_NULL;
	pfilterfunc = NULL;
	pbeforedownfilterfunc = NULL;
	pafterdownfilterfunc = NULL;
	SetSnapTo(GOPSNAP_GEOMETRY|GOPSNAP_COORD|GOPSNAP_GEOMETRYCOORD|GOPSNAP_CONTINUITY);
	bCheckMouseDown = false;
	bRenderMouseDown = false;
	bFilledPos =false;
	bTestMode = false;
	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pFakeLine[i] = new GBezierLine(GObjectManager::getInstance().GetFakeBaseNode(), PointF2D(0, 0), PointF2D(0, 0));
	}

	bLockXAxis = false;
	bLockYAxis = false;

	bLockLength = false;
	pLockLengthHandle = NULL;
	pLockLengthAnotherHandle = NULL;
	numLockAngles = 0;
	nCurrentLockAngleIndex = 0;
	pLockAngles = NULL;
	nSplitLockType = -1;

	pSplitMarking = NULL;
	bSplitMarkingInUse = false;

	pLockedLine = NULL;
}

GObjectPicker::~GObjectPicker(void)
{
	UnlockAngles();
}

void GObjectPicker::Init()
{
	state = PICKSTATE_NULL;
	SetSnapRange_S(15.0f);
}

int GObjectPicker::TestPickPoint( float x, float y, float *pfProportion/*=0*/, PickFilterCallback pbeforefunc/*=NULL*/, PickFilterCallback pafterfunc/*=NULL*/, float range_s/*=0*/ )
{
	state = PICKSTATE_NULL;

	PickPoint(pbeforefunc, pafterfunc);
	bFilledPos = true;
	bTestMode = true;
	filledX_C = x;
	filledY_C = y;

	this->snaptoflag = getInstance().snaptoflag;
	if (range_s >= 0)
	{
		SetSnapRange_S(range_s);
	}
	else
	{
		SetSnapRange_S(getInstance().snaprange_s);
	}

	ClearSet();
	UpdatePickPoint();
	int iret = PickPoint(pbeforefunc, pafterfunc);
	if (pfProportion)
	{
		*pfProportion = 0;
		if (IsPickReady(iret))
		{
			*pfProportion = CalculateProportion();
		}
	}
	return iret;
}


void GObjectPicker::FillInitialPosition()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();

	if (bFilledPos)
	{
		pickx_c = filledX_C;
		picky_c = filledY_C;
	}
	else
	{
		pickx_c = pguic->GetCursorX_C();
		picky_c = pguic->GetCursorY_C();
	}
}

void GObjectPicker::AdjustPositionToLocks()
{
	if (nSplitLockType >= 0 && pSplitMarking)
	{
		GLine * pLine = (GLine *)pSplitMarking->getTargetObj();
		PointF2D ptBegin = pLine->GetBeginPoint()->GetPointF2D();
		PointF2D ptEnd = pLine->GetEndPoint()->GetPointF2D();
		float fLength = 0;
		float fProp = 0;

		switch (nSplitLockType)
		{
		case MARKSPLITLINETYPE_BEGIN:
			fLength = pLine->getLength();
			fProp = fSplitValue/fLength;
			break;
		case MARKSPLITLINETYPE_BEGINPROP:
			fProp = fSplitValue;
			break;
		case MARKSPLITLINETYPE_END:
			fLength = pLine->getLength();
			fProp = (fLength-fSplitValue)/fLength;
			break;
		case MARKSPLITLINETYPE_ENDPROP:
			fProp = 1-fSplitValue;
			break;
		}
		int isec = 0;
		PointF2D ptSplit;
		pLine->GetPositionAtProportion(fProp, &ptSplit, &isec);
		pickx_c = ptSplit.x;
		picky_c = ptSplit.y;

		snappedstate |= GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
		pSplitMarking->SetSplitPoint(pickx_c, picky_c, isec);
		SetPickObj(pLine);

		bSplitMarkingInUse = true;
		pickSection[nOnLine] = isec;
		nOnLine = GOPONLINE_MAX;

	}
	if (pLockedLine)
	{
		if (nOnLine < GOPONLINE_MAX)
		{
			PointF2D ptPick;
			bool bNearTo = pLockedLine->CheckNearTo(pickx_c, picky_c, snaprange_c, &ptPick.x, &ptPick.y, &pickSection[nOnLine]);
			if (!bNearTo)
			{
				pLockedLine->GetPositionAtProportion(fLockLineDefaultProportion, &ptPick, &pickSection[nOnLine]);
			}
			pickx_c = ptPick.x;
			picky_c = ptPick.y;

			if (ptPick.StrictEquals(pLockedLine->GetBeginPoint()->GetPointF2D()))
			{
				snappedstate |= GOPSNAPPED_POINT|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
				SetPickObj(pLockedLine->GetBeginPoint());
				nOnLine = GOPONLINE_MAX;
			}
			else if (ptPick.StrictEquals(pLockedLine->GetEndPoint()->GetPointF2D()))
			{
				snappedstate |= GOPSNAPPED_POINT|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
				SetPickObj(pLockedLine->GetEndPoint());
				nOnLine = GOPONLINE_MAX;
			}

			if (!nOnLine)
			{
				AddSplitUI(pLockedLine);
			}

			if (nOnLine < GOPONLINE_MAX)
			{
				snappedstate |= GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
				SetPickObj(pLockedLine);
				nOnLine++;
				CheckSnapGeometryLine_ClingBy(pLockedLine);
			}
		}
	}
	if (bLockXAxis)
	{
		if (nOnLine < GOPONLINE_MAX)
		{
			picky_c = fLockXAxis_y;
			snappedstate |= GOPSNAPPED_XAXIS;
			nOnLine++;
		}
	}
	if (bLockYAxis)
	{
		if (nOnLine < GOPONLINE_MAX)
		{
			pickx_c = fLockYAxis_x;
			snappedstate |= GOPSNAPPED_YAXIS;
			nOnLine++;
		}
	}
	if (bLockLength)
	{
		if (nOnLine < GOPONLINE_MAX)
		{
			if (pLockLengthHandle)
			{
				GLine * pLine = pLockLengthHandle->getLine();
				float tx = pickx_c;
				float ty = picky_c;
				PointF2D ptLockAnchor = pLockLengthHandle->GetAnchor()->GetPointF2D();
				if (bLockLengthHandleInvert)
				{
					tx = 2*ptLockAnchor.x-tx;
					ty = 2*ptLockAnchor.y-ty;
				}
				bool bRet = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength(
					fLockLength, pLockLengthAnotherHandle->GetAnchor()->GetPointF2D(), pLockLengthAnotherHandle->GetPointF2D(),
					ptLockAnchor,
					tx, ty, &tx, &ty);
				if (bRet)
				{
					pickx_c = tx;
					picky_c = ty;
					if (bLockLengthHandleInvert)
					{
						pickx_c = 2*ptLockAnchor.x-pickx_c;
						picky_c = 2*ptLockAnchor.y-picky_c;
					}

					snappedstate |= GOPSNAPPED_LENGTHLOCK;
					nOnLine++;
				}
				else
				{
//					DASSERT(false);
				}
			}
			else
			{
				int angle = MathHelper::getInstance().GetLineAngle(PointF2D(fLockOriginX_c, fLockOriginY_c), PointF2D(pickx_c, picky_c));
				pickx_c = fLockOriginX_c+fLockLength*cost(angle);
				picky_c = fLockOriginY_c+fLockLength*sint(angle);

				snappedstate |= GOPSNAPPED_LENGTHLOCK;
				nOnLine++;
			}
		}
	}
	if (numLockAngles)
	{
		if (nOnLine < GOPONLINE_MAX)
		{
			int angle = MathHelper::getInstance().GetLineAngle(PointF2D(fLockOriginX_c, fLockOriginY_c), PointF2D(pickx_c, picky_c));
			// Angle: [-pi, pi];

			nCurrentLockAngleIndex = -1;
			if (numLockAngles == 1)
			{
				nCurrentLockAngleIndex = 0;
			}
			else
			{
				int tangle = pLockAngles[numLockAngles-1]-ANGLEBASE_360;
				for (int i=0; i<numLockAngles-2; i++)
				{
					if (IsAngleInBetween(angle, tangle, pLockAngles[i], pLockAngles[i+1]))
					{
						nCurrentLockAngleIndex = i;
						break;
					}
					tangle = pLockAngles[i];
				}
				if (nCurrentLockAngleIndex < 0)
				{
					if (IsAngleInBetween(angle, pLockAngles[numLockAngles-2], pLockAngles[numLockAngles-1], pLockAngles[0]+ANGLEBASE_360))
					{
						nCurrentLockAngleIndex = numLockAngles-2;
					}
					else
					{
						nCurrentLockAngleIndex = numLockAngles-1;
					}
				}
			}

			if (nCurrentLockAngleIndex >= 0)
			{
				snappedstate |= GOPSNAPPED_ANGLESLOCK;
				nOnLine++;

				ptCurrentLockAngleDir = PointF2D(pLockAngles[nCurrentLockAngleIndex]);
			}

			if (angle != pLockAngles[nCurrentLockAngleIndex])
			{
				if (bLockLength)
				{
					pickx_c = fLockOriginX_c+fLockLength*ptCurrentLockAngleDir.x;//cost(pLockAngles[nCurrentLockAngleIndex]);
					picky_c = fLockOriginY_c+fLockLength*ptCurrentLockAngleDir.y;//sint(pLockAngles[nCurrentLockAngleIndex]);
				}
				else
				{
					float tx;
					float ty;
					if (MathHelper::getInstance().FindPerpendicularPoint(pickx_c, picky_c, fLockOriginX_c, fLockOriginY_c, pLockAngles[nCurrentLockAngleIndex], &tx, &ty))
					{
						pickx_c = tx;
						picky_c = ty;
					}
				}
			}
		}
	}
}

bool GObjectPicker::IsInSnapRangePoint_C( float x, float y )
{
	return MathHelper::getInstance().PointInCircle(pickx_c, picky_c, x, y, snaprange_c);
}

bool GObjectPicker::IsInSnapRangeXAxis_C( float y )
{
	return fabsf(picky_c-y) < snaprange_c;
}

bool GObjectPicker::IsInSnapRangeAngle_C( float x, float y, int angle, float *plx, float *ply )
{
	MathHelper * pmh = &MathHelper::getInstance();

	float lx, ly;
	float x1, y1, x2, y2;
	bool bIntersect = false;
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	bIntersect = pmh->GetLineSegmentInRect(x, y, angle, pguic->StoCx(0), pguic->StoCy(0), pguic->StoCx(pguic->GetScreenWidth_S()), pguic->StoCy(pguic->GetScreenHeight_S()), &x1, &y1, &x2, &y2);

	bool bret = pmh->PointNearToStraightLine(pickx_c, picky_c, x1, y1, x2, y2, snaprange_c, &lx, &ly);
	if (bret)
	{
		if (plx)
		{
			*plx = lx;
		}
		if (ply)
		{
			*ply = ly;
		}
	}
	return bret;
}

bool GObjectPicker::IsInSnapRangeYAxis_C( float x )
{
	return fabsf(pickx_c-x) < snaprange_c;
}

float GObjectPicker::GetPickX_S()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	return pguic->CtoSx(pickx_c);
}

float GObjectPicker::GetPickY_S()
{
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	return pguic->CtoSy(picky_c);
}


GObject * GObjectPicker::GetPickedObj(int index/*=0*/)
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickObj[index];
	}
	return NULL;
}

GObject * GObjectPicker::GetPickedEntityObj(int index/*=0*/)
{
	if (snappedstate & GOPSNAPPED_OBJ)
	{
		return pickEntityObj[index];
	}
	return NULL;
}

/*
GObject * GObjectPicker::GetMDownPickedObj()
{
	if (mousestate != GOPMOUSE_NONE)
	{
		return mousedownPickObj;
	}
	return NULL;
}

GObject * GObjectPicker::GetMDownPickedEntityObj()
{
	if (mousestate != GOPMOUSE_NONE)
	{
		return mousedownPickEntityObj;
	}
    return NULL;
}
*/
void GObjectPicker::SetSnapTo(int snapto, bool bSet)
{
    if (bSet)
    {
        snaptoflag |= snapto;
    }
    else
    {
        snaptoflag &= ~snapto;
    }
}

void GObjectPicker::ClearSet()
{
	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pickObj[i] = NULL;
		pickEntityObj[i] = NULL;
		pickCoordEntityObj[i] = NULL;
	}

//	mousedownPickObj = NULL;
//	mousedownPickEntityObj = NULL;
}

void GObjectPicker::OnDeleteNode( GObject * pDeletedObj )
{
#define _NULLIFYNODE_IF_DELETED(NODE, DELETEDNODE)	\
	if (NODE == DELETEDNODE)	\
	{	\
		NODE = NULL;	\
	}

	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		_NULLIFYNODE_IF_DELETED(pickObj[i], pDeletedObj);
		_NULLIFYNODE_IF_DELETED(pickEntityObj[i], pDeletedObj);
		_NULLIFYNODE_IF_DELETED(pickCoordEntityObj[i], pDeletedObj);
	}
	if (pSplitMarking)
	{
		if (pDeletedObj == pSplitMarking->getTargetObj())
		{
			ClearSplitMarking();
		}
	}
	UnlockLockLine();
	/*
	UnlockAngles();
	UnlockLength();
	UnlockLockLine();
	UnlockSplitLine();
	UnlockXAxis();
	UnlockYAxis();
	*/
//	_NULLIFYNODE_IF_DELETED(mousedownPickObj, node);
//	_NULLIFYNODE_IF_DELETED(mousedownPickEntityObj, node);
}

bool GObjectPicker::IsPickReady( int iret/*=-1*/ )
{
	if (iret < 0)
	{
		return state & PICKSTATE_READY;
	}
	return iret & PICKSTATE_READY;
}

bool GObjectPicker::IsMouseDownReady()
{
	return mousestate & GOPMOUSE_DOWN;
}

void GObjectPicker::PushInterestPoint( float x, float y, bool bHasAngle/*=false*/, int angle/*=0*/ )
{
	PickerInterestPointInfo _info;
	_info.SetPosition(x, y);
	if (bHasAngle)
	{
		_info.SetAngle(angle);
	}
	for (list<PickerInterestPointInfo>::iterator it=pipinfo.begin(); it!=pipinfo.end(); ++it)
	{
		if ((*it).Equals(_info))
		{
			return;
		}
	}
	pipinfo.push_back(_info);
}

void GObjectPicker::SetPickObj( GObject * pObj )
{
	ASSERT(nPickObj < GOPONLINE_MAX);
	pickObj[nPickObj] = pObj;
	pickEntityObj[nPickObj] = pObj->getParent();
	if (pickEntityObj[nPickObj]->isLayer())
	{
		pickEntityObj[nPickObj] = pObj;
	}
	nPickObj++;
}

void GObjectPicker::SetPickPIP( PickerInterestPointInfo info, bool bPerp )
{
	ASSERT(nPickPIP < GOPONLINE_MAX);
	pickPIP[nPickPIP] = info;
	pickPIP[nPickPIP].SetPerpendicular(bPerp);
	nPickPIP++;
}

void GObjectPicker::SetPickObjCoord( GObject * pObj )
{
	ASSERT(nOnLine < GOPONLINE_MAX);
	pickCoordEntityObj[nOnLine] = pObj->getParent();
	if (pickCoordEntityObj[nOnLine]->isLayer())
	{
		pickCoordEntityObj[nOnLine] = pObj;
	}
	// No increment
}


void GObjectPicker::TraslateLineToStraightLine( GLine * pLine, int index, int isec )
{
	if (pLine->isStraightLine())
	{
		pFakeLine[index]->SetBeginEnd(pLine->GetBeginPoint()->getX(), pLine->GetBeginPoint()->getY(), pLine->GetEndPoint()->getX(), pLine->GetEndPoint()->getY());
	}
	else
	{
		GBezierLine * pBLine = (GBezierLine *)pLine;
		BezierSublinesInfo * pBSInfo = pBLine->getBSInfo();
		if (isec > pBSInfo->GetSubPointsCount()-2)
		{
			isec = pBSInfo->GetSubPointsCount()-2;
		}
		pFakeLine[index]->SetBeginEnd(pBSInfo->GetX(isec), pBSInfo->GetY(isec), pBSInfo->GetX(isec+1), pBSInfo->GetY(isec+1));
	}
}

void GObjectPicker::TraslatePIPToStraightLine( PickerInterestPointInfo * pPIP, int index, float fLength/*=-1 */ )
{
	int angle = pPIP->GetAngle();
	if (fLength < 0)
	{
		fLength = snaprange_c;
	}
	float fcos = cost(angle) * fLength;
	float fsin = sint(angle) * fLength;
	float xbase = pPIP->GetX();
	float ybase = pPIP->GetY();

	pFakeLine[index]->SetBeginEnd(xbase-fcos, ybase-fsin, xbase+fcos, ybase+fsin);
}

float GObjectPicker::CalculateProportion( int index/*=0 */ )
{
	GObject * pObj = GetPickedObj(index);
	if (!pObj)
	{
		return 0;
	}
	if (pObj->isMidPoint())
	{
		return pObj->getLine()->CalculateMidPointProportion();
	}
	if (!pObj->isLine())
	{
		return 0;
	}
	GLine * pLine = (GLine *)pObj;
	return pLine->CalculateProportion(pickx_c, picky_c, pickSection[index]);
}

void GObjectPicker::SetLockOrigin( float x, float y )
{
	fLockOriginX_c = x;
	fLockOriginY_c = y;
}

void GObjectPicker::SetLockLength( float fLength, GHandlePoint * pHandle/*=NULL*/, bool bInvert/*=false*/ )
{
	ASSERT(fLength >= 0);
	if (!fLength)
	{
		UnlockLength();
		return;
	}
	fLockLength = fLength;
	pLockLengthHandle = pHandle;
	if (pLockLengthHandle)
	{
		GLine * pLine = pLockLengthHandle->getLine();
		GHandlePoint * pBeginHandle = pLine->GetBeginPoint()->GetHandle();
		GHandlePoint * pEndHandle = pLine->GetEndPoint()->GetHandle();
		pLockLengthAnotherHandle = pLockLengthHandle==pBeginHandle?pEndHandle:pBeginHandle;
		bLockLengthHandleInvert = bInvert;
	}
	bLockLength = true;
}

void GObjectPicker::SetLockAngles( int nLocks, int * pAngles )
{
	ASSERT(nLocks >= 0);
	UnlockAngles();
	if (!nLocks)
	{
		return;
	}

	ASSERT(pAngles);
	numLockAngles = nLocks;

	size_t s = sizeof(int)*numLockAngles;
	pLockAngles = (int *)malloc(s);
	memcpy_s(pLockAngles, s, pAngles, s);
}

void GObjectPicker::UnlockLength()
{
	bLockLength = false;
	pLockLengthHandle = NULL;
	pLockLengthAnotherHandle = NULL;
}

void GObjectPicker::UnlockAngles()
{
	if (pLockAngles)
	{
		free(pLockAngles);
		pLockAngles = NULL;
	}
	numLockAngles = 0;
	nCurrentLockAngleIndex = 0;
}

void GObjectPicker::SetLockXAxis( float y )
{
	fLockXAxis_y = y;
	bLockXAxis = true;
}

void GObjectPicker::SetLockYAxis( float x )
{
	fLockYAxis_x = x;
	bLockYAxis = true;
}

void GObjectPicker::UnlockXAxis()
{
	bLockXAxis = false;
}

void GObjectPicker::UnlockYAxis()
{
	bLockYAxis = false;
}

void GObjectPicker::SetLockSplitLine( int splitlocktype, float fval )
{
	ASSERT(splitlocktype >= MARKSPLITLINETYPE_BEGIN && splitlocktype <= MARKSPLITLINETYPE_ENDPROP);
	nSplitLockType = splitlocktype;
	fSplitValue = fval;
}

void GObjectPicker::UnlockSplitLine()
{
	if (pSplitMarking)
	{
		pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_B)->SetLock(false);
		pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_E)->SetLock(false);
	}
	nSplitLockType = -1;
}

void GObjectPicker::SetLockLockLine( GObject * pObj, float fDefaultProportion )
{
	if (!pObj)
	{
		return;
	}
	if (pObj->isMidPoint())
	{
		pObj = pObj->getLine();
	}
	if (pObj->isRepresentableLine())
	{
		ASSERT(fDefaultProportion >= 0.0f && fDefaultProportion <= 1.0f);
		pLockedLine = (GLine *)pObj;
		fLockLineDefaultProportion = fDefaultProportion;
	}
}

void GObjectPicker::UnlockLockLine()
{
	pLockedLine = NULL;
}

bool GObjectPicker::IsAngleInBetween( int angle, int nBeginAngle, int nMidAngle, int nNextAngle )
{
	return angle >= (nBeginAngle+nMidAngle)/2 && angle < (nMidAngle+nNextAngle)/2;
}


bool GObjectPicker::staticMIDCBSplit(MarkingUI * pmui, bool bAccept)
{
	return GObjectPicker::getInstance().MIDCBSplit(pmui, bAccept);
}

bool GObjectPicker::MIDCBSplit( MarkingUI * pmui, bool bAccept )
{
	int typeflag = pmui->getTypeFlag();
	bool bBegin = (typeflag==MARKFLAG_SPLITLENGTH_B);
	if (pmui->IsValueLocked())
	{
		if (bBegin)
		{
			pmui->getMarking()->getMarkingUI(MARKFLAG_SPLITLENGTH_E)->SetLock(false);
		}
		else
		{
			pmui->getMarking()->getMarkingUI(MARKFLAG_SPLITLENGTH_B)->SetLock(false);
		}
		QString str = pmui->getStr();
		bool bOk;
		if (str.endsWith("%"))
		{
			string s = str.toStdString();
			sscanf_s(s.c_str(), "%f%%", &fSplitValue);
			fSplitValue/=100.0f;
			bOk = true;
			nSplitLockType = bBegin?MARKSPLITLINETYPE_BEGINPROP:MARKSPLITLINETYPE_ENDPROP;
		}
		else
		{
			fSplitValue = str.toFloat(&bOk);
			nSplitLockType = bBegin?MARKSPLITLINETYPE_BEGIN:MARKSPLITLINETYPE_END;
		}
		if (!bOk)
		{
			UnlockSplitLine();
			return false;
		}
	}
	else
	{
		UnlockSplitLine();
	}
	return true;
}

void GObjectPicker::AddSplitUI( GObject * pObj )
{
	if (bTestMode)
	{
		return;
	}
	if (pSplitMarking)
	{
		if (pSplitMarking->getTargetObj() == pObj)
		{
		}
		else
		{
			ClearSplitMarking();
		}
	}
	if (!pSplitMarking)
	{
		pSplitMarking = new MarkingSplitLine((GLine *)pObj, MARKFLAG_SPLITLENGTH);
		MarkingUI * pmuiSplitB = pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_B);
		MarkingUI * pmuiSplitE = pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_E);
		pmuiSplitB->SetEditable(true);
		pmuiSplitE->SetEditable(true);
		pmuiSplitB->SetCallback(staticMIDCBSplit);
		pmuiSplitE->SetCallback(staticMIDCBSplit);
		MarkingManager::getInstance().EnableMarking(pSplitMarking);
	}
	pSplitMarking->SetSplitPoint(pickx_c, picky_c, pickSection[0]);
	bSplitMarkingInUse = true;
}

void GObjectPicker::ClearSplitMarking()
{
	if (pSplitMarking)
	{
		MarkingManager::getInstance().DisableMarking(pSplitMarking);
		pSplitMarking = NULL;
	}
	bSplitMarkingInUse = false;
}
bool PickerInterestPointInfo::Equals( PickerInterestPointInfo & r )
{
	PointF2D p1(x, y);
	PointF2D p2(r.x, r.y);
	if (p1.Equals(p2))
	{
		if (bHasAngle)
		{
			if (r.bHasAngle && angle == r.angle)
			{
				return true;
			}
		}
		else
		{
			if (!r.bHasAngle)
			{
				return true;
			}
		}
	}
	return false;
}
