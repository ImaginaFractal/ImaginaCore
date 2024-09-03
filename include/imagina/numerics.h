#pragma once

#include <cstdint>
#include "basic_types.h"
#include "multi-precision.h"
#include "float_f64ei64.h"
#include "complex.h"
#include "vector.h"

namespace Imagina::inline Numerics {
	using real_hr = float_f64ei64; // Real type for fractal with standard precision and highest range
	using real_hp = imp_real; // Real type for fractal with high precision

	using complex_sr = Complex<real_sr>;
	using complex_hr = Complex<real_hr>;
	using complex_hp = Complex<real_hp>;

	using vector3_gr = Vector3<real_gr>;
	using vector4_gr = Vector4<real_gr>;

	using rgb32f = Vector3<float>;
	using rgba32f = Vector4<float>;
}