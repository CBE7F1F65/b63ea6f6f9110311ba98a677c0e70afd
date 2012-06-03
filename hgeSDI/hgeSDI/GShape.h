#pragma once
#include "gline.h"
class GShape :
	public GLine
{
public:
	GShape(void);
	~GShape(void);

	virtual const char * GetTypeName();
};