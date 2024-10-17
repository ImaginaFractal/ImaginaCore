#include <imagina/multi-precision.h>
#include <algorithm>

namespace Imagina::inline Numerics {
	template <typename T> T Power2(const int64_t &Power);

	template <> inline double Power2<double>(const int64_t &Power) {
		uint64_t Exponent = (0x3FF + Power) << 52;
		return reinterpret_cast<const double &>(Exponent);
	}

	//void DoubleInitContent(double *x) { *x = 0.0; }
	//void DoubleInitContentWithPrecision(double *x, mp_uint_bc) { *x = 0.0; }
	void DoubleInitContent(double *x, mp_uint_bc) { *x = 0.0; }
	void DoubleInitContentCopy(double *dst, const double *src) { *dst = *src; }

	void DoubleClearContent(double *) {}

	mp_uint_bc DoubleGetPrecision(const double *) { return 53; }
	void DoubleSetPrecision(double *, mp_uint_bc) {}

	void DoubleSet(double *dst, const double *src) { *dst = *src; }
	void DoubleSetDouble(double *dst, double src) { *dst = src; }
	void DoubleSetDouble2Exp(double *dst, double src, int_exp exp) { *dst = (double)float_f64ei64(src, exp); }
	void DoubleSetFloatF64eI64(double *dst, float_f64ei64 src) { *dst = double(src); }

	double DoubleGetDouble(const double *x) { return *x; }
	double DoubleGetDouble2Exp(int_exp *exp, const double *x) { *exp = 0; return *x; }
	float_f64ei64 DoubleGetFloatF64eI64(const double *x) { return *x; }

	void DoubleAdd(double *dst, const double *x, const double *y) { *dst = *x + *y; }
	void DoubleSub(double *dst, const double *x, const double *y) { *dst = *x - *y; }
	void DoubleMul(double *dst, const double *x, const double *y) { *dst = *x * *y; }
	void DoubleDiv(double *dst, const double *x, const double *y) { *dst = *x / *y; }

	void DoubleNeg(double *dst, const double *x) { *dst = -*x; }
	void DoubleAbs(double *dst, const double *x) { *dst = std::abs(*x); }

	multi_precision MPDouble{ {
		.Name = "double",

		//._init = (void (*)(imp_real *))DoubleInitContent,
		//.InitContentWithPrecision = (void (*)(imp_real *, mp_uint_bc))DoubleInitContentWithPrecision,
		._init = (void (*)(imp_real *, mp_uint_bc))DoubleInitContent,
		._init_copy = (void (*)(imp_real *, const imp_real *))DoubleInitContentCopy,


		._clear = (void (*)(imp_real *))DoubleClearContent,

		.get_precision = (mp_uint_bc(*)(const imp_real *))DoubleGetPrecision,
		.set_precision = (void (*)(imp_real *, mp_uint_bc))DoubleSetPrecision,

		.set = (void (*)(imp_real *, const imp_real *))DoubleSet,
		.copy = (void (*)(imp_real *, const imp_real *))DoubleSet,
		.set_double = (void (*)(imp_real *, double))DoubleSetDouble,
		.set_double_2exp = (void (*)(imp_real *, double, int_exp))DoubleSetDouble2Exp,
		.set_float_f64ei64 = (void (*)(imp_real *, float_f64ei64)) DoubleSetFloatF64eI64,

		.get_double = (double (*)(const imp_real *))DoubleGetDouble,
		.get_double_2exp = (double (*)(int_exp *, const imp_real *))DoubleGetDouble2Exp,
		.get_float_f64ei64 = (float_f64ei64 (*)(const imp_real *))DoubleGetFloatF64eI64,

		.add = (void (*)(imp_real *, const imp_real *, const imp_real *))DoubleAdd,
		.sub = (void (*)(imp_real *, const imp_real *, const imp_real *))DoubleSub,
		.mul = (void (*)(imp_real *, const imp_real *, const imp_real *))DoubleMul,
		.div = (void (*)(imp_real *, const imp_real *, const imp_real *))DoubleDiv,

		.neg = (void (*)(imp_real *, const imp_real *))DoubleNeg,
		.abs = (void (*)(imp_real *, const imp_real *))DoubleAbs,
	} };
}