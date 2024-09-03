#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>

namespace Imagina {
	enum class PixelDataType : uint32_t {
		invalid = 0x0,

		VectorSizeMask = 0xF000'0000,
		vector2 = 0x1000'0000,
		vector3 = 0x2000'0000,
		vector4 = 0x3000'0000,

		ScalarTypeMask = 0x0FFF'FFFF,

		// Integer
		// Signed
		int8 = 0x10000,
		int16,
		int32,
		int64,

		// Unsigned
		uint8 = 0x10010,
		uint16,
		uint32,
		uint64,

		// Fixed point
		// Fractional part, unsigned integers normalized to [0, 1) (divided by 2^bits)
		fract8 = 0x20000,
		fract16,
		fract32,
		fract64,

		// Color, unsigned integers normalized to [0, 1] (divided by 2^bits - 1)
		monochrome8 = 0x30000,
		monochrome16,

		// Floating point
		float16 = 0x40000, // Reserved
		float32,
		float64,
		float_hr,



		//RG8 = Vector2 | Monochrome8,
		//RG16 = Vector2 | Monochrome16,

		rgb8  = vector3 | monochrome8,
		rgb16 = vector3 | monochrome16,
		
		rgba8  = vector4 | monochrome8,
		rgba16 = vector4 | monochrome16,

		rgb16f = vector3 | float16,
		rgb32f = vector3 | float32,

		rgba16f = vector4 | float16,
		rgba32f = vector4 | float32,

		real_sr = float64,
		real_hr = float_hr,

		complex_sr = vector2 | real_sr,
		complex_hr = vector2 | real_hr,
	};

	struct FieldInfo {
		const char *Name;
		ptrdiff_t Offset;
		PixelDataType Type;

		bool IsScalar() const {
			return ((uint32_t)Type & (uint32_t)PixelDataType::VectorSizeMask) == 0;
		}
		bool IsColor() const {
			PixelDataType vectorSize = (PixelDataType)((uint32_t)Type & (uint32_t)PixelDataType::VectorSizeMask);
			PixelDataType scalarType = (PixelDataType)((uint32_t)Type & (uint32_t)PixelDataType::ScalarTypeMask);
			return (vectorSize == PixelDataType::vector3 || vectorSize == PixelDataType::vector4) &&
				(scalarType == PixelDataType::monochrome8 || scalarType == PixelDataType::monochrome16 || scalarType == PixelDataType::float16 || scalarType == PixelDataType::float32);
		}
		template <typename T>
		T GetScalar(void *base) const;

		template <typename T> // TODO: Constrain T
		T GetColor(void *base) const;
	};

	template<typename T>
	T &GetField(void *base, ptrdiff_t offset) {
		return *(T *)((char *)base + offset);
	}

	template <typename T>
	T FieldInfo::GetScalar(void *base) const {
		switch (Type) {
			case PixelDataType::int8:			return GetField<int8_t>		(base, Offset);
			case PixelDataType::int16:			return GetField<int16_t>	(base, Offset);
			case PixelDataType::int32:			return GetField<int32_t>	(base, Offset);
			case PixelDataType::int64:			return GetField<int64_t>	(base, Offset);

			case PixelDataType::uint8:			return GetField<uint8_t>	(base, Offset);
			case PixelDataType::uint16:			return GetField<uint16_t>	(base, Offset);
			case PixelDataType::uint32:			return GetField<uint32_t>	(base, Offset);
			case PixelDataType::uint64:			return GetField<uint64_t>	(base, Offset);

			case PixelDataType::fract8:			return GetField<uint8_t>	(base, Offset) * 0x1p-8;
			case PixelDataType::fract16:		return GetField<uint16_t>	(base, Offset) * 0x1p-16;
			case PixelDataType::fract32:		return GetField<uint32_t>	(base, Offset) * 0x1p-32;
			case PixelDataType::fract64:		return GetField<uint64_t>	(base, Offset) * 0x1p-64;

			case PixelDataType::monochrome8:	return GetField<uint8_t>	(base, Offset) / 0xFFp0;
			case PixelDataType::monochrome16:	return GetField<uint16_t>	(base, Offset) / 0xFFFFp0;

			//case PixelDataType::Float16:		not supported yet
			case PixelDataType::float32:		return GetField<float>		(base, Offset);
			case PixelDataType::float64:		return GetField<double>		(base, Offset);
			//case PixelDataType::FloatHR:		not supported yet

			default: return T(); // FIXME
		}
	}

	template <typename T>
	T FieldInfo::GetColor(void *base) const {
		switch (Type) {
			case PixelDataType::rgb8: return GetField<rgb8>(base, Offset);
			case PixelDataType::rgb32f: return GetField<rgb32f>(base, Offset);

			case PixelDataType::rgba8: return GetField<rgba8>(base, Offset);
			case PixelDataType::rgba32f: return GetField<rgba32f>(base, Offset);

			default: return T(); // FIXME
		}
	}

	struct PixelDataInfo {
		size_t Size;
		size_t FieldCount;
		const FieldInfo *Fields;

		const FieldInfo *FindField(const char *name) const;
	};
}