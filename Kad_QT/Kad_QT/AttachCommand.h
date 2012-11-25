#pragma once
#include "CommandTemplate.h"

class AttachCommand : public CommandTemplate
{
public:
	static AttachCommand& getInstance() { static AttachCommand instance; return instance; }

private:
	AttachCommand();
	~AttachCommand();
	AttachCommand(AttachCommand const&);
	void operator=(AttachCommand const&);


public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
};