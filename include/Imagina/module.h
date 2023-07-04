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

#ifndef __cplusplus
typedef uint32_t ComponentType;
typedef uint32_t ComponentFlag;
#else
enum class ComponentType : uint32_t;
enum class ComponentFlag : uint32_t;
#endif

typedef void *(*pCreateComponent)(const char *ID);

typedef struct {
	const char *Name;
	const char *DisplayName;
	pCreateComponent Create;
	ComponentType Type;
	ComponentFlag Flags;
} ComponentInfo;

typedef struct {
	const char *Name;
	const char *DisplayName;
	size_t ComponentCount;
	const ComponentInfo *Components;
} ModuleInfo;

typedef void (*pImInit)();
typedef ModuleInfo *(*pImGetModuleInfo)();

#ifdef __cplusplus
}
#endif