#pragma once
#include "commandtemplate.h"

class OutputCommand :
	public CommandTemplate
{
public:
	static OutputCommand& getInstance() { static OutputCommand instance; return instance; }

private:
	OutputCommand();
	~OutputCommand();
	OutputCommand(OutputCommand const&);
	void operator=(OutputCommand const&);

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

	bool OutputNodeStructure(GObject * pObj);

public:
	
};

