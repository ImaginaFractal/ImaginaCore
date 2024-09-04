#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include "declarations.h"
#include "float_f64ei64.h"

namespace Imagina::inline Numerics {
	struct imp_real;

	typedef void		(*p_multi_precision_init)				(imp_real *, mp_uint_bc);
	typedef void		(*p_multi_precision_init_copy)			(imp_real *, const imp_real *);

	typedef void		(*p_multi_precision_clear)				(imp_real *);

	typedef mp_uint_bc	(*p_multi_precision_get_precision)		(const imp_real *);
	typedef void		(*p_multi_precision_set_precision)		(imp_real *, mp_uint_bc);

	typedef void		(*p_multi_precision_set)				(imp_real *, const imp_real *);
	typedef void		(*p_multi_precision_copy)				(imp_real *, const imp_real *); // Set value and precision
	typedef void		(*p_multi_precision_set_double)			(imp_real *, double);
	typedef void		(*p_multi_precision_set_double_2exp)	(imp_real *, double, int_exp);
	typedef void		(*p_multi_precision_set_float_f64ei64)	(imp_real *, float_f64ei64);
	typedef bool		(*p_multi_precision_set_string)			(imp_real *, const char *, int);

	typedef double		(*p_multi_precision_get_double)			(const imp_real *);
	typedef double		(*p_multi_precision_get_double_2exp)	(int_exp *, const imp_real *);
	typedef float_f64ei64(*p_multi_precision_get_float_f64ei64)	(const imp_real *);

	typedef void		(*p_multi_precision_add)				(imp_real *, const imp_real *, const imp_real *);
	typedef void		(*p_multi_precision_sub)				(imp_real *, const imp_real *, const imp_real *);
	typedef void		(*p_multi_precision_mul)				(imp_real *, const imp_real *, const imp_real *);
	typedef void		(*p_multi_precision_div)				(imp_real *, const imp_real *, const imp_real *);
	
	typedef size_t		(*p_multi_precision_calc_size)			(mp_uint_bc);
	typedef void		(*p_multi_precision_placement_init)		(imp_real *, mp_uint_bc, void *); // Construct imp_real using preallocated memory.

	struct _multi_precision_mutable { // TODO: Reorder
		const char *Name;

		p_multi_precision_init				_init; // Do not use directly, use init instead.
		p_multi_precision_init_copy			_init_copy; // Do not use directly, use init_copy instead.

		p_multi_precision_clear				_clear; // Do not use directly, use clear instead.

		p_multi_precision_get_precision		get_precision;
		p_multi_precision_set_precision		set_precision;

		p_multi_precision_set				set;
		p_multi_precision_copy				copy;
		p_multi_precision_set_double		set_double;
		p_multi_precision_set_double_2exp	set_double_2exp;
		p_multi_precision_set_float_f64ei64	set_float_f64ei64;
		p_multi_precision_set_string		set_string;

		p_multi_precision_get_double		get_double;
		p_multi_precision_get_double_2exp	get_double_2exp;
		p_multi_precision_get_float_f64ei64	get_float_f64ei64;

		p_multi_precision_add				add;
		p_multi_precision_sub				sub;
		p_multi_precision_mul				mul;
		p_multi_precision_div				div;

		p_multi_precision_calc_size			calc_size;
		p_multi_precision_placement_init	placement_init;
	};

	struct multi_precision_mutable : _multi_precision_mutable {
		operator multi_precision_mutable *() { return this; }
		operator const multi_precision_mutable *() const { return this; }

		multi_precision_mutable(_multi_precision_mutable &&mp) : _multi_precision_mutable(mp) {}
		multi_precision_mutable(const multi_precision_mutable &) = delete;
		multi_precision_mutable(multi_precision_mutable &&) = delete;

		void Init(imp_real *x, mp_uint_bc precision) const;
		void InitCopy(imp_real *dst, const imp_real *src) const;

		void Clear(imp_real *x) const;
	};

	//using MultiPrecision = const MultiPrecisionMutable;

	struct imp_real {
		const void *PlaceHolder[7]; // This is for saving the contents of the actual number type (like mpf_t or mpfr_t)
		multi_precision *const MP = nullptr;

		imp_real() = default;
		imp_real(multi_precision *mp, mp_uint_bc precision)						: MP(mp) { mp->_init(this, precision); }
		imp_real(double x, multi_precision *mp)									: MP(mp) { mp->_init(this, 53);			mp->set_double(this, x); }
		imp_real(double x, multi_precision *mp, mp_uint_bc precision)			: MP(mp) { mp->_init(this, precision);	mp->set_double(this, x); }
		imp_real(float_f64ei64 x, multi_precision *mp)							: MP(mp) { mp->_init(this, 53);			mp->set_float_f64ei64(this, x); }
		imp_real(float_f64ei64 x, multi_precision *mp, mp_uint_bc precision)	: MP(mp) { mp->_init(this, precision);	mp->set_float_f64ei64(this, x); }
		//imp_real(const imp_real &x)											: MP(x.MP) { MP->_init(this, x.get_precision()); MP->Set(this, x); }
		imp_real(const imp_real &x)												: MP(x.MP) { MP->_init_copy(this, x); }
		~imp_real() { if (MP) MP->_clear(this); }

		bool Valid() { return MP != nullptr; }

		operator imp_real *() { return this; }
		operator const imp_real *() const { return this; }

		imp_real &operator=(const imp_real &x) { assert(MP == x.MP); MP->set(this, x); return *this; } // Assign
		imp_real &operator&=(const imp_real &x); // Copy (set precision and assign)
		imp_real &operator|=(const imp_real &x); // Set MP and copy
		imp_real &operator=(double x) { MP->set_double(this, x); return *this; }
		imp_real &operator=(float_f64ei64 x) { MP->set_float_f64ei64(this, x); return *this; }

		explicit operator double() const { return MP->get_double(this); }
		explicit operator float_f64ei64() const { return MP->get_float_f64ei64(this); };

		mp_uint_bc get_precision() const { return MP->get_precision(this); }
		void set_precision(mp_uint_bc precision) { MP->set_precision(this, precision); }

		imp_real &operator+=(const imp_real &x) { assert(MP == x.MP); MP->add(this, this, x); return *this; }
		imp_real &operator-=(const imp_real &x) { assert(MP == x.MP); MP->sub(this, this, x); return *this; }
		imp_real &operator*=(const imp_real &x) { assert(MP == x.MP); MP->mul(this, this, x); return *this; }
		imp_real &operator/=(const imp_real &x) { assert(MP == x.MP); MP->div(this, this, x); return *this; }

		imp_real &operator+=(double x) { MP->add(this, this, imp_real(x, MP)); return *this; }
		imp_real &operator-=(double x) { MP->sub(this, this, imp_real(x, MP)); return *this; }

		imp_real &operator+=(float_f64ei64 x) { MP->add(this, this, imp_real(x, MP)); return *this; }
		imp_real &operator-=(float_f64ei64 x) { MP->sub(this, this, imp_real(x, MP)); return *this; }

		void CopyFrom(const imp_real &x) {
			if_likely (MP == x.MP) {
				MP->copy(this, x);
			} else {
				if (MP) MP->_clear(this);

				x.MP->InitCopy(this, x);
			}
		}
	};

	inline void multi_precision_mutable::Init(imp_real *x, mp_uint_bc precision) const {
		_init(x, precision);
		const_cast<multi_precision *&>(x->MP) = this;
	}

	inline void multi_precision_mutable::InitCopy(imp_real *dst, const imp_real *src) const {
		_init_copy(dst, src);
		const_cast<multi_precision *&>(dst->MP) = this;
	}

	inline void multi_precision_mutable::Clear(imp_real *x) const {
		_clear(x);
		const_cast<multi_precision *&>(x->MP) = nullptr;
	}

	// Copy operator. imp_real does not support bitwise and operation
	inline imp_real &imp_real::operator&=(const imp_real &x) {
		MP->copy(this, x);
		return *this;
	}

	// Copy operator. (Re)init if needed. imp_real does not support bitwise or operation
	inline imp_real &imp_real::operator|=(const imp_real &x) {
		if (MP != x.MP) [[unlikely]] {
			if (MP) MP->_clear(this);

			x.MP->InitCopy(this, x);
		} else {
			MP->copy(this, x);
		}
		return *this;
	}

	// TODO: Eliminate temporary variable
	inline imp_real operator+(imp_real x, const imp_real &y) { return x += y; }
	inline imp_real operator-(imp_real x, const imp_real &y) { return x -= y; }
	inline imp_real operator*(imp_real x, const imp_real &y) { return x *= y; }
	inline imp_real operator/(imp_real x, const imp_real &y) { return x /= y; }

	extern im_export multi_precision MPDouble;
}