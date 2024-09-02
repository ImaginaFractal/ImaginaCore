#pragma once

#include <bit>
#include "basic_types.h"

namespace Imagina::inline Numerics {
	struct _FloatF64eI64 {
		double Mantissa;
		int64_t Exponent;
	};

	constexpr ExpInt ExponentOf(double x)	{ return ExpInt((std::bit_cast<uint64_t>(x) >> 52) & 0x7FF) - 0x3FF; }
	constexpr ExpInt ExponentOf(float x)	{ return ExpInt((std::bit_cast<uint32_t>(x) >> 23) & 0x0FF) - 0x07F; }

	// Make sure double have the correct format
	static_assert(std::bit_cast<uint64_t, double>(0x1.3456789ABCDEFp-1005) == 0x0123'4567'89AB'CDEF);

	struct FloatF64eI64 : _FloatF64eI64 {
		constexpr int64_t MantissaI64() { return std::bit_cast<int64_t>(Mantissa); }
		constexpr void MantissaI64(int64_t x) { Mantissa = std::bit_cast<double>(x); }

		static constexpr int64_t SignMask = 1LL << 63;
		static constexpr int64_t ExponentMask = 0x7FFLL << 52;
		static constexpr int64_t MantissaMask = (1LL << 52) - 1;

		static constexpr int64_t ZeroOffset = 0x3FFLL;
		static constexpr int64_t MaxExponent = 0x7FFLL;
		static constexpr int64_t NormalizedExponent = 0x3FFLL;

		static constexpr int64_t ZeroOffsetShifted = ZeroOffset << 52;
		static constexpr int64_t NormalizedExponentShifted = NormalizedExponent << 52;

		static constexpr int64_t ZeroInfExponent = 0x2800'0000'0000'0000;
		static constexpr int64_t ZeroInfExponentThreshold = 0x1000'0000'0000'0000;

		FloatF64eI64() : _FloatF64eI64{ .Mantissa = 1.0, .Exponent = ~ZeroInfExponentThreshold } {}

		constexpr void Normalize() {
			uint64_t MantissaI64 = std::bit_cast<uint64_t>(Mantissa);

#ifndef FLOAT_F64_E_I64_INFINITY
			if (Mantissa == 0.0 || Exponent <= ~ZeroInfExponentThreshold) {
				Exponent = ~ZeroInfExponent;
			} else {
				Exponent += int64_t((MantissaI64 >> 52) & MaxExponent) - NormalizedExponent;
			}
#else
			if (Mantissa == 0.0) [[unlikely]] {
				Exponent = ~ZeroInfExponent;
			} else {
				Exponent += int64_t((MantissaI64 >> 52) & MaxExponent) - NormalizedExponent;

				int64_t sign = Exponent >> 63;
				if ((Exponent ^ sign) >= ZeroInfExponentThreshold) {
					Exponent = ZeroInfExponent ^ sign;
				}
			}
#endif

			MantissaI64 = (MantissaI64 & ~ExponentMask) | NormalizedExponentShifted;
			Mantissa = std::bit_cast<double>(MantissaI64);
		}

		//constexpr FloatF64eI64(float	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		constexpr FloatF64eI64(double	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(int8_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(uint8_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(int16_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(uint16_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(int32_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(uint32_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(int64_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }
		//constexpr FloatF64eI64(uint64_t	x) : _FloatF64eI64{ .Mantissa = double(x), .Exponent = 0 } { Normalize(); }

		constexpr FloatF64eI64(double mantissa, int64_t exponent)		: _FloatF64eI64{ .Mantissa = mantissa, .Exponent = exponent } { Normalize(); }
		constexpr FloatF64eI64(double mantissa, int64_t exponent, int)	: _FloatF64eI64{ .Mantissa = mantissa, .Exponent = exponent } {}

		constexpr explicit operator double() const {
			int64_t MantissaI64 = std::bit_cast<int64_t>(Mantissa);

			if (Exponent >= MaxExponent - NormalizedExponent) {
				int64_t temp = (MantissaI64 | ExponentMask) & ~MantissaMask;
				return std::bit_cast<double>(temp);
			} else if (Exponent <= -NormalizedExponent) {
				return 0.0;
			} else {
				int64_t temp = MantissaI64 + (Exponent << 52);
				return std::bit_cast<double>(temp);
			}
		}

		constexpr bool IsZero() const {
			return Exponent <= ~ZeroInfExponentThreshold;
		}

		constexpr FloatF64eI64 operator+() const {
			return *this;
		}

		constexpr FloatF64eI64 operator-() const {
			return FloatF64eI64(-Mantissa, Exponent, 0);
		}

		constexpr FloatF64eI64 operator+=(const FloatF64eI64 &n) {
			int64_t ExponentDifference = n.Exponent - Exponent;

			// n is too small
			if (ExponentDifference < -0x80) {
				return *this;
			}

			// this is too small
			if (ExponentDifference > 0x80) {
				return *this = n;
			}

			Mantissa += std::bit_cast<double>(std::bit_cast<int64_t>(n.Mantissa) + (ExponentDifference << 52));
			Normalize();

			return *this;
		}

		constexpr FloatF64eI64 operator-=(const FloatF64eI64 &n) {
			int64_t ExponentDifference = n.Exponent - Exponent;

			// n is too small
			if (ExponentDifference < -0x80) {
				return *this;
			}

			// this is too small
			if (ExponentDifference > 0x80) {
				Exponent = n.Exponent;
				Mantissa = -n.Mantissa;
				return *this;
			}

			Mantissa -= std::bit_cast<double>(std::bit_cast<int64_t>(n.Mantissa) + (ExponentDifference << 52));
			Normalize();

			return *this;
		}

		constexpr FloatF64eI64 operator*=(const FloatF64eI64 &n) {
			Mantissa *= n.Mantissa;
			Exponent += n.Exponent;
			Normalize();
			return *this;
		}

		constexpr FloatF64eI64 operator/=(const FloatF64eI64 &n) {
			Mantissa /= n.Mantissa;
			Exponent -= n.Exponent;
			Normalize();
			return *this;
		}
	};

	constexpr bool operator==(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return a.Exponent == b.Exponent && (a.IsZero() || a.Mantissa == b.Mantissa);
	}

	constexpr bool operator!=(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		return a.Exponent != b.Exponent || (!a.IsZero() && a.Mantissa != b.Mantissa);
	}

	constexpr bool operator>(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		bool Positive = a.Mantissa > 0;
		if (Positive == b.Mantissa > 0) {
			return Positive == (a.Exponent > b.Exponent || (a.Exponent == b.Exponent && !a.IsZero() && a.Mantissa > b.Mantissa));
		} else {
			return Positive;
		}
	}

	constexpr bool operator>=(const FloatF64eI64 &a, const FloatF64eI64 &b) {
		bool Positive = a.Mantissa > 0;
		if (Positive == b.Mantissa > 0) {
			return Positive == (a.Exponent > b.Exponent || (a.Exponent == b.Exponent && (a.IsZero() || a.Mantissa >= b.Mantissa)));
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

	constexpr FloatF64eI64 operator+(double a, const FloatF64eI64 &b) { return FloatF64eI64(a) + b; }
	constexpr FloatF64eI64 operator-(double a, const FloatF64eI64 &b) { return FloatF64eI64(a) - b; }
	constexpr FloatF64eI64 operator*(double a, const FloatF64eI64 &b) { return FloatF64eI64(a) * b; }
	constexpr FloatF64eI64 operator/(double a, const FloatF64eI64 &b) { return FloatF64eI64(a) / b; }

	constexpr FloatF64eI64 operator+(const FloatF64eI64 &a, double b) { return a + FloatF64eI64(b); }
	constexpr FloatF64eI64 operator-(const FloatF64eI64 &a, double b) { return a - FloatF64eI64(b); }
	constexpr FloatF64eI64 operator*(const FloatF64eI64 &a, double b) { return a * FloatF64eI64(b); }
	constexpr FloatF64eI64 operator/(const FloatF64eI64 &a, double b) { return a / FloatF64eI64(b); }

	//constexpr FloatF64eI64 operator+(const int &a, const FloatF64eI64 &b) { return (FloatF64eI64(a)) + b; }
	//constexpr FloatF64eI64 operator-(const int &a, const FloatF64eI64 &b) { return (FloatF64eI64(a)) - b; }
	//constexpr FloatF64eI64 operator*(const int &a, const FloatF64eI64 &b) { return (FloatF64eI64(a)) * b; }
	//constexpr FloatF64eI64 operator/(const int &a, const FloatF64eI64 &b) { return (FloatF64eI64(a)) / b; }

	//inline mpf_class &operator+=(mpf_class &a, const FloatF64eI64 &b) { return a += b.operator mpf_class(); }
	//inline mpf_class &operator-=(mpf_class &a, const FloatF64eI64 &b) { return a -= b.operator mpf_class(); }
	//inline mpf_class &operator*=(mpf_class &a, const FloatF64eI64 &b) { return a *= b.operator mpf_class(); }
	//inline mpf_class &operator/=(mpf_class &a, const FloatF64eI64 &b) { return a /= b.operator mpf_class(); }
	//
	//inline mpf_class operator+=(const mpf_class &a, const FloatF64eI64 &b) { return a + b.operator mpf_class(); }
	//inline mpf_class operator-=(const mpf_class &a, const FloatF64eI64 &b) { return a - b.operator mpf_class(); }
	//inline mpf_class operator*=(const mpf_class &a, const FloatF64eI64 &b) { return a * b.operator mpf_class(); }
	//inline mpf_class operator/=(const mpf_class &a, const FloatF64eI64 &b) { return a / b.operator mpf_class(); }

	constexpr ExpInt ExponentOf(FloatF64eI64 x) { return x.Exponent; }
}