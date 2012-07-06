#pragma once

#include "GPiece.h"

class MarqueeSelect
{
public:
	static MarqueeSelect& getInstance() { static MarqueeSelect instance; return instance; }

private:
	MarqueeSelect();
	~MarqueeSelect();
	MarqueeSelect(MarqueeSelect const&);
	void operator=(MarqueeSelect const&);

public:

	list<GObject *>selectednodes;

	int itemmovestate;

	int marqueestate;
	float beginx_c;
	float beginy_c;
	float endx_c;
	float endy_c;

	float lastmx_c;
	float lastmy_c;

	void DeSelect(GObject * pObj);
	void DeSelectAll();
	void AddSelect(GObject * pObj, int level=-1);
	void PushSelectCling( GObject * pObj );

	int nomoveflag;

	void DoMovePoint(GPoint * pPoint, float movedx_c, float movedy_c, int nMoveActionID);
	void DoMoveLine(GLine * pLine, float movedx_c, float movedy_c, int nMoveActionID);
	void DoMovePiece(GPiece * pPiece, float movedx_c, float movedy_c, int nMoveActionID);

	bool CheckObjInSelection(GObject * pObj, bool bFindAncestor=false, bool bFindUpperClass=false, bool bFindMerge=false, bool bFindCling=false);

	void Update();
	void Render();
	void CheckMarqueeSelect_Line( GObject * pObj, bool bAdd=true );
	void CheckMarqueeSelect_Pt( GObject * pObj, bool bAdd=true );
	void MoveSelected( float movedx_c, float movedy_c );
	void CheckValid();

	bool PickFilterCallback(GObject * pObj);

	void OnDeleteNode(GObject * pObj);
};

