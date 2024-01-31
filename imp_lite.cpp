#include <Imagina/imp_lite>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <bit>
#include <utility>
#include <Imagina/floating_point>

namespace Imagina::MPLite {
	uint32_t PrecisionToSize(uintptr_t precision) {
		uint32_t size = (precision + 32) / 32; // ceil((precision + 1) / 32)
		if (size < 2) size = 2;
		return size;
	}

	uintptr_t SizeToPrecision(uint32_t size) {
		return uintptr_t(size) * 32;
	}

	void Float::Init(Float *x, uintptr_t precision) {
		memset(x, 0, sizeof(Float));
		uint32_t size = PrecisionToSize(precision);
		x->SignSize = size;
		if (size > BufferSize) {
			x->Pointer = (uint32_t *)calloc(size, sizeof(uint32_t));
		}
	}
	void Float::InitCopy(Float *x, const Float *src) {
		x->SignSize = src->SignSize;
		x->Exponent = src->Exponent;
		uint32_t size = src->Size;
		if (size > BufferSize) {
			x->Pointer = (uint32_t *)malloc(size * sizeof(uint32_t));
			assert(x->Pointer);
			memcpy(x->Pointer, src->Pointer, size * sizeof(uint32_t));
		} else {
			memcpy(x->Buffer, src->Buffer, size * sizeof(uint32_t));
		}
	}

	void Float::Clear(Float *x) {
		uint32_t size = x->Size;
		if (size > BufferSize) {
			free(x->Pointer);
		}
		x->SignSize = 0;
	}

	uintptr_t Float::GetPrecision(const Float *x) {
		return SizeToPrecision(x->Size);
	}

	void Float::SetPrecision(Float *x, uintptr_t precision) {
		uint32_t size = x->Size;
		uint32_t newSize = PrecisionToSize(precision);
		if (size == newSize) return;
		x->Size = newSize;
		if (size <= BufferSize) {
			if (newSize <= BufferSize) {
				if (newSize < size) {
					memmove(x->Buffer, x->Buffer + (size - newSize), newSize * sizeof(uint32_t));
				} else {
					memmove(x->Buffer + (newSize - size), x->Buffer, size * sizeof(uint32_t));
					memset(x->Buffer, 0, (newSize - size) * sizeof(uint32_t));
				}
			} else {
				uint32_t *data = (uint32_t *)malloc(newSize * sizeof(uint32_t));
				assert(data);
				memset(data, 0, (newSize - size) * sizeof(uint32_t));
				memcpy(data + (newSize - size), x->Buffer, size * sizeof(uint32_t));
				x->Pointer = data;
			}
		} else if (newSize <= BufferSize) {
			uint32_t *data = x->Pointer;
			memcpy(x->Buffer, data + (size - newSize), newSize * sizeof(uint32_t));
			free(data);
		} else {
			if (newSize < size) {
				memmove(x->Pointer, x->Pointer + (size - newSize), newSize * sizeof(uint32_t));
			}
			x->Pointer = (uint32_t *)realloc(x->Pointer, newSize * sizeof(uint32_t));
			assert(x->Pointer);
			if (newSize > size) {
				memmove(x->Pointer + (newSize - size), x->Pointer, size * sizeof(uint32_t));
				memset(x->Pointer, 0, (newSize - size) * sizeof(uint32_t));
			}
		}
	}

	inline void Float::UnsignedSet(Float *x, const Float *src) {
		x->Exponent = src->Exponent;
		if_unlikely(src->Exponent == INT32_MIN) return;

		uint32_t size = x->Size;
		uint32_t srcSize = src->Size;

		uint32_t *dstData = x->Data();
		const uint32_t *srcData = src->Data();

		if (srcSize > size) {
			srcData += srcSize - size;
			srcSize = size;
		} else if (srcSize < size) {
			memset(dstData, 0, (size - srcSize) * sizeof(uint32_t));
			dstData += size - srcSize;
		}

		memcpy(dstData, srcData, srcSize * sizeof(uint32_t));
	}

	void Float::Set(Float *x, const Float *src) {
		x->Sign = src->Sign;

		UnsignedSet(x, src);
	}

	void Float::Copy(Float *x, const Float *src) {
		uint32_t size = x->Size, newSize = src->Size;
		if (size != newSize) {
			if (size > BufferSize) free(x->Pointer);
			if (newSize > BufferSize) x->Pointer = (uint32_t *)malloc(newSize * sizeof(uint32_t));
		}
		x->SignSize = src->SignSize;
		x->Exponent = src->Exponent;
		if_unlikely(src->Exponent == INT32_MIN) return;

		memcpy((newSize > BufferSize) ? x->Pointer : x->Buffer, (newSize > BufferSize) ? src->Pointer : src->Buffer, newSize * sizeof(uint32_t));
	}

	void Float::SetDouble(Float *x, double d) { // FIXME: Denormal
		if_unlikely (d == 0.0) {
			x->Exponent = INT32_MIN;
			return;
		}
		uint32_t size = x->Size;
		uint32_t *data = x->Data();
		
		uint64_t bits = std::bit_cast<uint64_t>(d);
		x->Sign = bits >> 63;
		x->Exponent = int64_t((bits >> 52) & 0x7FF) - 0x3FE;
		uint64_t Mantissa = (bits << 11) | (1ULL << 63);

		data[size - 1] = Mantissa >> 32;
		// if_unlikely(size < 2) return;
		data[size - 2] = (uint32_t)Mantissa;
		if (size == 2) return;

		memset(data, 0, (size - 2) * sizeof(uint32_t));
	}

	void Float::SetFloatF64eI64(Float *x, Imagina::FloatF64eI64 f) {
		if_unlikely (f.IsZero()) {
			x->Exponent = INT32_MIN;
			return;
		}
		SetDouble(x, f.Mantissa);
		x->Exponent += f.Exponent;
	}

	double Float::GetDouble(const Float *x) {
		uint64_t result;
		if (x->Exponent >= 0x7FF - 0x3FE) {
			//return std::bit_cast<double>(0x7FF0'0000'0000'0000ULL | (uint64_t(x->Sign) << 63));
			result = 0x7FF0'0000'0000'0000ULL;
		} else if (x->Exponent <= -0x3FE) {
			result = 0;
		} else {
			uint32_t size = x->Size;
			const uint32_t *data = x->Data();
			uint64_t Mantissa = (uint64_t(data[size - 1]) << 32) | data[size - 2];
			result = (Mantissa >> 11) & 0x000F'FFFF'FFFF'FFFFULL;
			result |= uint64_t(x->Exponent + 0x3FE) << 52;
		}
		result |= uint64_t(x->Sign) << 63;
		return std::bit_cast<double>(result);
	}

	Imagina::FloatF64eI64 Float::GetFloatF64eI64(const Float *x) {
		if_unlikely(x->Exponent == INT32_MIN) {
			return Imagina::FloatF64eI64();
		}

		uint32_t size = x->Size;
		const uint32_t *data = (size > BufferSize) ? x->Pointer : x->Buffer;
		uint64_t Mantissa = (uint64_t(data[size - 1]) << 32) | data[size - 2];
		return Imagina::FloatF64eI64(x->Sign ? -double(Mantissa) : double(Mantissa), x->Exponent - 64);
	}

	void Float::Add(Float *result, const Float *x, const Float *y) {
		if (x->Sign == y->Sign) {
			UnsignedAdd(result, x, y);
		} else {
			UnsignedSub(result, x, y);
		}
	}

	void Float::Sub(Float *result, const Float *x, const Float *y) {
		if (x->Sign == y->Sign) {
			UnsignedSub(result, x, y);
		} else {
			UnsignedAdd(result, x, y);
		}
	}

	void Float::Mul(Float *result, const Float *x, const Float *y) {
		if_unlikely(x->Exponent == INT32_MIN || y->Exponent == INT32_MIN) {
			result->Exponent = INT32_MIN;
			return;
		}
		
		if (y == result) {
			std::swap(x, y);
		}

		uint32_t rsize = result->Size;
		uint32_t xsize = x->Size;
		uint32_t ysize = y->Size;

		uint32_t *rdata = result->Data();
		const uint32_t *xdata = x->Data();
		const uint32_t *ydata = y->Data();

		uint32_t rindex = 0;

		if (rsize > xsize) {
			memset(rdata, 0, (rsize - xsize) * sizeof(uint32_t));
			uint32_t productSize = xsize + ysize;
			if (rsize > productSize) {
				rdata += rsize - productSize;
				rsize = productSize;
			}
			rindex = rsize - xsize;
		} else {
			xdata += xsize - rsize;
			xsize = rsize;
		}

		uint32_t lowWord = 0x8000'0000; // For rounding

		for (uint32_t xindex = 0; xindex < xsize; xindex++, rindex++) {
			uint32_t i;
			uint32_t j;
			uint64_t factor = xdata[xindex];
			uint64_t product;
			if (rindex < ysize) {
				i = 0;
				j = ysize - rindex;
				product = lowWord + factor * ydata[j - 1];
				lowWord = (uint32_t)product;
				product >>= 32;
			} else {
				i = rindex - ysize;
				j = 0;
				product = 0;
			}
			while (j < ysize) {
				product += rdata[i] + factor * ydata[j];
				rdata[i] = product;
				product >>= 32;
				i++; j++;
			}
			rdata[i] = (uint32_t)product;
		}
		result->Sign = x->Sign ^ y->Sign;
		result->Exponent = x->Exponent + y->Exponent;

		if (!(rdata[rsize - 1] & 0x8000'0000)) {
			uint32_t loword = 0;
			for (uint32_t i = 0; i < rsize; i++) {
				uint32_t hiword = rdata[i];
				rdata[i] = (hiword << 1) | (loword >> 31);
				loword = hiword;
			}
			result->Exponent--;
		}
	}

	void Float::Div(Float *result, const Float *x, const Float *y) {
		throw ""; // FIXME
	}

	bool Float::MagnitudeGreater(const Float *x, const Float *y) {
		if (x->Exponent > y->Exponent) return true;
		if (x->Exponent < y->Exponent || y->Exponent == INT32_MIN) return false;

		const uint32_t *xdata = x->Data();
		const uint32_t *ydata = y->Data();

		uint32_t i = x->Size, j = y->Size;
		while (i-- > 0 && j-- > 0) {
			if (xdata[i] > ydata[j]) {
				return true;
			} else if (xdata[i] < ydata[j]) {
				return false;
			}
		}

		return i > 0;
	}

	// result = (a << shift) + b
	uint32_t ShlAdd1(uint32_t *result, const uint32_t *a, uint32_t shift, uint32_t b, uint32_t size) {
		assert(shift <= 32);
		uint64_t carry = b;
		for (uint32_t i = 0; i < size; i++) {
			carry += uint64_t(a[i]) << shift;
			result[i] = uint32_t(carry);
			carry >>= 32;
		}
		return uint32_t(carry);
	}

	// result = (-a << shift) + b
	int32_t NegShlAdd1(uint32_t *result, const uint32_t *a, uint32_t shift, int32_t b, uint32_t size) {
		assert(shift <= 32);
		int64_t carry = b;
		for (uint32_t i = 0; i < size; i++) {
			carry -= int64_t(a[i]) << shift;
			result[i] = uint32_t(carry);
			carry >>= 32;
		}
		return int32_t(carry);
	}

	// Sign of y is ignored
	void Float::UnsignedAdd(Float *result, const Float *x, const Float *y) {
		UnsignedAddSub<true>(result, x, y);
	}
	// Sign of y is ignored
	void Float::UnsignedSub(Float *result, const Float *x, const Float *y) {
		UnsignedAddSub<false>(result, x, y);
	}
	template<bool IsAdd>
	void Float::UnsignedAddSub(Float *result, const Float *x, const Float *y) {
		using CarryType = std::conditional<IsAdd, uint64_t, int64_t>::type;

		result->Sign = x->Sign;
		if constexpr (IsAdd) {
			if (x->Exponent < y->Exponent) std::swap(x, y);
		} else {
			if (MagnitudeGreater(y, x)) {
				std::swap(x, y);
				result->Sign = ~result->Sign;
			}
		}
		if (x->Exponent == INT32_MIN) {
			result->Exponent = INT32_MIN;
			return;
		}

		uint32_t exponentDifference = uint32_t(x->Exponent - y->Exponent);

		uint32_t rsize = result->Size;

		//if_unlikely(y->Exponent == INT32_MIN) {
		if (uintptr_t(exponentDifference) >= uintptr_t(rsize) * 32) {
			if (result != x) UnsignedSet(result, x);
			return;
		}

		uint32_t xsize = x->Size;
		uint32_t ysize = y->Size;

		uint32_t shift = -exponentDifference & 0x1F;
		exponentDifference = (exponentDifference + shift) >> 5;

		uint32_t *rdata = result->Data();
		uint32_t *rdata2 = rdata;
		const uint32_t *xdata = x->Data();
		const uint32_t *ydata = y->Data();


		CarryType carry = 0;
		uint32_t yalign = ysize + exponentDifference;

		// TODO: Ensure ysize doesn't underflow
		// FIXME: Loss of precision in catastrophic cancellation if rsize is much smaller than xsize and ysize

		if (uint32_t temp = std::max(xsize, yalign); temp < rsize) { // Result is too large: shrink result
			uint32_t diff = rsize - temp;
			memset(rdata, 0, diff * sizeof(uint32_t));
			rdata += diff;
			rsize = temp;
		} else {
			if (xsize > rsize) {
				xdata += xsize - rsize;
				xsize = rsize;

				carry = xdata[-1];
			}

			if (yalign > rsize) {
				uint32_t diff = yalign - rsize;

				ydata += diff;
				ysize -= diff;
				yalign -= diff;

				if (IsAdd) {
					carry += CarryType(ydata[-1]) << shift;
				} else {
					carry -= CarryType(ydata[-1]) << shift;
				}
			}

			carry += 0x8000'0000; // Round to nearest
			carry >>= 32;
		}

		if (yalign > xsize) {
			uint32_t diff = yalign - xsize;
			assert(ysize >= diff);
			if (IsAdd) {
				carry = ShlAdd1(rdata, ydata, shift, carry, diff);
			} else {
				carry = NegShlAdd1(rdata, ydata, shift, carry, diff);
			}
			ydata += diff;
			ysize -= diff;
			yalign -= diff;
			rdata += diff;
			rsize -= diff;
			assert(yalign == rsize);
		} else {
			uint32_t diff = xsize - yalign;
			if (result != x) memcpy(rdata, xdata, diff * sizeof(uint32_t)); // FIXME: carry
			carry = 0;
			xdata += diff;
			xsize -= diff;
			rdata += diff;
			rsize -= diff;
			assert(xsize == rsize);
		}

		uint32_t i = 0;

		for (; i < ysize; i++) {
			if (IsAdd) {
				carry += CarryType(xdata[i]) + (CarryType(ydata[i]) << shift);
			} else {
				carry += CarryType(xdata[i]) - (CarryType(ydata[i]) << shift);
			}
			rdata[i] = uint32_t(carry);
			carry >>= 32;
		}

		for (; i < xsize; i++) {
			carry += CarryType(xdata[i]);
			rdata[i] = uint32_t(carry);
			carry >>= 32;
		}

		result->Exponent = x->Exponent;

		rdata = result->Data();
		rsize = result->Size;

		if constexpr (IsAdd) {
			if (carry) {
				assert(carry == 1);

				carry = rdata[0] >> 1;
				for (i = 1; i < rsize; i++) {
					carry |= uint64_t(rdata[i]) << 31;
					rdata[i - 1] = uint32_t(carry);
					carry >>= 32;
				}
				rdata[i - 1] = uint32_t(carry) | 0x8000'0000;
				result->Exponent++;
			}
		} else {
			assert(!carry);

			i = rsize;
			while (!rdata[--i]) if (i == 0) {
				result->Exponent = INT32_MIN;
				return;
			}

			uint32_t lshift = std::countl_zero(rdata[i]);
			result->Exponent -= (rsize - 1 - i) * 32 + lshift;

			uint32_t j = rsize;
			if (lshift == 0) {
				do {
					j--;
					rdata[j] = rdata[i];
				} while (i-- > 0);
			} else {
				uint32_t rshift = 32 - lshift;
				uint32_t hiword = rdata[i];

				while (i-- > 0) {
					j--;

					uint32_t loword = rdata[i];
					rdata[j] = (hiword << lshift) | (loword >> rshift);
					hiword = loword;
				}
				if (j == 0) return;
				j--;
				rdata[j] = hiword << lshift;
			}
			if (j > 0) memset(rdata, 0, j * sizeof(uint32_t));
		}
	}
}

namespace Imagina {
	MultiPrecision IMPLite{ {
		.Name = "IMPLite",

		.InitContent		= (pMultiPrecision_InitContent)		MPLite::Float::Init,
		.InitContentCopy	= (pMultiPrecision_InitContentCopy)	MPLite::Float::InitCopy,

		.ClearContent		= (pMultiPrecision_ClearContent)	MPLite::Float::Clear,

		.GetPrecision		= (pMultiPrecision_GetPrecision)	MPLite::Float::GetPrecision,
		.SetPrecision		= (pMultiPrecision_SetPrecision)	MPLite::Float::SetPrecision,

		.Set				= (pMultiPrecision_Set)				MPLite::Float::Set,
		.Copy				= (pMultiPrecision_Copy)			MPLite::Float::Copy,
		.SetDouble			= (pMultiPrecision_SetDouble)		MPLite::Float::SetDouble,
		.SetFloatF64eI64	= (pMultiPrecision_SetFloatF64eI64)	MPLite::Float::SetFloatF64eI64,

		.GetDouble			= (pMultiPrecision_GetDouble)		MPLite::Float::GetDouble,
		.GetFloatF64eI64	= (pMultiPrecision_GetFloatF64eI64)	MPLite::Float::GetFloatF64eI64,

		.Add				= (pMultiPrecision_Add)				MPLite::Float::Add,
		.Sub				= (pMultiPrecision_Sub)				MPLite::Float::Sub,
		.Mul				= (pMultiPrecision_Mul)				MPLite::Float::Mul,
		.Div				= (pMultiPrecision_Div)				MPLite::Float::Div,
	} };

}