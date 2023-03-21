#include "MultiPrecision.h"
#include <algorithm>

namespace Imagina {
	template <typename T> T Power2(const int64_t &Power);

	template <> inline double Power2<double>(const int64_t &Power) {
		uint64_t Exponent = (0x3FF + Power) << 52;
		return reinterpret_cast<const double &>(Exponent);
	}

	size_t FixedGeneric32::DefaultSize = 4;

	FixedGeneric32::FixedGeneric32(const FixedGeneric32 &x) : Value(new uint32_t[x.Size]), Size(x.Size) {
		for (size_t i = 0; i < Size; i++) {
			Value[i] = x.Value[i];
		}
	}

	FixedGeneric32::FixedGeneric32(FixedGeneric32 &&x) : Value(x.Value), Size(x.Size) {
		x.Value = nullptr;
		x.Size = 0;
	}

	FixedGeneric32::FixedGeneric32(double n) : Value(new uint32_t[DefaultSize]) {
		int64_t NumberOfWords = Size;
		intptr_t LSBOffset = 8 - NumberOfWords * 32;
		if (n == 0.0) {
			for (int64_t i = 0; i < NumberOfWords; i++) {
				Value[i] = 0;
			}
			return;
		}
		int64_t temp = reinterpret_cast<const uint64_t &>(n);
		int64_t Fraction = (temp & 0x000FFFFFFFFFFFFF) | 0x0010000000000000;
		int64_t Exponent = temp >> 52 & 0x7FF;
		bool Sign = bool(temp >> 63);
		if (Sign)
			Fraction = -Fraction;
		Exponent -=
			+ 0x3FF //Zero offset
			+ 52   //Fraction size
			+ LSBOffset;

		for (int64_t i = 0, j = std::min(Exponent / 32, NumberOfWords); i < j; i++) {
			Value[i] = 0;
		}
		if (Exponent >= 0 && Exponent < 32 * NumberOfWords) {
			Value[Exponent / 32] = Fraction << (Exponent % 32);
		}
		Exponent += 31;
		if (Exponent >= 0 && Exponent < 32 * NumberOfWords) {
			Value[Exponent / 32] = Fraction >> (31 - Exponent % 32);
		}
		Exponent += 32;
		Fraction >>= 32;
		if (Exponent >= 0 && Exponent < 32 * NumberOfWords) {
			Value[Exponent / 32] = Fraction >> (31 - Exponent % 32);
		}
		for (int64_t i = std::max(Exponent / 32 + 1, 0ll); i < NumberOfWords; i++) {
			Value[i] = Sign ? ~0l : 0l;
		}
	}

	FixedGeneric32 &FixedGeneric32::operator=(const FixedGeneric32 &x) {
		if (Size != x.Size) {
			Size = x.Size;
			if (Value) delete[] Value;
			Value = new uint32_t[Size];
		}
		for (size_t i = 0; i < Size; i++) {
			Value[i] = x.Value[i];
		}
		return *this;
	}

	FixedGeneric32 &FixedGeneric32::operator=(FixedGeneric32 &&x) {
		if (Value) delete[] Value;
		Value = x.Value;
		Size = x.Size;
		x.Value = nullptr;
		x.Size = 0;
		return *this;
	}

	FixedGeneric32::operator double() const {
		//return operator FExpDouble();
		size_t NumberOfWords = Size;
		intptr_t LSBOffset = 8 - NumberOfWords * 32;
		if (!IsNegative()) {
			for (size_t i = NumberOfWords - 1; i > 0; i--) {
				if (Value[i] != 0) {
					double LowPart = double(Value[i - 1]);
					double HighPart = double(Value[i]);
					LowPart *= Power2<double>(LSBOffset + 32 * (i - 1));
					(uint64_t &)HighPart += (LSBOffset + 32 * i) << 52;
					return HighPart + LowPart;
				}
			}
			double Temp = double(Value[0]);
			//(uint64_t &)Temp += LSBOffset << 52;
			return Temp * Power2<double>(LSBOffset);
		} else {
			for (size_t i = NumberOfWords - 1; i > 0; i--) {
				if (Value[i] != UINT32_MAX || ((int32_t *)Value)[i - 1] >= 0) {
					double LowPart = double(Value[i - 1]);
					double HighPart = double(((int32_t *)Value)[i]);
					LowPart *= Power2<double>(LSBOffset + 32 * (i - 1));
					(uint64_t &)HighPart += (LSBOffset + 32 * i) << 52;
					return HighPart + LowPart;
				}
			}
			double Temp = double(((int32_t *)Value)[0]);
			//(uint64_t &)Temp += LSBOffset << 52;
			return Temp * Power2<double>(LSBOffset);
		}
	}

	FixedGeneric32 &FixedGeneric32::operator+=(const FixedGeneric32 &x) {
		uint64_t temp = 0;
		for (size_t i = 0; i < Size; i++) {
			temp = (uint64_t)Value[i] + x.Value[i] + (temp > UINT32_MAX);
			Value[i] = (uint32_t)temp;
		}
		return *this;
	}

	FixedGeneric32 &FixedGeneric32::operator-=(const FixedGeneric32 &x) {
		int64_t temp = 0;
		for (size_t i = 0; i < Size; i++) {
			temp = (int64_t)Value[i] - x.Value[i] - (temp < 0);
			Value[i] = (uint32_t)temp;
		}
		return *this;
	}

	FixedGeneric32 &FixedGeneric32::operator*=(const FixedGeneric32 &x) {
		uint64_t PartialProduct = 0;
		uint32_t PrevPartialProduct;
		int32_t Carry = 0;
		bool Neg = IsNegative();
		bool xNeg = x.IsNegative();
		for (size_t i = 0; i < Size; i++) {
			//PartialProduct = 0;
			for (size_t j = i, k = Size; j < Size; j++) {
				k--;
				uint64_t temp = PartialProduct + (uint64_t)Value[j] * x.Value[k];
				Carry += temp < PartialProduct;
				PartialProduct = temp;
			}

			if (i != 0) {
				if (Neg) {
					uint64_t temp = PartialProduct - x.Value[i - 1];
					Carry -= temp > PartialProduct;
					PartialProduct = temp;
				}
				if (xNeg) {
					uint64_t temp = PartialProduct - Value[i - 1];
					Carry -= temp > PartialProduct;
					PartialProduct = temp;
				}
				Value[i - 1] = ((uint32_t)PartialProduct << 8) | (PrevPartialProduct >> 24);
			}
			PrevPartialProduct = uint32_t(PartialProduct);
			PartialProduct >>= 32;
			PartialProduct |= (uint64_t)Carry << 32;
			Carry >>= 31;
		}
		if (Neg) {
			PartialProduct -= x.Value[Size - 1];
		}
		if (xNeg) {
			PartialProduct -= Value[Size - 1];
		}
		Value[Size - 1] = ((uint32_t)PartialProduct << 8) | (PrevPartialProduct >> 24);
		//Value[Size - 1] = (uint32_t)PartialProduct;
		return *this;
	}

	//void DoubleInitContent(double *x) { *x = 0.0; }
	//void DoubleInitContentWithPrecision(double *x, MPBCUint) { *x = 0.0; }
	void DoubleInitContent(double *x, MPBCUint) { *x = 0.0; }
	void DoubleInitContentCopy(double *dst, const double *src) { *dst = *src; }

	void DoubleClearContent(double *) {}

	MPBCUint DoubleGetPrecision(const double *) { return 53; }
	void DoubleSetPrecision(double *, MPBCUint) {}

	void DoubleSet(double *dst, const double *src) { *dst = *src; }
	void DoubleSetDouble(double *dst, double src) { *dst = src; }
	double DoubleToDouble(const double *x) { return *x; }

	void DoubleAdd(double *dst, const double *x, const double *y) { *dst = *x + *y; }
	void DoubleSub(double *dst, const double *x, const double *y) { *dst = *x - *y; }
	void DoubleMul(double *dst, const double *x, const double *y) { *dst = *x * *y; }
	void DoubleDiv(double *dst, const double *x, const double *y) { *dst = *x / *y; }

	MultiPrecision MPDouble{
		.Name = "double",

		//.InitContent = (void (*)(MPReal *))DoubleInitContent,
		//.InitContentWithPrecision = (void (*)(MPReal *, MPBCUint))DoubleInitContentWithPrecision,
		.InitContent = (void (*)(MPReal *, MPBCUint))DoubleInitContent,
		.InitContentCopy = (void (*)(MPReal *, const MPReal *))DoubleInitContentCopy,


		.ClearContent = (void (*)(MPReal *))DoubleClearContent,

		.GetPrecision = (MPBCUint(*)(const MPReal *))DoubleGetPrecision,
		.SetPrecision = (void (*)(MPReal *, MPBCUint))DoubleSetPrecision,

		.Set = (void (*)(MPReal *, const MPReal *))DoubleSet,
		.SetDouble = (void (*)(MPReal *, double))DoubleSetDouble,
		.ToDouble = (double (*)(const MPReal *))DoubleToDouble,

		.Add = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleAdd,
		.Sub = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleSub,
		.Mul = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleMul,
		.Div = (void (*)(MPReal *, const MPReal *, const MPReal *))DoubleDiv,
	};
}