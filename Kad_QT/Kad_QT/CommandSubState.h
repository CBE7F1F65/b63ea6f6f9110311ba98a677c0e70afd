#pragma once

#include "CommandMap.h"

class CSS_PickVoid: public CommandSubStateTemplate
{
public:
	CSS_PickVoid(FillCommandParamInfo * pToInfo, FillCommandParamInfo * xParamInfo, FillCommandParamInfo * yParamInfo);
	~CSS_PickVoid(void);

	virtual bool RequirePickPoint(){return true;};
	virtual bool ExcuteSubState();
	virtual bool SetPickParam( int iret, bool bReady );

	FillCommandParamInfo xFCP;
	FillCommandParamInfo yFCP;
};

