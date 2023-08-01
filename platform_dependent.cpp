#include "platform_dependent"

#ifdef _WIN32

#if __has_include(<Windows.h>)
#include <Windows.h>
#else
#include <windows.h>
#endif
#undef LoadLibrary

namespace Imagina {
	void SetWorkerPriority() {
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	}

	void *LoadLibrary(const char *filename) {
		return LoadLibraryA(filename);
	}

	inline void *_LoadLibrary(LPCSTR filename) {
		return LoadLibraryA(filename);
	}

	inline void *_LoadLibrary(LPCWSTR filename) {
		return LoadLibraryW(filename);
	}

	void *LoadLibrary(std::filesystem::path path) {
		return _LoadLibrary(path.c_str());
	}

	bool UnloadLibrary(void *handle) {
		return FreeLibrary((HMODULE)handle) != 0;
	}

	void *GetSymbol(void *handle, const char *name) {
		return (void *)GetProcAddress((HMODULE)handle, name);
	}
}

#else
#include <dlfcn.h>

namespace Imagina {
	void SetWorkerPriority() {
		// Do nothing
	}

	void *LoadLibrary(const char *filename) {
		return dlopen(filename, RTLD_LAZY | RTLD_LOCAL);
	}

	void *LoadLibrary(std::filesystem::path path) {
		return dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
	}

	bool UnloadLibrary(void *handle) {
		return FreeLibrary((HMODULE)handle) == 0;
	}

	void *GetSymbol(void *handle, const char *name) {
		return dlsym(handle, name);
	}
}

#endif