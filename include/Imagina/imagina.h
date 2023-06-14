#pragma once

#include <stdint.h>

#include "declarations.h"
#include "types.h"
#include "multi-precision.h"
#include "pixel_processing.h"
#include "evaluator.h"

#ifdef __cplusplus
extern "C" namespace ImCApi {
	//using namespace Imagina;
#else
#include <stdbool.h>
#endif

im_export void *ImAlloc(size_t size);
im_export void ImFree(void *p);

im_export bool RasterizingInterface_GetCoordinate(IRasterizingInterface *rasterizingInterface, Imagina::HRReal *x, Imagina::HRReal *y);

im_export void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value);

typedef void (*pIEvaluator_Release)(IEvaluator *instance);
typedef const Imagina::PixelDataDescriptor *(*pIEvaluator_GetOutputDescriptor)(IEvaluator *instance);
typedef void (*pStandardEvaluator_SetEvaluationParameters)(IEvaluator *instance, const Imagina::StandardEvaluationParameters *parameters);
typedef void (*pSimpleEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);
typedef void (*pLowPrecisionEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);

#ifdef __cplusplus
}
#endif