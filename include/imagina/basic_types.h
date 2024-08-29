#pragma once

#include <stdint.h>

namespace Imagina::inline Numerics {
	using GRReal = float; // Real type for graphics
	using SRReal = double; // Real type for fractal with standard precision and standard range

	using GRInt = int32_t; // Integer type for graphics
	using ITInt = int64_t; // Integer type for storing iteration or step count
	using ITUInt = uint64_t; // Unsigned integer type for storing iteration or step count

	using BCInt = int64_t; // Integer type for bit count
	using BCUInt = uint64_t; // Integer type for bit count
	using ExpInt = int64_t; // Integer type for storing exponent

	using MPExpInt = intptr_t;
	using MPBCInt = intptr_t;
	using MPBCUInt = uintptr_t;

}