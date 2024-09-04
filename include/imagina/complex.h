#pragma once
#include <utility>
#include "constants.h"

namespace Imagina::inline Numerics {
	template<typename real>
	struct Complex {
		real re, im;

		Complex() = default;
		//Complex(Complex &&) = default;
		//Complex(const Complex &) = default;
		//Complex(real re) noexcept : re(re), im(Constants::Zero<real>()) {}

		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
		constexpr Complex(real2 re) noexcept : re(re), im(0.0) {}

		constexpr Complex(real re, real im) noexcept : re(re), im(im) {}
		//template<typename real2>
		//Complex(real2 re, real2 im) noexcept : re(re), im(im) {}

		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
		constexpr Complex(const Complex<real2> &complex) : re(complex.re), im(complex.im) {}
		
		template<typename real2>
		explicit(!std::is_convertible_v<real2, real>)
		constexpr Complex(Complex<real2> &&complex) : re(std::move(complex.re)), im(std::move(complex.im)) {}

		//Complex &operator=(Complex &&) = default;
		//Complex &operator=(const Complex &) = default;

		constexpr Complex &operator+=(const real &a) { re += a; return *this; }
		constexpr Complex &operator-=(const real &a) { re -= a; return *this; }
		constexpr Complex &operator*=(const real &a) { re *= a; im *= a; return *this; }
		constexpr Complex &operator/=(const real &a) { re /= a; im /= a; return *this; }

		//Complex &operator+=(const Complex &a) { re += a.re; im += a.im; return *this; }
		//Complex &operator-=(const Complex &a) { re -= a.re; im -= a.im; return *this; }
		constexpr Complex &operator*=(const Complex &a) { *this = *this * a; return *this; }
		constexpr Complex &operator/=(const Complex &a) { *this = *this / a; return *this; }

		template<typename real2>
		constexpr Complex &operator+=(const Complex<real2> &a) requires requires (real a, const real2 &b) { a += b; } {
			re += a.re;
			im += a.im;
			return *this;
		}
		template<typename real2>
		constexpr Complex &operator-=(const Complex<real2> &a) requires requires (real a, const real2 &b) { a -= b; } {
			re -= a.re;
			im -= a.im;
			return *this;
		}

		constexpr bool operator==(const Complex &x) { return re == x.re && im == x.im; }
		constexpr bool operator!=(const Complex &x) { return re != x.re || im != x.im; }
	};

	template<typename real>
	constexpr real norm(const Complex<real> &a) {
		return a.re * a.re + a.im * a.im;
	}

	template<typename real>
	constexpr Complex<real> conj(const Complex<real> &a) {
		return Complex<real>(a.re, -a.im);
	}

	template<typename real> constexpr Complex<real> operator+(Complex<real> a, const real &b) { return a += b; }
	template<typename real> constexpr Complex<real> operator-(Complex<real> a, const real &b) { return a -= b; }
	template<typename real> constexpr Complex<real> operator*(Complex<real> a, const real &b) { return a *= b; }
	template<typename real> constexpr Complex<real> operator/(Complex<real> a, const real &b) { return a /= b; }

	template<typename real> constexpr Complex<real> operator+(const real &a, Complex<real> b) { return b += a; }
	template<typename real> constexpr Complex<real> operator*(const real &a, Complex<real> b) { return b *= a; }
	template<typename real> constexpr Complex<real> operator-(const real &a, Complex<real> b) { return Complex<real>(a) -= b; }
	template<typename real> constexpr Complex<real> operator/(const real &a, Complex<real> b) { return Complex<real>(a) /= b; }

	template<typename real> constexpr Complex<real> operator+(Complex<real> a, const Complex<real> &b) { return a += b; }
	template<typename real> constexpr Complex<real> operator-(Complex<real> a, const Complex<real> &b) { return a -= b; }

	template<typename real>
	constexpr Complex<real> operator*(const Complex<real> &a, const Complex<real> &b) {
		return Complex<real>(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
	}

	template<typename real>
	constexpr Complex<real> operator/(const Complex<real> &a, const Complex<real> &b) {
		return a * conj(b) / norm(b); // FIXME: overflow
	}
}