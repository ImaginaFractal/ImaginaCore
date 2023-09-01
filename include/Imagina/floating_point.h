#pragma once

#include <stdint.h>

#include "declarations.h"

#ifdef __cplusplus
namespace Imagina::inline Numerics {
#endif

struct _FloatF64eI64 {
	double Mantissa;
	int64_t Exponent;
};

IM_C_STRUCT_ALIAS(FloatF64eI64);

#ifdef __cplusplus
}
#endif