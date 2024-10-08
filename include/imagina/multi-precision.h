#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include "declarations.h"
#include "float_f64ei64.h"

namespace Imagina::inline Numerics {
	struct MPReal;

	typedef void		(*pMultiPrecision_InitContent)		(MPReal *, mp_uint_bc); // Do not use directly, use Init instead.
	typedef void		(*pMultiPrecision_InitContentCopy)	(MPReal *, const MPReal *); // Do not use directly, use InitCopy instead.

	typedef void		(*pMultiPrecision_ClearContent)		(MPReal *); // Do not use directly, use Clear instead.

	typedef mp_uint_bc	(*pMultiPrecision_GetPrecision)		(const MPReal *);
	typedef void		(*pMultiPrecision_SetPrecision)		(MPReal *, mp_uint_bc);

	typedef void		(*pMultiPrecision_Set)				(MPReal *, const MPReal *);
	typedef void		(*pMultiPrecision_Copy)				(MPReal *, const MPReal *); // Set value and precision
	typedef void		(*pMultiPrecision_SetDouble)		(MPReal *, double);
	typedef void		(*pMultiPrecision_SetFloatF64eI64)	(MPReal *, float_f64ei64);
	typedef bool		(*pMultiPrecision_SetString)		(MPReal *, const char *, int);

	typedef double		(*pMultiPrecision_GetDouble)		(const MPReal *);
	typedef float_f64ei64(*pMultiPrecision_GetFloatF64eI64)	(const MPReal *);

	typedef void		(*pMultiPrecision_Add)				(MPReal *, const MPReal *, const MPReal *);
	typedef void		(*pMultiPrecision_Sub)				(MPReal *, const MPReal *, const MPReal *);
	typedef void		(*pMultiPrecision_Mul)				(MPReal *, const MPReal *, const MPReal *);
	typedef void		(*pMultiPrecision_Div)				(MPReal *, const MPReal *, const MPReal *);
	
	typedef size_t		(*pMultiPrecision_CalcSize)			(mp_uint_bc);
	typedef void		(*pMultiPrecision_PlacementInit)	(MPReal *, mp_uint_bc, void *); // Construct MPReal using preallocated memory.

	struct _MultiPrecisionMutable { // TODO: Reorder
		const char *Name;

		pMultiPrecision_InitContent		InitContent;
		pMultiPrecision_InitContentCopy	InitContentCopy;

		pMultiPrecision_ClearContent	ClearContent;

		pMultiPrecision_GetPrecision	GetPrecision;
		pMultiPrecision_SetPrecision	SetPrecision;

		pMultiPrecision_Set				Set;
		pMultiPrecision_Copy			Copy;
		pMultiPrecision_SetDouble		SetDouble;
		pMultiPrecision_SetFloatF64eI64	SetFloatF64eI64;
		pMultiPrecision_SetString		SetString;

		pMultiPrecision_GetDouble		GetDouble;
		pMultiPrecision_GetFloatF64eI64	GetFloatF64eI64;

		pMultiPrecision_Add				Add;
		pMultiPrecision_Sub				Sub;
		pMultiPrecision_Mul				Mul;
		pMultiPrecision_Div				Div;

		pMultiPrecision_CalcSize		CalcSize;
		pMultiPrecision_PlacementInit	PlacementInit;
	};

	struct MultiPrecisionMutable : _MultiPrecisionMutable {
		operator MultiPrecisionMutable *() { return this; }
		operator const MultiPrecisionMutable *() const { return this; }

		MultiPrecisionMutable(_MultiPrecisionMutable &&mp) : _MultiPrecisionMutable(mp) {}
		MultiPrecisionMutable(const MultiPrecisionMutable &) = delete;
		MultiPrecisionMutable(MultiPrecisionMutable &&) = delete;

		void Init(MPReal *x, mp_uint_bc precision) const;
		void InitCopy(MPReal *dst, const MPReal *src) const;

		void Clear(MPReal *x) const;
	};

	//using MultiPrecision = const MultiPrecisionMutable;

	struct MPReal {
		const void *PlaceHolder[7]; // This is for saving the contents of the actual number type (like mpf_t or mpfr_t)
		MultiPrecision *const MP = nullptr;

		MPReal() = default;
		MPReal(MultiPrecision *mp, mp_uint_bc precision)					: MP(mp) { mp->InitContent(this, precision); }
		MPReal(double x, MultiPrecision *mp)							: MP(mp) { mp->InitContent(this, 53);			mp->SetDouble(this, x); }
		MPReal(double x, MultiPrecision *mp, mp_uint_bc precision)		: MP(mp) { mp->InitContent(this, precision);	mp->SetDouble(this, x); }
		MPReal(float_f64ei64 x, MultiPrecision *mp)						: MP(mp) { mp->InitContent(this, 53);			mp->SetFloatF64eI64(this, x); }
		MPReal(float_f64ei64 x, MultiPrecision *mp, mp_uint_bc precision)	: MP(mp) { mp->InitContent(this, precision);	mp->SetFloatF64eI64(this, x); }
		//MPReal(const MPReal &x) : MP(x.MP) { MP->InitContent(this, x.GetPrecision()); MP->Set(this, x); }
		MPReal(const MPReal &x)											: MP(x.MP) { MP->InitContentCopy(this, x); }
		~MPReal() { if (MP) MP->ClearContent(this); }

		bool Valid() { return MP != nullptr; }

		operator MPReal *() { return this; }
		operator const MPReal *() const { return this; }

		MPReal &operator=(const MPReal &x) { assert(MP == x.MP); MP->Set(this, x); return *this; } // Assign
		MPReal &operator&=(const MPReal &x); // Copy (set precision and assign)
		MPReal &operator|=(const MPReal &x); // Set MP and copy
		MPReal &operator=(double x) { MP->SetDouble(this, x); return *this; }
		MPReal &operator=(float_f64ei64 x) { MP->SetFloatF64eI64(this, x); return *this; }

		explicit operator double() const { return MP->GetDouble(this); }
		explicit operator float_f64ei64() const { return MP->GetFloatF64eI64(this); };

		mp_uint_bc GetPrecision() const { return MP->GetPrecision(this); }
		void SetPrecision(mp_uint_bc precision) { MP->SetPrecision(this, precision); }

		MPReal &operator+=(const MPReal &x) { assert(MP == x.MP); MP->Add(this, this, x); return *this; }
		MPReal &operator-=(const MPReal &x) { assert(MP == x.MP); MP->Sub(this, this, x); return *this; }
		MPReal &operator*=(const MPReal &x) { assert(MP == x.MP); MP->Mul(this, this, x); return *this; }
		MPReal &operator/=(const MPReal &x) { assert(MP == x.MP); MP->Div(this, this, x); return *this; }

		MPReal &operator+=(double x) { MP->Add(this, this, MPReal(x, MP)); return *this; }
		MPReal &operator-=(double x) { MP->Sub(this, this, MPReal(x, MP)); return *this; }

		MPReal &operator+=(float_f64ei64 x) { MP->Add(this, this, MPReal(x, MP)); return *this; }
		MPReal &operator-=(float_f64ei64 x) { MP->Sub(this, this, MPReal(x, MP)); return *this; }

		void CopyFrom(const MPReal &x) {
			if_likely (MP == x.MP) {
				MP->Copy(this, x);
			} else {
				if (MP) MP->ClearContent(this);

				x.MP->InitCopy(this, x);
			}
		}
	};

	inline void MultiPrecisionMutable::Init(MPReal *x, mp_uint_bc precision) const {
		InitContent(x, precision);
		const_cast<MultiPrecision *&>(x->MP) = this;
	}

	inline void MultiPrecisionMutable::InitCopy(MPReal *dst, const MPReal *src) const {
		InitContentCopy(dst, src);
		const_cast<MultiPrecision *&>(dst->MP) = this;
	}

	inline void MultiPrecisionMutable::Clear(MPReal *x) const {
		ClearContent(x);
		const_cast<MultiPrecision *&>(x->MP) = nullptr;
	}

	// Copy operator. MPReal does not support bitwise and operation
	inline MPReal &MPReal::operator&=(const MPReal &x) {
		MP->Copy(this, x);
		return *this;
	}

	// Copy operator. (Re)init if needed. MPReal does not support bitwise or operation
	inline MPReal &MPReal::operator|=(const MPReal &x) {
		if (MP != x.MP) [[unlikely]] {
			if (MP) MP->ClearContent(this);

			x.MP->InitCopy(this, x);
		} else {
			MP->Copy(this, x);
		}
		return *this;
	}

	// TODO: Eliminate temporary variable
	inline MPReal operator+(MPReal x, const MPReal &y) { return x += y; }
	inline MPReal operator-(MPReal x, const MPReal &y) { return x -= y; }
	inline MPReal operator*(MPReal x, const MPReal &y) { return x *= y; }
	inline MPReal operator/(MPReal x, const MPReal &y) { return x /= y; }

	extern im_export MultiPrecision MPDouble;
}