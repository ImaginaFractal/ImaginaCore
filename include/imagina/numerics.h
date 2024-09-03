#pragma once

#include <cstdint>
#include "basic_types.h"
#include "multi-precision.h"
#include "float_f64ei64.h"
#include "complex.h"
#include "vector.h"

namespace Imagina::inline Numerics {
	using real_hr = float_f64ei64; // Real type for fractal with standard precision and highest range
	using real_hp = MPReal; // Real type for fractal with high precision

	using SRComplex = Complex<real_sr>;
	using HRComplex = Complex<real_hr>;
	using HPComplex = Complex<real_hp>;

	using GRVector3 = Vector3<real_gr>;
	using GRVector4 = Vector4<real_gr>;

	using RGB = GRVector3;
	using RGBA = GRVector4;
}