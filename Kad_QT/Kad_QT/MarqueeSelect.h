#pragma once

#include "GPiece.h"
#include "MarkingObject.h"

class MarqueeSelect
{
public:
	static MarqueeSelect& getInstance() { static MarqueeSelect instance; return instance; }

private:
	MarqueeSelect();
	~MarqueeSelect();
	MarqueeSelect(MarqueeSelect const&);
	void operator=(MarqueeSelect const&);

private:

	list<GObject *>selectednodes;

	int itemmovestate;

	int marqueestate;
	float beginx_c;
	float beginy_c;
	float endx_c;
	float endy_c;

	GObject * pBeginObj;

	float lastmx_c;
	float lastmy_c;
	int nomoveflag;

	MarkingOffset * pMarkingOffset;
	MarkingLine * pMarkingLine;

public:

	void DeSelect(GObject * pObj);
	void DeSelectAll();
	void AddSelect(GObject * pObj, int level=-1);
	void SelectAll(bool bOnlyThisLayer=false);
	void AddRecSelect(GObject * pObj, int level);
	static bool staticRecAddSelectCB(GObject * pObj, void * param_intLevel);

	void OnSelectChanged();

	void DoMovePoint(GPoint * pPoint, float movedx_c, float movedy_c, int nMoveActionID);
	void DoMoveLine(GLine * pLine, float movedx_c, float movedy_c, int nMoveActionID);
	void DoMovePiece(GPiece * pPiece, float movedx_c, float movedy_c, int nMoveActionID);

	bool CheckObjInSelection(GObject * pTestObj, bool bFindAncestor=false, bool bFindUpperClass=false, bool bFindMerge=false, bool bFindCling=false, list<GObject *> * plstObj=NULL);

	void Update();
	void Render();
	void CheckMarqueeSelect_Line( GObject * pObj, bool bAdd=true );
	void CheckMarqueeSelect_Pt( GObject * pObj, bool bAdd=true );
	void MoveSelected( float movedx_c, float movedy_c );
	void CheckValid();

	void BeginMove(float nowx, float nowy);
	void EndMove();

	int GetMarqueeState(){return marqueestate;};
	int GetMoveState(){return itemmovestate;};

	bool MarqPickFilterCallback(GObject * pObj);
	static bool staticMarqPickFilterCallback(GObject * pObj);

	void OnDeleteNode(GObject * pDeletedObj);
	bool MIDCBLength( MarkingUI * pmui, bool bAccept );
	bool MIDCBAngle( MarkingUI * pmui, bool bAccept );
	bool MIDCBOffset( MarkingUI * pmui, bool bAccept );
	static bool staticMIDCBLength( MarkingUI * pmui, bool bAccept );
	static bool staticMIDCBAngle( MarkingUI * pmui, bool bAccept );
	static bool staticMIDCBOffset( MarkingUI * pmui, bool bAccept );
	list<GObject*> * OnGetSelectedNodes();
};

