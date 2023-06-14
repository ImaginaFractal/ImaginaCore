#pragma once

#include "declarations.h"
#include "types.h"

#ifdef __cplusplus
namespace Imagina {
#endif
	
struct _StandardEvaluationParameters {
	ITUint Iterations;
	HRReal DivergentEscapeRadius;
	HRReal ConvergentEscapeRadius;
};

IM_C_STRUCT_ALIAS(StandardEvaluationParameters);

#ifdef __cplusplus
}
#endif