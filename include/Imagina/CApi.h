#pragma once

#include <stdint.h>

#ifndef im_export
#ifdef _WIN32
#ifdef IM_BUILD_CORE
#define im_export __declspec(dllexport)
#else
#define im_export __declspec(dllimport)
#endif
#else
#define im_export
#endif
#endif

#ifdef __cplusplus
extern "C" namespace ImCApi {
#else
#include <stdbool.h>
#endif

typedef float GRReal; // Real type for graphics
typedef double SRReal; // Real type for fractal with standard precision and standard range
typedef double HRReal; // Real type for fractal with standard precision and highest range

typedef int32_t GRInt;
typedef int64_t ITInt;
typedef uint64_t ITUint;

typedef int64_t BCInt; // Integer type for bit count
typedef uint64_t BCUint; // Integer type for bit count
typedef int64_t ExpInt;

struct StandardEvaluationParameters {
	ITUint Iterations;
	HRReal DivergentEscapeRadius;
	HRReal ConvergentEscapeRadius;
};

typedef struct _IController IController;

class IPixelManager;
class IGpuTextureManager;
class IGpuPixelManager;
typedef struct _IRasterizingInterface IRasterizingInterface;

class IRasterizer;
typedef struct _IEvaluator IEvaluator;

class ILocationManager;

class IFractalContext;

im_export bool RasterizingInterface_GetCoordinate(IRasterizingInterface *rasterizingInterface, HRReal *x, HRReal *y);

im_export void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value);

typedef void (*pIEvaluator_Release)(IEvaluator *instance);
typedef void (*pStandardEvaluator_SetEvaluationParameters)(IEvaluator *instance, const StandardEvaluationParameters *parameters);
typedef void (*pSimpleEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);
typedef void (*pLowPrecisionEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);

#ifdef __cplusplus
}
#endif