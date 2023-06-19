#pragma once

#include "declarations.h"
#include "types.h"

#ifdef __cplusplus
#include "floating_point"
namespace Imagina {
#endif
	
struct _StandardEvaluationParameters {
	ITUInt Iterations;
	HRReal DivergentEscapeRadius;
	HRReal ConvergentEscapeRadius;
};

IM_C_STRUCT_ALIAS(StandardEvaluationParameters);

#ifdef __cplusplus
}
#endif