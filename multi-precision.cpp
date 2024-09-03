#include <imagina/multi-precision.h>
#include <algorithm>

namespace Imagina::inline Numerics {
	template <typename T> T Power2(const int64_t &Power);

	template <> inline double Power2<double>(const int64_t &Power) {
		uint64_t Exponent = (0x3FF + Power) << 52;
		return reinterpret_cast<const double &>(Exponent);
	}

	//void DoubleInitContent(double *x) { *x = 0.0; }
	//void DoubleInitContentWithPrecision(double *x, MPBCUInt) { *x = 0.0; }
	void DoubleInitContent(double *x, MPBCUInt) { *x = 0.0; }
	void DoubleInitContentCopy(double *dst, const double *src) { *dst = *src; }

	void DoubleClearContent(double *) {}

	MPBCUInt DoubleGetPrecision(const double *) { return 53; }
	void DoubleSetPrecision(double *, MPBCUInt) {}

	void DoubleSet(double *dst, const double *src) { *dst = *src; }
	void DoubleSetDouble(double *dst, double src) { *dst = src; }
	void DoubleSetFloatF64eI64(double *dst, float_f64ei64 src) { *dst = double(src); }

	double DoubleGetDouble(const double *x) { return *x; }
	float_f64ei64 DoubleGetFloatF64eI64(const double *x) { return *x; }

	void DoubleAdd(double *dst, const double *x, const double *y) { *dst = *x + *y; }
	void DoubleSub(double *dst, const double *x, const double *y) { *dst = *x - *y; }
	void DoubleMul(double *dst, const double *x, const double *y) { *dst = *x * *y; }
	void DoubleDiv(double *dst, const double *x, const double *y) { *dst = *x / *y; }

	MultiPrecision MPDouble{ {
		.Name = "double",

		//.InitContent = (void (*)(MPReal *))DoubleInitContent,
		//.InitContentWithPrecision = (void (*)(MPReal *, MPBCUInt))DoubleInitContentWithPrecision,
		.InitContent = (void (*)(MPReal *, MPBCUInt))DoubleInitContent,
		.InitContentCopy = (void (*)(MPReal *, const MPReal *))DoubleInitContentCopy,


		.ClearContent = (void (*)(MPReal *))DoubleClearContent,

		.GetPrecision = (MPBCUInt(*)(const MPReal *))DoubleGetPrecision,
		.SetPrecision = (void (*)(MPReal *, MPBCUInt))DoubleSetPrecision,

		.Set = (void (*)(MPReal *, const MPReal *))DoubleSet,
		.Copy = (void (*)(MPReal *, const MPReal *))DoubleSet,
		.SetDouble = (void (*)(MPReal *, double))DoubleSetDouble,
		.SetFloatF64eI64 = (void (*)(MPReal *, float_f64ei64)) DoubleSetFloatF64eI64,

		.GetDouble = (double (*)(const MPReal *))DoubleGetDouble,
		.GetFloatF64eI64 = (float_f64ei64 (*)(const MPReal *))DoubleGetFloatF64eI64,

		.Add = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleAdd,
		.Sub = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleSub,
		.Mul = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleMul,
		.Div = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleDiv,
	} };
}