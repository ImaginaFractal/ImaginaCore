#pragma once

#include <bit>
#include "basic_types.h"
#include "constants.h"

namespace Imagina::inline Numerics {
	struct _FloatF64eI64 {
		double mantissa;
		int64_t exponent;
	};

	constexpr ExpInt ExponentOf(double x)	{ return ExpInt((std::bit_cast<uint64_t>(x) >> 52) & 0x7FF) - 0x3FF; }
	constexpr ExpInt ExponentOf(float x)	{ return ExpInt((std::bit_cast<uint32_t>(x) >> 23) & 0x0FF) - 0x07F; }

	// Make sure double have the correct format
	static_assert(std::bit_cast<uint64_t, double>(0x1.3456789ABCDEFp-1005) == 0x0123'4567'89AB'CDEF);

	struct FloatF64eI64 : _FloatF64eI64 {
		constexpr int64_t mantissa_i64() { return std::bit_cast<int64_t>(mantissa); }
		constexpr void mantissa_i64(int64_t x) { mantissa = std::bit_cast<double>(x); }

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

		FloatF64eI64() = default;

		constexpr void normalize() {
			uint64_t mantissa_i64 = std::bit_cast<uint64_t>(mantissa);

#ifndef FLOAT_F64_E_I64_INFINITY
			if (mantissa == 0.0 || exponent <= ~zero_inf_exponent_threshold) {
				exponent = ~zero_inf_exponent;
			} else {
				exponent += int64_t((mantissa_i64 >> 52) & max_exponent) - normalized_exponent;
			}
#else
			if (mantissa == 0.0) [[unlikely]] {
				exponent = ~zero_inf_exponent;
			} else {
				exponent += int64_t((mantissa_i64 >> 52) & max_exponent) - normalized_exponent;

				int64_t sign = exponent >> 63;
				if ((exponent ^ sign) >= zero_inf_exponent_threshold) {
					exponent = zero_inf_exponent ^ sign;
				}
			}
#endif

			mantissa_i64 = (mantissa_i64 & ~exponent_mask) | normalized_exponent_shifted;
			mantissa = std::bit_cast<double>(mantissa_i64);
		}

		template<typename T>
		constexpr FloatF64eI64(T x) requires std::integral<T> || std::floating_point<T>
			: _FloatF64eI64{ .mantissa = double(x), .exponent = 0 } {
			normalize();
		}

		template<intmax_t value>
		constexpr FloatF64eI64(Constants::IntegerConstant<value> x) : _FloatF64eI64{ .mantissa = double(value), .exponent = 0 } {
			normalize();
		}

		constexpr FloatF64eI64(double mantissa, int64_t exponent)		: _FloatF64eI64{ .mantissa = mantissa, .exponent = exponent } { normalize(); }
		constexpr FloatF64eI64(double mantissa, int64_t exponent, int)	: _FloatF64eI64{ .mantissa = mantissa, .exponent = exponent } {}

		constexpr explicit operator double() const {
			int64_t mantissa_i64 = std::bit_cast<int64_t>(mantissa);

			if (exponent >= max_exponent - normalized_exponent) {
				int64_t temp = (mantissa_i64 | exponent_mask) & ~mantissa_mask;
				return std::bit_cast<double>(temp);
			} else if (exponent <= -normalized_exponent) {
				return 0.0;
			} else {
				int64_t temp = mantissa_i64 + (exponent << 52);
				return std::bit_cast<double>(temp);
			}
		}

		constexpr bool is_zero() const {
			return exponent <= ~zero_inf_exponent_threshold;
		}

		constexpr FloatF64eI64 operator+() const {
			return *this;
		}

		constexpr FloatF64eI64 operator-() const {
			return FloatF64eI64(-mantissa, exponent, 0);
		}

		constexpr FloatF64eI64 operator+=(const FloatF64eI64 &n) {
			int64_t ExponentDifference = n.exponent - exponent;

			// n is too small
			if (ExponentDifference < -0x80) {
				return *this;
			}

			// this is too small
			if (ExponentDifference > 0x80) {
				return *this = n;
			}

			mantissa += std::bit_cast<double>(std::bit_cast<int64_t>(n.mantissa) + (ExponentDifference << 52));
			normalize();

			return *this;
		}

		constexpr FloatF64eI64 operator-=(const FloatF64eI64 &n) {
			int64_t ExponentDifference = n.exponent - exponent;

			// n is too small
			if (ExponentDifference < -0x80) {
				return *this;
			}

			// this is too small
			if (ExponentDifference > 0x80) {
				exponent = n.exponent;
				mantissa = -n.mantissa;
				return *this;
			}

			mantissa -= std::bit_cast<double>(std::bit_cast<int64_t>(n.mantissa) + (ExponentDifference << 52));
			normalize();

			return *this;
		}

		constexpr FloatF64eI64 operator*=(const FloatF64eI64 &n) {
			mantissa *= n.mantissa;
			exponent += n.exponent;
			normalize();
			return *this;
		}

		constexpr FloatF64eI64 operator/=(const FloatF64eI64 &n) {
			mantissa /= n.mantissa;
			exponent -= n.exponent;
			normalize();
			return *this;
		}
	};

	constexpr bool operator==(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return a.exponent == b.exponent && (a.is_zero() || a.mantissa == b.mantissa);
	}

	constexpr bool operator!=(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return a.exponent != b.exponent || (!a.is_zero() && a.mantissa != b.mantissa);
	}

	constexpr bool operator>(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		bool Positive = a.mantissa > 0;
		if (Positive == b.mantissa > 0) {
			return Positive == (a.exponent > b.exponent || (a.exponent == b.exponent && !a.is_zero() && a.mantissa > b.mantissa));
		} else {
			return Positive;
		}
	}

	constexpr bool operator>=(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		bool Positive = a.mantissa > 0;
		if (Positive == b.mantissa > 0) {
			return Positive == (a.exponent > b.exponent || (a.exponent == b.exponent && (a.is_zero() || a.mantissa >= b.mantissa)));
		} else {
			return Positive;
		}
	}

	constexpr bool operator<(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return b > a;
	}

	constexpr bool operator<=(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return b >= a;
	}


	constexpr FloatF64eI64 operator+(FloatF64eI64 a, const FloatF64eI64 &b) { return a += b; }
	constexpr FloatF64eI64 operator-(FloatF64eI64 a, const FloatF64eI64 &b) { return a -= b; }
	constexpr FloatF64eI64 operator*(FloatF64eI64 a, const FloatF64eI64 &b) { return a *= b; }
	constexpr FloatF64eI64 operator/(FloatF64eI64 a, const FloatF64eI64 &b) { return a /= b; }

	constexpr ExpInt ExponentOf(FloatF64eI64 x) { return x.exponent; }
}