#pragma once

#include "declarations.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
namespace Imagina {
#endif

enum _ComponentType {
	IM_CT_Evaluator			= 1,
	IM_CT_LocationManager	= 2,
	IM_CT_PixelProcessor	= 3,
	IM_CT_PixelManager		= 16,
	IM_CT_Controller		= 20,
	IM_CT_MultiPrecision	= 28,
	IM_CT_ModuleExtension	= 31,
};

enum _ComponentFlag {
	IM_CF_UseCApi = 0x1,
};

enum _ProxyType {
	IM_PT_LowPrecisionEvaluator = 1,
	IM_PT_SimpleEvaluator = 2,
};

#ifndef __cplusplus
typedef uint32_t ComponentType;
typedef uint32_t ComponentFlag;
typedef uint32_t ProxyType;
#else
enum class ComponentType : uint32_t;
enum class ComponentFlag : uint32_t;
enum class ProxyType : uint32_t;
#endif

typedef void *(*pCreateComponent)(const char *ID);
typedef void (*pReleaseComponent)(const char *ID, void *instance);

typedef struct {
	const char *Name;
	const char *DisplayName;
	pCreateComponent Create;
	pReleaseComponent Release;
	ComponentType Type;
	ComponentFlag Flags;

	// For C API
	ProxyType ProxyType;
	void *FunctionTable;
} ComponentInfo;

typedef struct {
	const char *Name;
	const char *DisplayName;
	size_t ComponentCount;
	const ComponentInfo *Components;
} ModuleInfo;

typedef void (*pImInit)();
typedef const ModuleInfo *(*pImGetModuleInfo)();

#ifdef __cplusplus
}
#endif