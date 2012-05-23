#pragma once
#include "GObject.h"

class GPoint :
	public GObject
{
public:
	GPoint();
	GPoint(float x, float y);
	~GPoint();

	void SetPosition(float x, float y);

	float x;
	float y;
};

class GEndPoint : public GPoint
{
public:
	GEndPoint();
	GEndPoint(float x, float y);
	~GEndPoint();
};