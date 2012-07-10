#pragma once

#include "GLine.h"
#include "MathHelper.h"
#include "QTUI_MarkingFloating_Widget.h"

#define MARKFLAG_POSITION	0x01
#define MARKFLAG_LENGTH		0x02
#define MARKFLAG_ANGLE		0x04

class MarkingUI
{
public:
	MarkingUI()
	{
		pWidget = NULL;
	};
	~MarkingUI()
	{
		if (pWidget)
		{
			delete pWidget;
		}
	};

public:
    void SetPosition(PointF2D pos)
    {
        ptPos = pos;
    };
	void SetString(QString * pStr);
	PointF2D * getPos(){return &ptPos;};
	QString getStr(){return str;};

	void UseUI();
	void DeleteUI();
	void DoMove();

private:
    PointF2D ptPos;
    QTUI_MarkingFloating_Widget * pWidget;
	QString str;
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

	PointF2D * getPositionXMark(){return muiPositionX.getPos();};
	PointF2D * getPositionYMark(){return muiPositionY.getPos();};
	PointF2D * getLengthMark(){return muiLength.getPos();};
	PointF2D * getAngleMark(){return muiAngle.getPos();};

	void SetRedraw();

protected:
	GObject * pTargetObj;
	int nMarkFlag;
	int nModifyVersion;

	MarkingUI muiPositionX;
	MarkingUI muiPositionY;
    MarkingUI muiLength;
    MarkingUI muiAngle;
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
