#pragma once

#include "floating_point.h"
#include "complex.h"

namespace Imagina::inline Numerics {
	struct ComplexF64eI64 {
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

		//ComplexF64eI64() : re(1.0), im(1.0), Exponent(~ZeroInfExponentThreshold) {}
		ComplexF64eI64() = default;

		constexpr ComplexF64eI64(FloatF64eI64 re) noexcept : re(re.Mantissa), im(0.0), exponent(re.Exponent) {}

		constexpr ComplexF64eI64(double re) noexcept : ComplexF64eI64(FloatF64eI64(re)) {}
		constexpr ComplexF64eI64(double re, double im) noexcept : re(re), im(im), exponent(0) { Normalize(); }

		constexpr ComplexF64eI64(Complex<double> x) noexcept : ComplexF64eI64(x.re, x.im) {}

		constexpr ComplexF64eI64(double re, double im, int64_t exponent)		: re(re), im(im), exponent(exponent) { Normalize(); }
		constexpr ComplexF64eI64(double re, double im, int64_t exponent, int)	: re(re), im(im), exponent(exponent) {}


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

		constexpr ComplexF64eI64 &operator+=(const ComplexF64eI64 &x) {
			const ComplexF64eI64 *large, *small;

			if (exponent >= x.exponent) {
				large = this;
				small = &x;
			} else {
				large = &x;
				small = this;
			}

			int64_t ExponentDifference = large->exponent - small->exponent;

			if (ExponentDifference > 0x80) {
				*this = *large;
				Normalize(); // TODO: Optimize this
				return *this;
			}

			re = small->re + std::bit_cast<double>(std::bit_cast<int64_t>(large->re) + (ExponentDifference << 52));
			im = small->im + std::bit_cast<double>(std::bit_cast<int64_t>(large->im) + (ExponentDifference << 52));

			exponent = small->exponent;

			Normalize();

			return *this;
		}

		constexpr ComplexF64eI64 &operator-=(const ComplexF64eI64 &x) {
			return *this += -x;
		}

		constexpr ComplexF64eI64 &operator*=(const ComplexF64eI64 &x) {
			double new_re = re * x.re - im * x.im;
			double new_im = re * x.im + im * x.re;

			re = new_re;
			im = new_im;
			exponent += x.exponent;

			return *this;
		}

		constexpr ComplexF64eI64 &operator/=(const ComplexF64eI64 &x) {
			double new_re = re * x.re + im * x.im;
			double new_im = im * x.re - re * x.im;
			double x_norm = x.re * x.re + x.im * x.im;

			re = new_re / x_norm;
			im = new_im / x_norm;
			exponent -= x.exponent;

			return *this;
		}
	};

	constexpr ComplexF64eI64 operator+(ComplexF64eI64 a, const ComplexF64eI64 &b) { return a += b; }
	constexpr ComplexF64eI64 operator-(ComplexF64eI64 a, const ComplexF64eI64 &b) { return a -= b; }
	constexpr ComplexF64eI64 operator*(ComplexF64eI64 a, const ComplexF64eI64 &b) { return a *= b; }
	constexpr ComplexF64eI64 operator/(ComplexF64eI64 a, const ComplexF64eI64 &b) { return a /= b; }

	constexpr FloatF64eI64 norm(const ComplexF64eI64 &x) {
		return FloatF64eI64(x.re * x.re + x.im * x.im, x.exponent * 2);
	}

	constexpr FloatF64eI64 chebyshev_norm(const ComplexF64eI64 &x) {
		return FloatF64eI64(std::max(std::abs(x.re), std::abs(x.im)), x.exponent, 0);
	}
}