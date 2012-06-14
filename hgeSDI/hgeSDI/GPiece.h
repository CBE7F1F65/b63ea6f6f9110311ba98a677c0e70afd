#pragma once
#include "gshape.h"
class GPiece :
	public GShape
{
public:
	GPiece(void);
	~GPiece(void);

	virtual bool isPiece(){return true;};

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );
	virtual void MovePiece( float movedx_c, float movedy_c, bool trymode );
};