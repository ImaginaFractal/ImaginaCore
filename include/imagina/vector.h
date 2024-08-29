#pragma once

#include <cstddef>

namespace Imagina::inline Numerics {
	template<typename T>
	struct Vector4 {
		union {
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			T components[4];
		};

		Vector4() = default;
		Vector4(T x) : x(x), y(x), z(x), w(x) {}
		Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

		T &operator[](ptrdiff_t i) { return components[i]; }
		const T &operator[](ptrdiff_t i) const { return components[i]; }

		Vector4 &operator+=(Vector4 a) { x += a.x; y += a.y; z += a.z; w += a.w; return *this; }
		Vector4 &operator-=(Vector4 a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; return *this; }
		Vector4 &operator*=(Vector4 a) { x *= a.x; y *= a.y; z *= a.z; w *= a.w; return *this; }
		Vector4 &operator/=(Vector4 a) { x /= a.x; y /= a.y; z /= a.z; w /= a.w; return *this; }

		Vector4 &operator*=(T a) { x *= a; y *= a; z *= a; w *= a; return *this; }
		Vector4 &operator/=(T a) { x /= a; y /= a; z /= a; w /= a; return *this; }
	};

	template<typename T> Vector4<T> operator+(Vector4<T> a, const Vector4<T> &b) { return a += b; }
	template<typename T> Vector4<T> operator-(Vector4<T> a, const Vector4<T> &b) { return a -= b; }
	template<typename T> Vector4<T> operator*(Vector4<T> a, const Vector4<T> &b) { return a *= b; }
	template<typename T> Vector4<T> operator/(Vector4<T> a, const Vector4<T> &b) { return a /= b; }

	template<typename T> Vector4<T> operator*(Vector4<T> a, T b) { return a *= b; }
	template<typename T> Vector4<T> operator/(Vector4<T> a, T b) { return a /= b; }

	template<typename T> Vector4<T> operator*(T a, Vector4<T> b) { return b *= a; }
}