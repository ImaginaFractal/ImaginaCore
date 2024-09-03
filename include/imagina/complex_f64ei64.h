#pragma once

#include <type_traits>
#include "floating_point.h"
#include "complex.h"

namespace Imagina::inline Numerics {
	struct _complex_f64ei64_base {
		double re, im;
		int64_t exponent;

		static constexpr int64_t sign_mask = 1LL << 63;
		static constexpr int64_t exponent_mask = 0x7FFLL << 52;
		static constexpr int64_t mantissa_mask = (1LL << 52) - 1;

		static constexpr int64_t zero_offset = 0x3FFLL;
		static constexpr int64_t max_exponent = 0x7FFLL;
		static constexpr int64_t normalized_exponent = 0x3FFLL;

		static constexpr int64_t zero_offset_shifted = zero_offset << 52;
		static constexpr int64_t normalized_exponent_shifted = normalized_exponent << 52;

		static constexpr int64_t zero_inf_exponent = 0x2800'0000'0000'0000;
		static constexpr int64_t zero_inf_exponent_threshold = 0x1000'0000'0000'0000;

		static constexpr int64_t max_exp_deviation = 0x1D0; // 0x1D0 * 2 + 0x40 < 0x3FF

		_complex_f64ei64_base() = default;

		constexpr _complex_f64ei64_base(double re, double im, int64_t exponent) : re(re), im(im), exponent(exponent) {}
	};

	template<size_t exp_deviation>
	struct _complex_f64ei64_u : _complex_f64ei64_base {
		_complex_f64ei64_u() = default;

		using _complex_f64ei64_base::_complex_f64ei64_base;

		constexpr _complex_f64ei64_u(const _complex_f64ei64_base &x) : _complex_f64ei64_base(x) {}
		
		constexpr _complex_f64ei64_u operator-() const {
			return _complex_f64ei64_u{ -re, -im, exponent };
		}
	};

	struct complex_f64ei64 : _complex_f64ei64_u<0> {
		complex_f64ei64() = default;

		constexpr complex_f64ei64(float_f64ei64 re) noexcept : _complex_f64ei64_u(re.mantissa, 0.0, re.exponent) {}

		constexpr complex_f64ei64(double re) noexcept : complex_f64ei64(float_f64ei64(re)) {}
		constexpr complex_f64ei64(double re, double im) noexcept : _complex_f64ei64_u(re, im, 0) { normalize(); }

		constexpr complex_f64ei64(Complex<double> x) noexcept : complex_f64ei64(x.re, x.im) {}

		constexpr complex_f64ei64(double re, double im, int64_t exponent)		: _complex_f64ei64_u(re, im, exponent) { normalize(); }
		constexpr complex_f64ei64(double re, double im, int64_t exponent, int)	: _complex_f64ei64_u(re, im, exponent) {}

		constexpr complex_f64ei64(const _complex_f64ei64_base &x) : _complex_f64ei64_u(x.re, x.im, x.exponent) { normalize(); }

		template<size_t exp_deviation>
		constexpr complex_f64ei64(const _complex_f64ei64_u<exp_deviation> &x) : _complex_f64ei64_u(x.re, x.im, x.exponent) { normalize(); }

		constexpr void normalize() {
			double max = std::max(std::abs(re), std::abs(im));

			uint64_t max_i64 = std::bit_cast<uint64_t>(max);

			if (max == 0.0 || exponent <= ~zero_inf_exponent_threshold) {
				exponent = ~zero_inf_exponent;
				re = 1.0;
				im = 1.0;
			} else {
				uint64_t re_i64 = std::bit_cast<uint64_t>(re);
				uint64_t im_i64 = std::bit_cast<uint64_t>(im);

				//int64_t adjustment = int64_t((max_i64 >> 52) & max_exponent) - normalized_exponent;
				int64_t adjustment = int64_t(max_i64 & exponent_mask) - normalized_exponent_shifted;
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

		constexpr complex_f64ei64 operator-() const {
			return complex_f64ei64(-re, -im, exponent, 0);
		}

		static constexpr size_t exp_deviation_after_add(size_t exp_deviation1, size_t exp_deviation2) {
			return std::max(exp_deviation1, exp_deviation2) * 2 + 0x40;
		}

		static constexpr size_t exp_deviation_after_mul(size_t exp_deviation1, size_t exp_deviation2) {
			return exp_deviation1 + exp_deviation2 + 1;
		}

		template<size_t exp_deviation1, size_t exp_deviation2, size_t (*deviation_calc_func)(size_t, size_t)>
		using arithmetic_result_t = std::conditional_t<
			(deviation_calc_func(exp_deviation1, exp_deviation2) > max_exp_deviation),
			complex_f64ei64, // Normalization done on construction
			_complex_f64ei64_u<deviation_calc_func(exp_deviation1, exp_deviation2)> >;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using add_result_t = arithmetic_result_t<exp_deviation1, exp_deviation2, exp_deviation_after_add>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using sub_result_t = add_result_t<exp_deviation1, exp_deviation2>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using mul_result_t = arithmetic_result_t<exp_deviation1, exp_deviation2, exp_deviation_after_mul>;

		template<size_t exp_deviation1, size_t exp_deviation2>
		using div_result_t = complex_f64ei64; // TODO: Improve this
		// The problem with division is it can introduce negative deviation which other operations can't handel (yet)
	};

	static_assert(complex_f64ei64::exp_deviation_after_add(complex_f64ei64::max_exp_deviation, complex_f64ei64::max_exp_deviation) < 0x3FF);
	static_assert(complex_f64ei64::exp_deviation_after_mul(complex_f64ei64::max_exp_deviation, complex_f64ei64::max_exp_deviation) < 0x3FF);

	// FIXME: Return value may have zero mantissa
	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr complex_f64ei64::add_result_t<exp_deviation1, exp_deviation2>
		operator+(const _complex_f64ei64_u<exp_deviation1> &x, const _complex_f64ei64_u<exp_deviation2> &y) {
		const _complex_f64ei64_base *large, *small;

		if (x.exponent >= y.exponent) {
			large = &x;
			small = &y;
		} else {
			large = &y;
			small = &x;
		}

		int64_t exponent_difference = large->exponent - small->exponent;

		if (exponent_difference > std::max(exp_deviation1, exp_deviation2) + 0x40) {
			return *large;
		}

		double re = small->re + std::bit_cast<double>(std::bit_cast<int64_t>(large->re) + (exponent_difference << 52));
		double im = small->im + std::bit_cast<double>(std::bit_cast<int64_t>(large->im) + (exponent_difference << 52));

		int64_t exponent = small->exponent;

		return { re, im, exponent };
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr complex_f64ei64::sub_result_t<exp_deviation1, exp_deviation2>
		operator-(const _complex_f64ei64_u<exp_deviation1> &x, const _complex_f64ei64_u<exp_deviation2> &y) {
		return x + (-y);
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr complex_f64ei64::mul_result_t<exp_deviation1, exp_deviation2>
		operator*(const _complex_f64ei64_u<exp_deviation1> &x, const _complex_f64ei64_u<exp_deviation2> &y) {
		return {
			x.re * y.re - x.im * y.im,
			x.re * y.im + x.im * y.re,
			x.exponent + y.exponent
		};
	}

	template<size_t exp_deviation1, size_t exp_deviation2>
	constexpr complex_f64ei64::div_result_t<exp_deviation1, exp_deviation2>
		operator/(const _complex_f64ei64_u<exp_deviation1> &x, const _complex_f64ei64_u<exp_deviation2> &y) {
		double re = x.re * y.re + x.im * y.im;
		double im = x.im * y.re - x.re * y.im;
		double y_norm = y.re * y.re + y.im * y.im;

		return { re / y_norm, im / y_norm, x.exponent - y.exponent };
	}

	template<size_t exp_deviation> constexpr complex_f64ei64 &operator+=(complex_f64ei64 &x, const _complex_f64ei64_u<exp_deviation> &y) { return x = x + y; }
	template<size_t exp_deviation> constexpr complex_f64ei64 &operator-=(complex_f64ei64 &x, const _complex_f64ei64_u<exp_deviation> &y) { return x = x - y; }
	template<size_t exp_deviation> constexpr complex_f64ei64 &operator*=(complex_f64ei64 &x, const _complex_f64ei64_u<exp_deviation> &y) { return x = x * y; }
	template<size_t exp_deviation> constexpr complex_f64ei64 &operator/=(complex_f64ei64 &x, const _complex_f64ei64_u<exp_deviation> &y) { return x = x / y; }

	constexpr float_f64ei64 norm(const complex_f64ei64 &x) {
		return float_f64ei64(x.re * x.re + x.im * x.im, x.exponent * 2);
	}

	constexpr float_f64ei64 chebyshev_norm(const complex_f64ei64 &x) {
		return float_f64ei64(std::max(std::abs(x.re), std::abs(x.im)), x.exponent, 0);
	}
}