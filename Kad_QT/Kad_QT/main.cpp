#include "stdafx.h"
//#include "vld.h"

// Necessary includes and defines for memory leak detection:
#ifdef _SELFCRTLEAKDETECT
#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _MSC_VER


#if defined(_MSC_VER)

// Code to display the memory leak report
// We use a custom report hook to filter out Qt's own memory leaks
// Credit to Andreas Schmidts - http://www.schmidt-web-berlin.de/winfig/blog/?p=154

_CRT_REPORT_HOOK prevHook;

int customReportHook(int /* reportType */, char* message, int* /* returnValue */) {
	// This function is called several times for each memory leak.
	// Each time a part of the error message is supplied.
	// This holds number of subsequent detail messages after
	// a leak was reported
	const int numFollowupDebugMsgParts = 2;
	static bool ignoreMessage = false;
	static int debugMsgPartsCount = 0;

	// check if the memory leak reporting starts
	if ((strncmp(message,"Detected memory leaks!\n", 10) == 0)
		|| ignoreMessage)
	{
		// check if the memory leak reporting ends
		if (strncmp(message,"Object dump complete.\n", 10) == 0)
		{
			_CrtSetReportHook(prevHook);
			ignoreMessage = false;
		} else
			ignoreMessage = true;

		// something from our own code?
		if(strstr(message, ".cpp") == NULL)
		{
			if(debugMsgPartsCount++ < numFollowupDebugMsgParts)
				// give it back to _CrtDbgReport() to be printed to the console
				return FALSE;
			else
				return TRUE;  // ignore it
		} else
		{
			debugMsgPartsCount = 0;
			// give it back to _CrtDbgReport() to be printed to the console
			return FALSE;
		}
	} else
		// give it back to _CrtDbgReport() to be printed to the console
		return FALSE;
}

#endif
#endif



#include "kad_qt.h"
#include <QApplication>


int main(int argc, char *argv[]) {
#ifdef _SELFCRTLEAKDETECT
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	prevHook = _CrtSetReportHook(customReportHook);
    //_CrtSetBreakAlloc(18416); // Use this line to break at the nth memory allocation
#endif
#endif

	QApplication* app = new QApplication(argc, argv);
	Kad_QT *w = new Kad_QT();
	w->show();
	int appError = app->exec();
	delete w;
	delete app;

#ifdef _SELFCRTLEAKDETECT
#if defined(_MSC_VER)
	// Once the app has finished running and has been deleted,
	// we run this command to view the memory leaks:
	_CrtDumpMemoryLeaks();
#endif 
#endif

	return appError;
}

