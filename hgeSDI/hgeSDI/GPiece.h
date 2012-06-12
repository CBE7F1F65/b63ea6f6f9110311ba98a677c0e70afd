#pragma once
#include "gshape.h"
class GPiece :
	public GShape
{
public:
	GPiece(void);
	~GPiece(void);

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );
};