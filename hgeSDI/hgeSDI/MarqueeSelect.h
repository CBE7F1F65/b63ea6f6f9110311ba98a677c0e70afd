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
	void AddSelect(GObject * pObj);

	int nomoveflag;

	void DoMovePoint(GPoint * pPoint, float movedx_c, float movedy_c);
	void DoMoveLine(GLine * pLine, float movedx_c, float movedy_c);
	void DoMovePiece(GPiece * pPiece, float movedx_c, float movedy_c);

	bool CheckObjInSelection(GObject * pObj, bool bFindAncestor=false, bool bFindUpperClass=false);

	void Update();
	void Render();
	void CheckMarqueeSelect_Line( GObject * pObj );
	void CheckMarqueeSelect_Pt( GObject * pObj );
	void MoveSelected( float movedx_c, float movedy_c );
	void CheckValid();

	bool PickFilterCallback(GObject * pObj);

	void OnDeleteNode(GObject * pObj);
};

