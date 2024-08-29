#pragma once

#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__)
#include <alloca.h>     // alloca
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

#include <filesystem>

namespace Imagina {
	void SetWorkerPriority();
	void *LoadLibrary(const char *filename);
	void *LoadLibrary(std::filesystem::path path);
	bool UnloadLibrary(void *handle);
	void *GetSymbol(void *handle, const char *name);
}