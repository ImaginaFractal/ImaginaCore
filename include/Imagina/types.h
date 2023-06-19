#pragma once

#include <stdint.h>
#include "multi-precision.h"
#include "floating_point.h"

#ifdef __cplusplus
namespace Imagina {
#endif

using GRReal = float; // Real type for graphics
using SRReal = double; // Real type for fractal with standard precision and standard range
using HRReal = FloatF64eI64; // Real type for fractal with standard precision and highest range
using HPReal = MPReal; // Real type for fractal with high precision

using GRInt = int32_t;
using ITInt = int64_t;
using ITUInt = uint64_t;

using BCInt = int64_t; // Integer type for bit count
using BCUInt = uint64_t; // Integer type for bit count
using ExpInt = int64_t;

#ifdef __cplusplus
}
#endif