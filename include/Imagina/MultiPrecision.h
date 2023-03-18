#pragma once

#include <stdint.h>
#include "Declarations.h"

namespace Imagina {
	struct im_export FixedGeneric32 {
		static size_t DefaultSize;
		static constexpr uint32_t SignMask = 0x80000000;

		uint32_t *Value;
		size_t Size = DefaultSize;

		FixedGeneric32() : Value(new uint32_t[DefaultSize]) {}

		FixedGeneric32(double x);
		FixedGeneric32(const FixedGeneric32 &x);
		FixedGeneric32(FixedGeneric32 &&x);

		FixedGeneric32 &operator=(const FixedGeneric32 &x);
		FixedGeneric32 &operator=(FixedGeneric32 &&x);

		bool IsNegative() const { return Value[Size - 1] & (1ull << 31) ? true : false; }

		operator double() const;

		FixedGeneric32 &operator+=(const FixedGeneric32 &x);
		FixedGeneric32 &operator-=(const FixedGeneric32 &x);
		FixedGeneric32 &operator*=(const FixedGeneric32 &x);
	};

	inline FixedGeneric32 operator+(FixedGeneric32 x, const FixedGeneric32 &y) { return x += y; }
	inline FixedGeneric32 operator-(FixedGeneric32 x, const FixedGeneric32 &y) { return x -= y; }
	inline FixedGeneric32 operator*(FixedGeneric32 x, const FixedGeneric32 &y) { return x *= y; }
}