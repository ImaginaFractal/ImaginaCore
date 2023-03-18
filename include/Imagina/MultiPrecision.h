#pragma once

#include <stdint.h>
#include "Declarations.h"

namespace Imagina {
	struct im_export FixedGeneric32 {
		static size_t DefaultSize;
		static constexpr uint32_t SignMask = 0x80000000;

		uint32_t *Value;
		size_t Size = DefaultSize;

		FixedGeneric32() : Value(new uint32_t[DefaultSize]) {}
		~FixedGeneric32() { delete[] Value; }

		FixedGeneric32(double x);
		FixedGeneric32(const FixedGeneric32 &x);
		FixedGeneric32(FixedGeneric32 &&x);

		FixedGeneric32 &operator=(const FixedGeneric32 &x);
		FixedGeneric32 &operator=(FixedGeneric32 &&x);

		bool IsNegative() const { return Value[Size - 1] & (1ull << 31) ? true : false; }

		operator double() const;

		FixedGeneric32 &operator+=(const FixedGeneric32 &x);
		FixedGeneric32 &operator-=(const FixedGeneric32 &x);
		FixedGeneric32 &operator*=(const FixedGeneric32 &x);
	};

	inline FixedGeneric32 operator+(FixedGeneric32 x, const FixedGeneric32 &y) { return x += y; }
	inline FixedGeneric32 operator-(FixedGeneric32 x, const FixedGeneric32 &y) { return x -= y; }
	inline FixedGeneric32 operator*(FixedGeneric32 x, const FixedGeneric32 &y) { return x *= y; }

	/*class MPReal;
	class MultiPrecision {
	public:
		virtual MPReal *NewReal() = 0;

		virtual void Add(MPReal *result, const MPReal *x, const MPReal y) = 0;
		virtual void Sub(MPReal *result, const MPReal *x, const MPReal y) = 0;
		virtual void Mul(MPReal *result, const MPReal *x, const MPReal y) = 0;
		virtual void Div(MPReal *result, const MPReal *x, const MPReal y) = 0;

		virtual void Sqr(MPReal *result, const MPReal *x) = 0;
	};

	class MPReal : public MultiPrecision {
	public:
		virtual ~MPReal() = 0;

		virtual MPReal &operator=(const MPReal &) = 0;
		virtual MPReal &operator=(MPReal &&) = 0;

		virtual explicit operator double() const = 0;

		virtual bool Sign() const = 0; // Returns true if negative

		virtual MPReal &operator+=(const MPReal &x) = 0;
		virtual MPReal &operator-=(const MPReal &x) = 0;
		virtual MPReal &operator*=(const MPReal &x) = 0;
		virtual MPReal &operator/=(const MPReal &x) = 0;

		//virtual MPReal &operator<<=(const ??? x)
	};*/

	struct MPReal;
	struct _MultiPrecision;

	struct _MultiPrecision {
		operator _MultiPrecision *() { return this; }
		operator const _MultiPrecision *() const { return this; }

		void Init(MPReal *x) const;
		void InitWithPrecision(MPReal *x, size_t precision) const;

		void (*InitContent)(MPReal *); // Do not use directly, use Init instead.
		void (*InitContentWithPrecision)(MPReal *, size_t); // Do not use directly, use InitWithPrecision instead.

		void Clear(MPReal *x) const;
		void (*ClearContent)(MPReal *); // Do not use directly, use Clear instead.

		void (*SetPrecision)(MPReal *, size_t);

		void (*Set)(MPReal *, const MPReal *);
		void (*SetDouble)(MPReal *, double);
		double (*ToDouble)(const MPReal *);

		void (*Add)(MPReal *, const MPReal *, const MPReal *);
		void (*Sub)(MPReal *, const MPReal *, const MPReal *);
		void (*Mul)(MPReal *, const MPReal *, const MPReal *);
		void (*Div)(MPReal *, const MPReal *, const MPReal *);
	};

	using MultiPrecision = const _MultiPrecision;

	struct MPReal {
		const void *Content[7]; // This is for saving the contents of the actual number type (like mpf_t or mpfr_t)
		MultiPrecision *const MP = nullptr;

		MPReal() = default;
		MPReal(MultiPrecision *mp) : MP(mp) { mp->InitContent(this); }
		MPReal(MultiPrecision *mp, double x) : MP(mp) { mp->InitContent(this); mp->SetDouble(this, x); }
		MPReal(const MPReal &x) : MPReal(x.MP) { MP->Set(this, x); }
		~MPReal() { if (MP) MP->ClearContent(this); }

		operator MPReal *() { return this; }
		operator const MPReal *() const { return this; }

		MPReal &operator=(const MPReal &x) { MP->Set(this, x); return *this; }
		MPReal &operator=(double x) { MP->SetDouble(this, x); return *this; }

		explicit operator double() const { return MP->ToDouble(this); }

		MPReal &operator+=(const MPReal &x) { MP->Add(this, this, x); return *this; }
		MPReal &operator-=(const MPReal &x) { MP->Sub(this, this, x); return *this; }
		MPReal &operator*=(const MPReal &x) { MP->Mul(this, this, x); return *this; }
		MPReal &operator/=(const MPReal &x) { MP->Div(this, this, x); return *this; }

		MPReal &operator+=(double x) { MP->Add(this, this, MPReal(MP, x)); return *this; }
		MPReal &operator-=(double x) { MP->Sub(this, this, MPReal(MP, x)); return *this; }
	};

	inline void Imagina::_MultiPrecision::Init(MPReal *x) const {
		InitContent(x);
		const_cast<MultiPrecision *&>(x->MP) = this;
	}
	inline void _MultiPrecision::InitWithPrecision(MPReal *x, size_t precision) const {
		InitContentWithPrecision(x, precision);
		const_cast<MultiPrecision *&>(x->MP) = this;
	}

	inline void _MultiPrecision::Clear(MPReal *x) const {
		ClearContent(x);
		const_cast<MultiPrecision *&>(x->MP) = nullptr;
	}

	inline MPReal operator+(MPReal x, const MPReal &y) { return x += y; }
	inline MPReal operator-(MPReal x, const MPReal &y) { return x -= y; }
	inline MPReal operator*(MPReal x, const MPReal &y) { return x *= y; }
	inline MPReal operator/(MPReal x, const MPReal &y) { return x /= y; }
}