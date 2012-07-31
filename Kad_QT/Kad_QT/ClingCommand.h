#pragma once
#include "CommandTemplate.h"

class ClingCommand : public CommandTemplate
{
public:
	static ClingCommand& getInstance() { static ClingCommand instance; return instance; }

private:
	ClingCommand();
	~ClingCommand();
	ClingCommand(ClingCommand const&);
	void operator=(ClingCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
//	virtual void OnProcessUnDoCommand(RevertableCommand * rc);
	
};

