#include "platform_dependent"

#ifdef _WIN32

#if __has_include(<Windows.h>)
#include <Windows.h>
#else
#include <windows.h>
#endif

void SetWorkerPriority() {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
}

#else

void SetWorkerPriority() {
	// Do nothing
}

#endif