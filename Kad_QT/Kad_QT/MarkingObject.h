#pragma once

#include "GLine.h"
#include "MathHelper.h"
#include "QTUI_MarkingFloating_Widget.h"

#define MARKFLAG_POSITIONX	0x01
#define MARKFLAG_POSITIONY	0x02
#define MARKFLAG_POSITION	(MARKFLAG_POSITIONX|MARKFLAG_POSITIONY)
#define MARKFLAG_LENGTH		0x04
#define MARKFLAG_ANGLE		0x08

class MarkingUI;
class MarkingObject;

typedef bool (*MarkingInputDoneCallback)(MarkingUI * pmui, bool bAccept);

class MarkingUI
{
public:
	MarkingUI(MarkingObject * p);
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

	bool DoCallback(bool bAccept);

	MarkingObject * getMarking(){return pMarking;};

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

	virtual void Update();
	virtual void Render();

	GObject * getTargetObj(){return pTargetObj;};
	int getMarkFlag(){return nMarkFlag;};

	void SetEditable(int nFlag, bool bSet);
	void SetCallback(int nFlag, MarkingInputDoneCallback cb);

	PointF2D * getPositionXMark(){return pmuiPositionX->getPos();};
	PointF2D * getPositionYMark(){return pmuiPositionY->getPos();};
	PointF2D * getLengthMark(){return pmuiLength->getPos();};
	PointF2D * getAngleMark(){return pmuiAngle->getPos();};

	void SetRedraw();

protected:
	GObject * pTargetObj;
	int nMarkFlag;
	int nModifyVersion;

	MarkingUI *pmuiPositionX;
	MarkingUI *pmuiPositionY;
    MarkingUI *pmuiLength;
    MarkingUI *pmuiAngle;
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
