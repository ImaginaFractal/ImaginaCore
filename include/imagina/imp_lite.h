#pragma once

#include <cstdint>
#include "multi-precision.h"
#include "float_f64ei64.h"

namespace Imagina::MPLite {
	class im_export Float {
		static constexpr int32_t BufferSize = 7 * sizeof(void*) / sizeof(uint32_t) - 2;
		static_assert(BufferSize > 0);

		union {
			uint32_t Buffer[BufferSize];
			uint32_t *Pointer;
		};
		int32_t Exponent;
		union {
			struct {
				uint32_t Size : 31;
				uint32_t Sign : 1;
			};
			uint32_t SignSize;
		};

		uint32_t *Data() { return (Size > BufferSize) ? Pointer : Buffer; }
		const uint32_t *Data() const { return (Size > BufferSize) ? Pointer : Buffer; }

		static void _Init(Float *x, uint32_t size);

	public:
		static void Init(Float *x, uintptr_t precision);
		static void InitCopy(Float *x, const Float *src);

		static void Clear(Float *x);

		static uintptr_t get_precision(const Float *x);
		static void set_precision(Float *x, uintptr_t precision);

		static void set(Float *x, const Float *src);
		static void copy(Float *x, const Float *src);
		static void SetU32(Float *x, uint32_t u32);
		static void set_double(Float *x, double d);
		static void set_double_2exp(Float *x, double d, int_exp exp);
		static void set_float_f64ei64(Float *x, Imagina::float_f64ei64 f);
		static void set_string(Float *x, const char *str, int base);

		static double get_double(const Float *x);
		static double get_double_2exp(int_exp *exp, const Float *x);
		static Imagina::float_f64ei64 get_float_f64ei64(const Float *x);

		static void MulU32(Float *result, const Float *x, uint32_t y);
		static void U32PowU32(Float *result, uint32_t x, uint32_t y);

		static void Neg(Float *result, const Float *x);
		static void Abs(Float *result, const Float *x);
		static void add(Float *result, const Float *x, const Float *y);
		static void sub(Float *result, const Float *x, const Float *y);
		static void mul(Float *result, const Float *x, const Float *y);
		static void div(Float *result, const Float *x, const Float *y);

		static bool MagnitudeGreater(const Float *x, const Float *y);

	private:
		inline static void UnsignedSet(Float *x, const Float *src);
		template<bool IsAdd>
		static void UnsignedAddSub(Float *result, const Float *x, const Float *y);
		static void UnsignedAdd(Float *result, const Float *x, const Float *y);
		static void UnsignedSub(Float *result, const Float *x, const Float *y);
	};

	static_assert(sizeof(Float) <= sizeof(void *) * 7);
}

namespace Imagina {
	extern im_export multi_precision IMPLite;
}