#pragma once

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <cmath>

namespace Imagina::inline Numerics {
	template<typename T>
	struct Vector3 {
		union {
			struct { T x, y, z; };
			struct { T r, g, b; };
			T components[3];
		};

		Vector3() = default;
		Vector3(T x) : x(x), y(x), z(x) {}
		Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

		T &operator[](ptrdiff_t i) { return components[i]; }
		const T &operator[](ptrdiff_t i) const { return components[i]; }

		Vector3 &operator+=(Vector3 a) { x += a.x; y += a.y; z += a.z; return *this; }
		Vector3 &operator-=(Vector3 a) { x -= a.x; y -= a.y; z -= a.z; return *this; }
		Vector3 &operator*=(Vector3 a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
		Vector3 &operator/=(Vector3 a) { x /= a.x; y /= a.y; z /= a.z; return *this; }

		Vector3 &operator*=(T a) { x *= a; y *= a; z *= a; return *this; }
		Vector3 &operator/=(T a) { x /= a; y /= a; z /= a; return *this; }
	};

	template<typename T> Vector3<T> operator+(Vector3<T> a, const Vector3<T> &b) { return a += b; }
	template<typename T> Vector3<T> operator-(Vector3<T> a, const Vector3<T> &b) { return a -= b; }
	template<typename T> Vector3<T> operator*(Vector3<T> a, const Vector3<T> &b) { return a *= b; }
	template<typename T> Vector3<T> operator/(Vector3<T> a, const Vector3<T> &b) { return a /= b; }

	template<typename T> Vector3<T> operator*(Vector3<T> a, T b) { return a *= b; }
	template<typename T> Vector3<T> operator/(Vector3<T> a, T b) { return a /= b; }

	template<typename T> Vector3<T> operator*(T a, Vector3<T> b) { return b *= a; }

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

	template<std::floating_point T>
	T clamp01(T x) { return (x < T(0.0)) ? T(0.0) : (x > T(1.0)) ? T(1.0) : x; }

	struct rgb8 {
		uint8_t r, g, b;

		rgb8() = default;
		rgb8(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

		template<std::floating_point T>
		rgb8(T r, T g, T b) :
			r((uint32_t)std::round(clamp01(r) * T(255.0))),
			g((uint32_t)std::round(clamp01(g) * T(255.0))),
			b((uint32_t)std::round(clamp01(b) * T(255.0))) {}

		template<std::floating_point T> rgb8(Vector3<T> x) : rgb8(x.r, x.g, x.b) {}
		template<std::floating_point T> rgb8(Vector4<T> x) : rgb8(x.r, x.g, x.b) {}

		template<std::floating_point T> operator Vector3<T>() { return Vector3<T>(r / T(255.0), g / T(255.0), b / T(255.0)); }
		template<std::floating_point T> operator Vector4<T>() { return Vector4<T>(r / T(255.0), g / T(255.0), b / T(255.0), 1.0); }

		explicit operator uint32_t() {
			return uint32_t(r)
				| (uint32_t(g) << 8)
				| (uint32_t(b) << 16)
				| (0xFFU << 24);
		}
	};

	struct rgba8 {
		uint8_t r, g, b, a;

		rgba8() = default;
		rgba8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) : r(r), g(g), b(b), a(a) {}

		template<std::floating_point T>
		rgba8(T r, T g, T b, T a = 1.0) :
			r((uint32_t)std::round(clamp01(r) * T(255.0))),
			g((uint32_t)std::round(clamp01(g) * T(255.0))),
			b((uint32_t)std::round(clamp01(b) * T(255.0))),
			a((uint32_t)std::round(clamp01(a) * T(255.0))) {}

		rgba8(rgb8 x, uint8_t a = 0xFF) : r(x.r), g(x.g), b(x.b), a(a) {}

		template<std::floating_point T> rgba8(Vector3<T> x) : rgba8(x.r, x.g, x.b, 0xFF) {}
		template<std::floating_point T> rgba8(Vector4<T> x) : rgba8(x.r, x.g, x.b, x.a) {}

		template<std::floating_point T> operator Vector3<T>() { return Vector3<T>(r / T(255.0), g / T(255.0), b / T(255.0)); }
		template<std::floating_point T> operator Vector4<T>() { return Vector4<T>(r / T(255.0), g / T(255.0), b / T(255.0), a / T(255.0)); }

		explicit operator uint32_t() {
			return uint32_t(r)
				| (uint32_t(g) << 8)
				| (uint32_t(b) << 16)
				| (uint32_t(a) << 24);
		}
	};
}