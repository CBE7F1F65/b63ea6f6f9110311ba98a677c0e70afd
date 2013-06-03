#pragma once
#include "commandtemplate.h"

class AddSACommand : public CommandTemplate
{
public:
	static AddSACommand& getInstance() { static AddSACommand instance; return instance; }

private:
	AddSACommand();
	~AddSACommand();
	AddSACommand(AddSACommand const&);
	void operator=(AddSACommand const&);


public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnClearTemp();

	static bool staticFilterCallback(GObject * pObj);

public:
	list<GObject *> lstObj;
	bool bManualMode;
};

