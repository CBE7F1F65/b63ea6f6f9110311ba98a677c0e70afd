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
	SetSnapTo(GOPSNAP_GEOMETRY|GOPSNAP_COORD|GOPSNAP_GEOMETRYCOORD|GOPSNAP_CONTINUITY);
	bCheckMouseDown = false;
	bRenderMouseDown = false;
	bFilledPos =false;
	bTestMode = false;
	for (int i=0; i<GOPONLINE_MAX; i++)
	{
		pFakeLine[i] = new GBezierLine(&(GObjectManager::getInstance().fakebasenode), PointF2D(0, 0), PointF2D(0, 0));
	}

	bLockLength = false;
	numLockAngles = 0;
	nCurrentLockAngleIndex = 0;
	pLockAngles = NULL;
	nSplitLockType = -1;

	pSplitMarking = NULL;
	bSplitMarkingInUse = false;
}

GObjectPicker::~GObjectPicker(void)
{
	UnlockAngles();
}

void GObjectPicker::Init()
{
	state = PICKSTATE_NULL;
	SetSnapRange_S(25.0f);
}

int GObjectPicker::TestPickPoint( float x, float y, float *pfProportion/*=0*/, PickFilterCallback pfunc/*=NULL*/, float range_s/*=0*/ )
{
	state = PICKSTATE_NULL;

	PickPoint(pfunc);
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
	int iret = PickPoint(pfunc);
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
	MainInterface * pmain = &MainInterface::getInstance();
	if (pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_UP) || pmain->hge->Input_GetDIKey(DIK_ESCAPE))
	{
		ClearSplitMarking();
		UnLockSplitLine();
	}

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
		pLine->GetPositionAtProportion(fProp, &pickx_c, &picky_c, &isec);

		snappedstate |= GOPSNAPPED_LINE|GOPSNAPPED_OBJ|GOPSNAP_GEOMETRY;
		pSplitMarking->SetSplitPoint(pickx_c, picky_c, isec);
		SetPickObj(pLine);

		bSplitMarkingInUse = true;
		nOnLine = GOPONLINE_MAX;

	}
	else
	{
		if (bLockLength)
		{
			int angle = MathHelper::getInstance().GetLineAngle(PointF2D(lockOriginX_c, lockOriginY_c), PointF2D(pickx_c, picky_c));
			pickx_c = lockOriginX_c+fLockLength*cost(angle);
			picky_c = lockOriginY_c+fLockLength*sint(angle);

			snappedstate |= GOPSNAPPED_LENGTHLOCK;
			nOnLine++;
		}
		if (numLockAngles)
		{
			int angle = MathHelper::getInstance().GetLineAngle(PointF2D(lockOriginX_c, lockOriginY_c), PointF2D(pickx_c, picky_c));
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
					pickx_c = lockOriginX_c+fLockLength*ptCurrentLockAngleDir.x;//cost(pLockAngles[nCurrentLockAngleIndex]);
					picky_c = lockOriginY_c+fLockLength*ptCurrentLockAngleDir.y;//sint(pLockAngles[nCurrentLockAngleIndex]);
				}
				else
				{
					float tx;
					float ty;
					if (MathHelper::getInstance().FindPerpendicularPoint(pickx_c, picky_c, lockOriginX_c, lockOriginY_c, pLockAngles[nCurrentLockAngleIndex], &tx, &ty))
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
	bIntersect = pmh->GetLineSegmentInRect(x, y, angle, 0, 0, pguic->GetScreenWidth_C(), pguic->GetScreenWidth_C(), &x1, &y1, &x2, &y2);

	bool bret = pmh->PointNearToStraightLine(x, y, x1, y1, x2, y2, snaprange_c, &lx, &ly);
	if (plx)
	{
		*plx = lx;
	}
	if (ply)
	{
		*ply = ly;
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

void GObjectPicker::SetPickPIP( PickerInterestPointInfo info )
{
	ASSERT(nPickPIP < GOPONLINE_MAX);
	pickPIP[nPickPIP] = info;
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
		pFakeLine[index]->SetBeginEnd(pBLine->getBSInfo()->GetX(isec), pBLine->getBSInfo()->GetY(isec), pBLine->getBSInfo()->GetX(isec+1), pBLine->getBSInfo()->GetY(isec+1));
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
	if (!pObj->isLine())
	{
		return 0;
	}
	GLine * pLine = (GLine *)pObj;
	return pLine->CalculateProportion(pickx_c, picky_c, pickSection[index]);
}

void GObjectPicker::SetLockOrigin( float x, float y )
{
	lockOriginX_c = x;
	lockOriginY_c = y;
}

void GObjectPicker::SetLockLength( float fLength )
{
	ASSERT(fLength >= 0);
	if (!fLength)
	{
		UnlockLength();
		return;
	}
	fLockLength = fLength;
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

void GObjectPicker::SetLockSplitLine( int splitlocktype, float fval )
{
	ASSERT(splitlocktype >= MARKSPLITLINETYPE_BEGIN && splitlocktype <= MARKSPLITLINETYPE_ENDPROP);
	nSplitLockType = splitlocktype;
	fSplitValue = fval;
}

void GObjectPicker::UnLockSplitLine()
{
	if (pSplitMarking)
	{
		pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_B)->SetLock(false);
		pSplitMarking->getMarkingUI(MARKFLAG_SPLITLENGTH_E)->SetLock(false);
	}
	nSplitLockType = -1;
}

bool GObjectPicker::IsAngleInBetween( int angle, int nBeginAngle, int nMidAngle, int nNextAngle )
{
	return angle >= (nBeginAngle+nMidAngle)/2 && angle < (nMidAngle+nNextAngle)/2;
}


bool staticMIDCBSplit(MarkingUI * pmui, bool bAccept)
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
			UnLockSplitLine();
		}
	}
	else
	{
		UnLockSplitLine();
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
