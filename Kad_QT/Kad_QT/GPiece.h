#pragma once
#include "gshape.h"
class GPiece :
	public GShape
{
public:
	typedef GShape super;
	virtual const char * getTypeName();		// Do not implement seperately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GPiece(void);
	virtual ~GPiece(void);

	virtual GPiece * getPiece(){return this;};
	virtual bool isPiece(){return true;};

	virtual const char * getDisplayName();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual void MovePiece( float movedx_c, float movedy_c, bool trymode );
};