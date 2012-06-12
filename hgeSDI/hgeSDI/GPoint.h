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

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );
	float x;
	float y;
};

class GEndPoint : public GPoint
{
public:
	GEndPoint();
	GEndPoint(float x, float y);
	~GEndPoint();

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
};

class GMidPoint : public GPoint
{
public:
	GMidPoint();
	GMidPoint(float x, float y);
	~GMidPoint();

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
};