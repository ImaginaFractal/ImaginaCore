#pragma once
#include <utility>
#include "constants.h"

namespace Imagina::inline Numerics {
	template<typename Real>
	struct Complex {
		Real re, im;

		Complex() = default;
		//Complex(Complex &&) = default;
		//Complex(const Complex &) = default;
		//Complex(Real re) noexcept : re(re), im(Constants::Zero<Real>()) {}

		template<typename Real2>
		explicit(!std::is_convertible_v<Real2, Real>)
		constexpr Complex(Real2 re) noexcept : re(re), im(Constants::Zero) {}

		constexpr Complex(Real re, Real im) noexcept : re(re), im(im) {}
		//template<typename Real2>
		//Complex(Real2 re, Real2 im) noexcept : re(re), im(im) {}

		template<typename Real2>
		explicit(!std::is_convertible_v<Real2, Real>)
		constexpr Complex(const Complex<Real2> &complex) : re(complex.re), im(complex.im) {}
		
		template<typename Real2>
		explicit(!std::is_convertible_v<Real2, Real>)
		constexpr Complex(Complex<Real2> &&complex) : re(std::move(complex.re)), im(std::move(complex.im)) {}

		//Complex &operator=(Complex &&) = default;
		//Complex &operator=(const Complex &) = default;

		constexpr Complex &operator+=(const Real &a) { re += a; return *this; }
		constexpr Complex &operator-=(const Real &a) { re -= a; return *this; }
		constexpr Complex &operator*=(const Real &a) { re *= a; im *= a; return *this; }
		constexpr Complex &operator/=(const Real &a) { re /= a; im /= a; return *this; }

		//Complex &operator+=(const Complex &a) { re += a.re; im += a.im; return *this; }
		//Complex &operator-=(const Complex &a) { re -= a.re; im -= a.im; return *this; }
		constexpr Complex &operator*=(const Complex &a) { *this = *this * a; return *this; }
		constexpr Complex &operator/=(const Complex &a) { *this = *this / a; return *this; }

		template<typename Real2>
		constexpr Complex &operator+=(const Complex<Real2> &a) requires requires (Real a, const Real2 &b) { a += b; } {
			re += a.re;
			im += a.im;
			return *this;
		}
		template<typename Real2>
		constexpr Complex &operator-=(const Complex<Real2> &a) requires requires (Real a, const Real2 &b) { a -= b; } {
			re -= a.re;
			im -= a.im;
			return *this;
		}

		constexpr bool operator==(const Complex &x) { return re == x.re && im == x.im; }
		constexpr bool operator!=(const Complex &x) { return re != x.re || im != x.im; }
	};

	template<typename Real>
	constexpr Real norm(const Complex<Real> &a) {
		return a.re * a.re + a.im * a.im;
	}

	template<typename Real>
	constexpr Complex<Real> conj(const Complex<Real> &a) {
		return Complex<Real>(a.re, -a.im);
	}

	template<typename Real> constexpr Complex<Real> operator+(Complex<Real> a, const Real &b) { return a += b; }
	template<typename Real> constexpr Complex<Real> operator-(Complex<Real> a, const Real &b) { return a -= b; }
	template<typename Real> constexpr Complex<Real> operator*(Complex<Real> a, const Real &b) { return a *= b; }
	template<typename Real> constexpr Complex<Real> operator/(Complex<Real> a, const Real &b) { return a /= b; }

	template<typename Real> constexpr Complex<Real> operator+(const Real &a, Complex<Real> b) { return b += a; }
	template<typename Real> constexpr Complex<Real> operator*(const Real &a, Complex<Real> b) { return b *= a; }
	template<typename Real> constexpr Complex<Real> operator-(const Real &a, Complex<Real> b) { return Complex<Real>(a) -= b; }
	template<typename Real> constexpr Complex<Real> operator/(const Real &a, Complex<Real> b) { return Complex<Real>(a) /= b; }

	template<typename Real> constexpr Complex<Real> operator+(Complex<Real> a, const Complex<Real> &b) { return a += b; }
	template<typename Real> constexpr Complex<Real> operator-(Complex<Real> a, const Complex<Real> &b) { return a -= b; }

	template<typename Real>
	constexpr Complex<Real> operator*(const Complex<Real> &a, const Complex<Real> &b) {
		return Complex<Real>(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
	}

	template<typename Real>
	constexpr Complex<Real> operator/(const Complex<Real> &a, const Complex<Real> &b) {
		return a * conj(b) / norm(b); // FIXME: overflow
	}
}