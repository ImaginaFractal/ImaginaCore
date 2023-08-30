#pragma once

#if false
#include <stdint.h>

#include "declarations.h"
#include "types.h"
#include "multi-precision.h"
#include "evaluator.h"
#ifdef __cplusplus
extern "C" namespace ImCApi {
	//using namespace Imagina;
#else
#include <stdbool.h>
#endif

im_export void *ImAlloc(size_t size);
im_export void ImFree(void *p);

im_export bool RasterizingInterface_GetPixel(IRasterizingInterface *rasterizingInterface, Imagina::HRReal *x, Imagina::HRReal *y);

im_export void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value);

typedef void (*pIEvaluator_Release)(IEvaluator instance);
typedef const Imagina::PixelDataInfo *(*pIEvaluator_GetOutputInfo)(IEvaluator instance);

typedef void (*pComplexEvaluator_SetEvaluationParameters)(IEvaluator instance, const Imagina::StandardEvaluationParameters *parameters);

typedef void (*pSimpleEvaluator_Evaluate)(IEvaluator instance, IRasterizingInterface *rasterizingInterface);
typedef void (*pSimpleEvaluator_SetReferenceLocationAndPrecompute)(IEvaluator instance, const Imagina::HPReal *x, const Imagina::HPReal *y, Imagina::HRReal radius);

typedef void (*pLowPrecisionEvaluator_Evaluate)(IEvaluator instance, IRasterizingInterface *rasterizingInterface);

typedef struct {
	ImCApi::pIEvaluator_GetOutputInfo getOutputInfo;
	ImCApi::pComplexEvaluator_SetEvaluationParameters setEvaluationParameters;
	ImCApi::pLowPrecisionEvaluator_Evaluate evaluate;
} LowPrecisionEvaluatorFunctionTable;

typedef struct {
	ImCApi::pIEvaluator_GetOutputInfo getOutputInfo;
	ImCApi::pComplexEvaluator_SetEvaluationParameters setEvaluationParameters;
	ImCApi::pSimpleEvaluator_SetReferenceLocationAndPrecompute setReferenceLocationAndPrecompute;
	ImCApi::pSimpleEvaluator_Evaluate evaluate;
} SimpleEvaluatorFunctionTable;

#ifdef __cplusplus
}
#endif
#endif