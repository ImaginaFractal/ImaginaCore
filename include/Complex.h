#pragma once
#include "Constants.h"

namespace Imagina {
	template<typename Real>
	struct Complex {
		Real re, im;

		Complex() = default;
		Complex(Complex &&) = default;
		Complex(const Complex &) = default;
		Complex(Real re) noexcept : re(re), im(Constants::Zero<Real>()) {}
		Complex(Real re, Real im) noexcept : re(re), im(im) {}

		Complex &operator=(Complex &&) = default;
		Complex &operator=(const Complex &) = default;

		Complex &operator+=(const Real &a) { re += a; return *this; }
		Complex &operator-=(const Real &a) { re -= a; return *this; }
		Complex &operator*=(const Real &a) { re *= a; im *= a; return *this; }
		Complex &operator/=(const Real &a) { re /= a; im /= a; return *this; }

		Complex &operator+=(const Complex &a) { re += a.re; im += a.im; return *this; }
		Complex &operator-=(const Complex &a) { re -= a.re; im -= a.im; return *this; }
		Complex &operator*=(const Complex &a) { *this = *this * a; return *this; }
		Complex &operator/=(const Complex &a) { *this = *this / a; return *this; }
	};

	template<typename Real>
	Real norm(const Complex<Real> &a) {
		return a.re * a.re + a.im * a.im;
	}

	template<typename Real>
	Complex<Real> conj(const Complex<Real> &a) {
		return Complex<Real>(a.re, -a.im);
	}

	template<typename Real>
	Complex<Real> operator+(Complex<Real> a, const Complex<Real> &b) {
		return a += b;
	}

	template<typename Real>
	Complex<Real> operator-(Complex<Real> a, const Complex<Real> &b) {
		return a -= b;
	}

	template<typename Real>
	Complex<Real> operator*(const Complex<Real> &a, const Complex<Real> &b) {
		return Complex<Real>(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
	}

	template<typename Real>
	Complex<Real> operator/(const Complex<Real> &a, const Complex<Real> &b) {
		return a * conj(b) / norm(b);
	}
}