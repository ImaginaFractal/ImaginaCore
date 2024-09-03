#pragma once

#include <stdint.h>

namespace Imagina::inline Numerics {
	using real_gr = float; // Real type for graphics
	using real_sr = double; // Real type for fractal with standard precision and standard range

	using int_gr = int32_t; // Integer type for graphics
	using int_iter = int64_t; // Integer type for storing iteration or step count
	using uint_iter = uint64_t; // Unsigned integer type for storing iteration or step count

	using int_bc = int64_t; // Integer type for bit count
	using uint_bc = uint64_t; // Integer type for bit count
	using int_exp = int64_t; // Integer type for storing exponent

	using mp_int_bc = intptr_t;
	using mp_uint_bc = uintptr_t;
	using mp_int_exp = intptr_t;

}