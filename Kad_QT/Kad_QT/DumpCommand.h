#pragma once
#include "commandtemplate.h"

class DumpCommand : public CommandTemplate
{
public:
	static DumpCommand& getInstance() { static DumpCommand instance; return instance; }

private:
	DumpCommand();
	~DumpCommand();
	DumpCommand(DumpCommand const&);
	void operator=(DumpCommand const&);


public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnClearTemp();

	list<GObject *> lstObj;
	bool bManualMode;
};

