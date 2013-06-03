#pragma once

enum{
	MSGBM_NULL=0,
	MSGBM_OKCANCEL_SNAPSHOT_RECOVERFROMINVALID,
};

class MessageBoxManager
{
public:
	static MessageBoxManager& getInstance() { static MessageBoxManager instance; return instance; }

private:
	MessageBoxManager();
	~MessageBoxManager();
	MessageBoxManager(MessageBoxManager const&);
	void operator=(MessageBoxManager const&);
	

public:
	bool DoOKCancelBox(int type);

};

