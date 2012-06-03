#include "stdafx.h"
#ifdef __WIN32
#pragma warning(disable:4073)
#pragma init_seg(lib)

static class InitDllLoader {
public:
	InitDllLoader()
	{
		module = LoadLibraryA(DEFAULT_HGEDLLPATH);
	}

	~InitDllLoader()
	{
		FreeLibrary(module);
	}

private:

	HMODULE module;
} _initDllLoader;

#endif