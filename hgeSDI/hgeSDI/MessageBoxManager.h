#pragma once

enum{
	MSGBM_NULL=0,
	MSGBM_OKCANCEL_SNAPSHOT_RECOVERFROMINVALID,
};

class MessageBoxManager
{
public:
	static MessageBoxManager& getInstance()
	{
		static MessageBoxManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	MessageBoxManager();
	~MessageBoxManager();
	// Don't forget to declare these two.
	MessageBoxManager(MessageBoxManager const&);
	// Don't Implement
	void operator=(MessageBoxManager const&);
	// Don't implement


public:
	bool DoOKCancelBox(int type);

};

