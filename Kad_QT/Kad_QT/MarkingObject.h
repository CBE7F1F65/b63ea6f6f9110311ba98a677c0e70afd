#pragma once

#include "GLine.h"
#include "MathHelper.h"
#include "QTUI_MarkingFloating_Widget.h"

#define MARKFLAG_POSITIONX	0x0001
#define MARKFLAG_POSITIONY	0x0002
#define MARKFLAG_POSITION	(MARKFLAG_POSITIONX|MARKFLAG_POSITIONY)
#define MARKFLAG_LENGTH		0x0004
#define MARKFLAG_ANGLE		0x0008

#define MARKFLAG_SPLITLENGTH_B	0x0010
#define MARKFLAG_SPLITLENGTH_E	0x0020
#define MARKFLAG_SPLITLENGTH	(MARKFLAG_SPLITLENGTH_B|MARKFLAG_SPLITLENGTH_E)
#define MARKFLAG_XOFFSET		0x0040
#define MARKFLAG_YOFFSET		0x0080
#define MARKFLAG_OFFSET			(MARKFLAG_XOFFSET|MARKFLAG_YOFFSET)

#define MARKFLAG_HORZVAL		0x0100
#define MARKFLAG_VERTVAL		0x0200
#define MARKFLAG_SHAPEVAL		(MARKFLAG_HORZVAL|MARKFLAG_VERTVAL)

enum{
	MARKSPLITLINETYPE_BEGIN,
	MARKSPLITLINETYPE_BEGINPROP,
	MARKSPLITLINETYPE_END,
	MARKSPLITLINETYPE_ENDPROP,
};

class MarkingUI;
class MarkingObject;

typedef bool (*MarkingInputDoneCallback)(MarkingUI * pmui, bool bAccept);

class MarkingUI
{
public:
	MarkingUI(MarkingObject * p, int typeflag);
	~MarkingUI();

public:
    void SetPosition(PointF2D pos) { ptPos = pos; };
	void SetCallback(MarkingInputDoneCallback _cb){cb = _cb;};

	void SetString(QString * pStr);
	PointF2D * getPos(){return &ptPos;};

	QString getStr(){return str;};
	float getFloat(bool * bOk=NULL);
	int getInt(bool *bOk=NULL);

	void SetEditable(bool bSet);
	bool isEditable(){return bEditable;};
	void SetLock(bool bSet);

	bool DoCallback(bool bAccept);

	MarkingObject * getMarking(){return pMarking;};
	int getTypeFlag(){return nTypeFlag;};

	void UseUI();
	void DeleteUI();
	void DoMove();
	void OnDoneEdit( QString strEdit );
	bool IsValueLocked();
private:
    PointF2D ptPos;
    QTUI_MarkingFloating_Widget * pWidget;
	MarkingInputDoneCallback cb;
	QString str;
	bool bEditable;
	int nTypeFlag;

	MarkingObject * pMarking;
};

class MarkingObject
{
public:
	MarkingObject();
	virtual ~MarkingObject();

protected:
	void SetValue(GObject * pObj, int nFlag);

public:
	void CallUpdate();
	void CallRender();

public:

	virtual void Update();
	virtual void Render();

	GObject * getTargetObj(){return pTargetObj;};
	int getMarkFlag(){return nMarkFlag;};
	
	MarkingUI * getMarkingUI(int nFlag);
	void SetRedraw();

protected:
	GObject * pTargetObj;
	int nMarkFlag;
	int nModifyVersion;

	bool bRedraw;
	bool bUpdate;

	MarkingUI *pmuiPositionX;
	MarkingUI *pmuiPositionY;
    MarkingUI *pmuiLength;
	MarkingUI *pmuiAngle;
	MarkingUI *pmuiSplitB;
	MarkingUI *pmuiSplitE;
	MarkingUI *pmuiXOffset;
	MarkingUI *pmuiYOffset;
	MarkingUI *pmuiHorzVal;
	MarkingUI *pmuiVertVal;
};

class MarkingLine : public MarkingObject
{
public:
//	MarkingLine();
	MarkingLine(GLine * pLine, int nFlag);

	virtual void Update();
	virtual void Render();

protected:
	PointF2D ptLengthDiff;
	PointF2D ptArcPoint;
	float fStraightLineLength;
	int nLineAngle;
};

class MarkingSplitLine : public MarkingObject
{
public:
	MarkingSplitLine(GLine * pLine, int nFlag);

	virtual void Update();
	virtual void Render();

	virtual void SetSplitPoint(float x, float y, int isec=0);

protected:
	PointF2D ptSplitPoint;
	int iSplitSec;
	PointF2D ptPosDiff;
	bool bSplitPointChanged;
	float fLineLength;
};

class MarkingOffset : public MarkingObject
{
public:
	MarkingOffset(GObject * pObj, int nFlag);

	virtual void Update();
	virtual void Render();

	virtual void SetMoveOrigin(float x, float y);
	virtual void SetNowPos(float x, float y);

protected:
	PointF2D ptMoveOrigin;
	PointF2D ptNow;
};