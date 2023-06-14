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

#include "multi-precision.h"
#include "pixel_processing.h"
#include "types.h"

#ifdef __cplusplus
extern "C" namespace ImCApi {
	using namespace Imagina;
#else
#include <stdbool.h>
#endif

typedef struct _StandardEvaluationParameters {
	ITUint Iterations;
	HRReal DivergentEscapeRadius;
	HRReal ConvergentEscapeRadius;
}StandardEvaluationParameters;

typedef struct _IController IController;

typedef struct _IPixelManager IPixelManager;
typedef struct _IGpuTextureManager IGpuTextureManager;
typedef struct _IGpuPixelManager IGpuPixelManager;
typedef struct _IRasterizingInterface IRasterizingInterface;

typedef struct _IRasterizer IRasterizer;
typedef struct _IEvaluator IEvaluator;

typedef struct _ILocationManager ILocationManager;

typedef struct _IFractalContext IFractalContext;

im_export void *ImAlloc(size_t size);
im_export void ImFree(void *p);

im_export bool RasterizingInterface_GetCoordinate(IRasterizingInterface *rasterizingInterface, HRReal *x, HRReal *y);

im_export void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value);

typedef void (*pIEvaluator_Release)(IEvaluator *instance);
typedef const PixelDataDescriptor *(*pIEvaluator_GetOutputDescriptor)(IEvaluator *instance);
typedef void (*pStandardEvaluator_SetEvaluationParameters)(IEvaluator *instance, const StandardEvaluationParameters *parameters);
typedef void (*pSimpleEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);
typedef void (*pLowPrecisionEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);

#ifdef __cplusplus
}
#endif