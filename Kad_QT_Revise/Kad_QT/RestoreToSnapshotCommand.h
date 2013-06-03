#pragma once
#include "commandtemplate.h"

class RestoreToSnapshotCommand : public CommandTemplate
{
public:
	static RestoreToSnapshotCommand& getInstance() { static RestoreToSnapshotCommand instance; return instance; }

private:
	RestoreToSnapshotCommand();
	~RestoreToSnapshotCommand();
	RestoreToSnapshotCommand(RestoreToSnapshotCommand const&);
	void operator=(RestoreToSnapshotCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	
};

