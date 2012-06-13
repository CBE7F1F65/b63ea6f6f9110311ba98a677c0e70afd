#pragma once
#include "GObject.h"

class GPoint :
	public GObject
{
public:
	GPoint();
	GPoint(float x, float y);
	~GPoint();

	virtual float getX(){return x;};
	virtual float getY(){return y;};

	void SetPosition(float x, float y);

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );
	float x;
	float y;
};

class GSubstantivePoint :
	public GPoint
{
public:
	GSubstantivePoint();
	~GSubstantivePoint();

//	virtual bool isAttributeNode(){return false;};
	virtual bool isRepresentablePoint(){return true;};

	virtual void OnRender(bool bHighlight/* =false */);
};

class GAttributePoint :
	public GPoint
{
public:
	GAttributePoint(){};
	~GAttributePoint(){};

	virtual bool isAttributeNode(){return true;};
	virtual bool isRepresentablePoint(){return true;};

	virtual void OnRender(bool bHighlight/* =false */);
};

class GVirtualPoint :
	public GAttributePoint
{
public:
	GVirtualPoint();
	~GVirtualPoint();

	virtual bool isRepresentablePoint(){return false;};
	virtual bool isModifyParent(){return false;};

	virtual void OnRender(bool bHighlight/* =false */);
};

class GEndPoint : public GAttributePoint
{
public:
	GEndPoint();
	GEndPoint(float x, float y);
	~GEndPoint();

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
};

class GMidPoint : public GAttributePoint
{
public:
	GMidPoint();
	GMidPoint(float x, float y);
	~GMidPoint();
	
	virtual bool isModifyParent(){return false;};

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
};