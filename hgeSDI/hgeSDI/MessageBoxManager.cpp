#include "StdAfx.h"
#include "MessageBoxManager.h"
#include "StringManager.h"
#include "Main.h"

MessageBoxManager::MessageBoxManager(void)
{
}


MessageBoxManager::~MessageBoxManager(void)
{
}

bool MessageBoxManager::DoOKCancelBox( int type )
{
	string strtitle;
	string strcontent;
	StringManager * psm = &StringManager::getInstance();
	HWND hwnd = MainInterface::getInstance().hge->System_GetState(HGE_HWND);

	switch (type)
	{
	case MSGBM_OKCANCEL_SNAPSHOT_RECOVERFROMINVALID:
		strtitle = psm->GetMBWarningName();
		strcontent = psm->GetMBWRecoverFromInvalidSnapshotName();
		break;
	default:
		return false;
	}

	return (MessageBox(hwnd, strcontent.c_str(), strtitle.c_str(), MB_OKCANCEL) == IDOK);
}