#pragma once

#include <cstdint>
#include "basic_types.h"
#include "multi-precision.h"
#include "floating_point.h"
#include "complex.h"
#include "vector.h"

namespace Imagina::inline Numerics {
	using HRReal = FloatF64eI64; // Real type for fractal with standard precision and highest range
	using HPReal = MPReal; // Real type for fractal with high precision

	using SRComplex = Complex<SRReal>;
	using HRComplex = Complex<HRReal>;
	using HPComplex = Complex<HPReal>;

	using GRVector3 = Vector3<GRReal>;
	using GRVector4 = Vector4<GRReal>;

	using RGB = GRVector3;
	using RGBA = GRVector4;
}