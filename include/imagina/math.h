#pragma once

#include <cmath>
#include <algorithm>
#include <concepts>
#include <limits>
#include <cassert>
#include "numerics.h"
#include "constants.h"

namespace Imagina {
	using std::abs;
	using std::max;

	inline FloatF64eI64 abs(FloatF64eI64 x) {
		x.mantissa = std::abs(x.mantissa);
		return x;
	}

	inline FloatF64eI64 sqrt(FloatF64eI64 x) {
		FloatF64eI64 result;
		result.mantissa_i64(x.mantissa_i64() + ((x.exponent & 1) << 52));
		result.exponent = x.exponent >> 1;
		result.mantissa = std::sqrt(result.mantissa);
		result.normalize();
		return result;
	}

	inline double log(const FloatF64eI64 &n) {
		constexpr double ln_2 = 0.693147180559945309417;
		return std::log(n.mantissa) + n.exponent * ln_2;
	}

	inline double log2(const FloatF64eI64 &n) {
		return std::log2(n.mantissa) + n.exponent;
	}

	inline double log10(const FloatF64eI64 &n) {
		constexpr double log10_2 = 0.30102999566398119521373889472449;
		return std::log10(n.mantissa) + n.exponent * log10_2;
	}

	inline FloatF64eI64 hypot(FloatF64eI64 x, FloatF64eI64 y) {
		return sqrt(x * x + y * y);
	}

	inline double atan2(FloatF64eI64 y, FloatF64eI64 x) {
		double y2, x2;
		y2 = y.mantissa;
		x2 = x.mantissa;

		int64_t exponent_difference = x.exponent - y.exponent;
		if (x.exponent >= y.exponent) {
			if (exponent_difference <= 0x200) {
				reinterpret_cast<int64_t &>(y2) -= exponent_difference << 52;
			} else {
				y = 0.0;
			}
		} else {
			if (exponent_difference >= -0x200) {
				reinterpret_cast<int64_t &>(x2) += exponent_difference << 52;
			} else {
				x = 0.0;
			}
		}
		return std::atan2(y2, x2);
	}

	template<typename T> inline T manhattan_hypot(const T &x, const T &y) {
		return abs(x) + abs(y);
	}

	template<typename T> inline T chebyshev_hypot(const T &x, const T &y) {
		return max(abs(x), abs(y));
	}

	template<typename T> inline T manhattan_norm(const Complex<T> &x) {
		return manhattan_hypot(x.re, x.im);
	}

	template<typename T> inline T chebyshev_norm(const Complex<T> &x) { // FIXME: Rename
		return chebyshev_hypot(x.re, x.im);
	}

	template<std::floating_point T> inline T abs(const Complex<T> &x) {
		return std::hypot(x.re, x.im);
	}

	inline FloatF64eI64 abs(const Complex<FloatF64eI64> &x) {
		return hypot(x.re, x.im);
	}

	template<std::floating_point T> inline T arg(const Complex<T> &x) {
		return std::atan2(x.im, x.re);
	}

	inline FloatF64eI64 arg(const Complex<FloatF64eI64> &x) {
		return atan2(x.im, x.re);
	}

	template<std::floating_point T> inline Complex<T> log(const Complex<T> &x) {
		return { std::log(abs(x)), std::atan2(x.im, x.re) };
	}

	inline Complex<double> log(const Complex<FloatF64eI64> &x) {
		return { log(norm(x)) * Constants::Half, atan2(x.im, x.re) };
	}

	template<typename T1, typename T2>
	T1 lerp(T1 a, T1 b, T2 t) {
		return a * (T2(1) - t) + b * t;
	}

	inline std::pair<double, double> _cmp_abs_common(const Complex<double> &a, const Complex<double> &b) {
		double ar = std::abs(a.re), ai = std::abs(a.im);
		double br = std::abs(b.re), bi = std::abs(b.im);
		double max = std::max(std::max(ar, ai), std::max(br, bi));
		assert(std::isfinite(max));
		double scale = std::bit_cast<double>(std::bit_cast<uint64_t>(std::numeric_limits<double>::max()) - std::bit_cast<uint64_t>(max));

		ar *= scale;
		ai *= scale;
		br *= scale;
		bi *= scale;

		return { ar * ar + ai * ai, br * br + bi * bi };
	}

	inline bool abs_lt(const Complex<double> &a, const Complex<double> &b) {
		auto [abs_a, abs_b] = _cmp_abs_common(a, b);
		return abs_a < abs_b;
	}

	inline bool abs_le(const Complex<double> &a, const Complex<double> &b) {
		auto [abs_a, abs_b] = _cmp_abs_common(a, b);
		return abs_a <= abs_b;
	}

	inline bool abs_gt(const Complex<double> &a, const Complex<double> &b) {
		auto [abs_a, abs_b] = _cmp_abs_common(a, b);
		return abs_a > abs_b;
	}

	inline bool abs_ge(const Complex<double> &a, const Complex<double> &b) {
		auto [abs_a, abs_b] = _cmp_abs_common(a, b);
		return abs_a >= abs_b;
	}

	inline bool abs_lt(const Complex<FloatF64eI64> &a, const Complex<FloatF64eI64> &b) {
		return norm(a) < norm(b);
	}

	inline bool abs_le(const Complex<FloatF64eI64> &a, const Complex<FloatF64eI64> &b) {
		return norm(a) <= norm(b);
	}

	inline bool abs_gt(const Complex<FloatF64eI64> &a, const Complex<FloatF64eI64> &b) {
		return norm(a) > norm(b);
	}

	inline bool abs_ge(const Complex<FloatF64eI64> &a, const Complex<FloatF64eI64> &b) {
		return norm(a) >= norm(b);
	}

	inline double fast_round(double x);
	inline int32_t round_i32(double x);
	inline int64_t round_i64(double x);
}

#if (defined(__SSE4_1__) || defined(__AVX__)) && __has_include(<smmintrin.h>)
#include <smmintrin.h>

inline double Imagina::fast_round(double x) {
	__m128d temp = _mm_set_sd(x);
	return _mm_cvtsd_f64(_mm_round_sd(temp, temp, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
}

inline int32_t Imagina::round_i32(double x) { return Imagina::fast_round(x); }
inline int64_t Imagina::round_i64(double x) { return Imagina::fast_round(x); }
#elif defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
inline double Imagina::fast_round(double x) {
	return std::nearbyint(x); // FIXME: rounding mode
}
inline int32_t Imagina::round_i32(double x) {
	return x + std::copysign(0.5, x);
}
inline int64_t Imagina::round_i64(double x) {
	return x + std::copysign(0.5, x);
}
#else // ARM
inline double Imagina::fast_round(double x) {
	return std::round(x);
}
inline int32_t Imagina::round_i32(double x) {
	return std::round(x);
}
inline int64_t Imagina::round_i64(double x) {
	return std::round(x);
}
#endif
