#pragma once
#include "commandtemplate.h"

class IsolateCommand : public CommandTemplate
{
public:
	static IsolateCommand& getInstance() { static IsolateCommand instance; return instance; }

private:
	IsolateCommand();
	~IsolateCommand();
	IsolateCommand(IsolateCommand const&);
	void operator=(IsolateCommand const&);


public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
};