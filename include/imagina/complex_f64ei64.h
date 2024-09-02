#pragma once

#include <type_traits>
#include "floating_point.h"
#include "complex.h"

namespace Imagina::inline Numerics {
	struct _ComplexF64eI64Base {
		double re, im;
		int64_t exponent;

		static constexpr bool NormalizeTo0x400 = false;

		static constexpr int64_t SignMask = 1LL << 63;
		static constexpr int64_t ExponentMask = 0x7FFLL << 52;
		static constexpr int64_t MantissaMask = (1LL << 52) - 1;

		static constexpr int64_t ZeroOffset = 0x3FFLL;
		static constexpr int64_t MaxExponent = 0x7FFLL;
		static constexpr int64_t NormalizedExponent = NormalizeTo0x400 ? 0x400LL : 0x3FFLL;

		static constexpr int64_t ZeroOffsetShifted = ZeroOffset << 52;
		static constexpr int64_t NormalizedExponentShifted = NormalizedExponent << 52;

		static constexpr int64_t ZeroInfExponent = 0x2800'0000'0000'0000;
		static constexpr int64_t ZeroInfExponentThreshold = 0x1000'0000'0000'0000;

		static constexpr int64_t MaxExpDeviation = 0x1D0; // 0x1D0 * 2 + 0x40 < 0x3FF

		_ComplexF64eI64Base() = default;

		constexpr _ComplexF64eI64Base(double re, double im, int64_t exponent) : re(re), im(im), exponent(exponent) {}
	};

	template<size_t exp_deviation>
	struct _ComplexF64eI64U : _ComplexF64eI64Base {
		_ComplexF64eI64U() = default;

		using _ComplexF64eI64Base::_ComplexF64eI64Base;

		constexpr _ComplexF64eI64U(const _ComplexF64eI64Base &x) : _ComplexF64eI64Base(x) {}
		
		constexpr _ComplexF64eI64U operator-() const {
			return _ComplexF64eI64U{ -re, -im, exponent };
		}
	};

	struct ComplexF64eI64 : _ComplexF64eI64U<0> {
		ComplexF64eI64() = default;

		constexpr ComplexF64eI64(FloatF64eI64 re) noexcept : _ComplexF64eI64U(re.Mantissa, 0.0, re.Exponent) {}

		constexpr ComplexF64eI64(double re) noexcept : ComplexF64eI64(FloatF64eI64(re)) {}
		constexpr ComplexF64eI64(double re, double im) noexcept : _ComplexF64eI64U(re, im, 0) { Normalize(); }

		constexpr ComplexF64eI64(Complex<double> x) noexcept : ComplexF64eI64(x.re, x.im) {}

		constexpr ComplexF64eI64(double re, double im, int64_t exponent)		: _ComplexF64eI64U(re, im, exponent) { Normalize(); }
		constexpr ComplexF64eI64(double re, double im, int64_t exponent, int)	: _ComplexF64eI64U(re, im, exponent) {}

		constexpr ComplexF64eI64(const _ComplexF64eI64Base &x) : _ComplexF64eI64U(x.re, x.im, x.exponent) { Normalize(); }

		template<size_t exp_deviation>
		constexpr ComplexF64eI64(const _ComplexF64eI64U<exp_deviation> &x) : _ComplexF64eI64U(x.re, x.im, x.exponent) { Normalize(); }

		constexpr void Normalize() {
			double max = std::max(std::abs(re), std::abs(im));

			uint64_t max_i64 = std::bit_cast<uint64_t>(max);

			if (max == 0.0 || exponent <= ~ZeroInfExponentThreshold) {
				exponent = ~ZeroInfExponent;
				re = 1.0;
				im = 1.0;
			} else {
				uint64_t re_i64 = std::bit_cast<uint64_t>(re);
				uint64_t im_i64 = std::bit_cast<uint64_t>(im);

				//int64_t adjustment = int64_t((max_i64 >> 52) & MaxExponent) - NormalizedExponent;
				int64_t adjustment = int64_t(max_i64 & ExponentMask) - NormalizedExponentShifted;
				exponent += adjustment >> 52;

				uint64_t new_re_i64 = re_i64 - adjustment;
				uint64_t new_im_i64 = im_i64 - adjustment;

				// Underflow. It will never overflow
				new_re_i64 &= ~int64_t(new_re_i64 ^ re_i64) >> 63;
				new_im_i64 &= ~int64_t(new_im_i64 ^ im_i64) >> 63;
				// Equivalent to:
				// if (int64_t(new_re_i64 ^ re_i64) < 0) new_re_i64 = 0;
				// if (int64_t(new_im_i64 ^ im_i64) < 0) new_im_i64 = 0;
				// or
				// if ((new_re_i64 >> 63) != (re_i64 >> 63)) new_re_i64 = 0;
				// if ((new_im_i64 >> 63) != (im_i64 >> 63)) new_im_i64 = 0;
				// Clang will generate the same machine code for all of these, but gcc won't eliminate the branch

				re = std::bit_cast<double>(new_re_i64);
				im = std::bit_cast<double>(new_im_i64);
			}
		}

		constexpr ComplexF64eI64 operator-() const {
			return ComplexF64eI64(-re, -im, exponent, 0);
		}

		static constexpr size_t exp_deviation_after_add(size_t exp_deviation1, size_t exp_deviation2) {
			return std::max(exp_deviation1, exp_deviation2) * 2 + 0x40;
		}

		static constexpr size_t exp_deviation_after_mul(size_t exp_deviation1, size_t exp_deviation2) {
			return exp_deviation1 + exp_deviation2 + 1;
		}

		template<size_t exp_deviation1, size_t exp_deviation2, size_t (*deviation_calc_func)(size_t, size_t)>
		using arithmetic_result_t = std::conditional_t<
			(deviation_calc_func(exp_deviation1, exp_deviation2) > MaxExpDeviation),
			ComplexF64eI64, // Normalization done on construction
			_ComplexF64eI64U<deviation_calc_func(exp_deviation1, exp_deviation2)> >;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using add_result_t = arithmetic_result_t<exp_deviation1, exp_deviation2, exp_deviation_after_add>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using sub_result_t = add_result_t<exp_deviation1, exp_deviation2>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using mul_result_t = arithmetic_result_t<exp_deviation1, exp_deviation2, exp_deviation_after_mul>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using div_result_t = ComplexF64eI64; // TODO: Improve this
		// The problem with division is it can introduce negative deviation which other operations can't handel (yet)
	};

	static_assert(ComplexF64eI64::exp_deviation_after_add(ComplexF64eI64::MaxExpDeviation, ComplexF64eI64::MaxExpDeviation) < 0x3FF);
	static_assert(ComplexF64eI64::exp_deviation_after_mul(ComplexF64eI64::MaxExpDeviation, ComplexF64eI64::MaxExpDeviation) < 0x3FF);

	// FIXME: Return value may have zero mantissa
	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr ComplexF64eI64::add_result_t<exp_deviation1, exp_deviation2>
		operator+(const _ComplexF64eI64U<exp_deviation1> &x, const _ComplexF64eI64U<exp_deviation2> &y) {
		const _ComplexF64eI64Base *large, *small;

		if (x.exponent >= y.exponent) {
			large = &x;
			small = &y;
		} else {
			large = &y;
			small = &x;
		}

		int64_t ExponentDifference = large->exponent - small->exponent;

		if (ExponentDifference > std::max(exp_deviation1, exp_deviation2) + 0x40) {
			return *large;
		}

		double re = small->re + std::bit_cast<double>(std::bit_cast<int64_t>(large->re) + (ExponentDifference << 52));
		double im = small->im + std::bit_cast<double>(std::bit_cast<int64_t>(large->im) + (ExponentDifference << 52));

		int64_t exponent = small->exponent;

		return { re, im, exponent };
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr ComplexF64eI64::sub_result_t<exp_deviation1, exp_deviation2>
		operator-(const _ComplexF64eI64U<exp_deviation1> &x, const _ComplexF64eI64U<exp_deviation2> &y) {
		return x + (-y);
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr ComplexF64eI64::mul_result_t<exp_deviation1, exp_deviation2>
		operator*(const _ComplexF64eI64U<exp_deviation1> &x, const _ComplexF64eI64U<exp_deviation2> &y) {
		return {
			x.re * y.re - x.im * y.im,
			x.re * y.im + x.im * y.re,
			x.exponent + y.exponent
		};
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr ComplexF64eI64::div_result_t<exp_deviation1, exp_deviation2>
		operator/(const _ComplexF64eI64U<exp_deviation1> &x, const _ComplexF64eI64U<exp_deviation2> &y) {
		double re = x.re * y.re + x.im * y.im;
		double im = x.im * y.re - x.re * y.im;
		double y_norm = y.re * y.re + y.im * y.im;

		return { re / y_norm, im / y_norm, x.exponent - y.exponent };
	}

	template<size_t exp_deviation> constexpr ComplexF64eI64 &operator+=(ComplexF64eI64 &x, const _ComplexF64eI64U<exp_deviation> &y) { return x = x + y; }
	template<size_t exp_deviation> constexpr ComplexF64eI64 &operator-=(ComplexF64eI64 &x, const _ComplexF64eI64U<exp_deviation> &y) { return x = x - y; }
	template<size_t exp_deviation> constexpr ComplexF64eI64 &operator*=(ComplexF64eI64 &x, const _ComplexF64eI64U<exp_deviation> &y) { return x = x * y; }
	template<size_t exp_deviation> constexpr ComplexF64eI64 &operator/=(ComplexF64eI64 &x, const _ComplexF64eI64U<exp_deviation> &y) { return x = x / y; }

	constexpr FloatF64eI64 norm(const ComplexF64eI64 &x) {
		return FloatF64eI64(x.re * x.re + x.im * x.im, x.exponent * 2);
	}

	constexpr FloatF64eI64 chebyshev_norm(const ComplexF64eI64 &x) {
		return FloatF64eI64(std::max(std::abs(x.re), std::abs(x.im)), x.exponent, 0);
	}
}