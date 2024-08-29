#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>

namespace Imagina {
	enum class PixelDataType : uint32_t {
		Invalid = 0x0,

		VectorSizeMask = 0xF000'0000,
		Vector2 = 0x1000'0000,
		Vector3 = 0x2000'0000,
		Vector4 = 0x3000'0000,

		ScalarTypeMask = 0x0FFF'FFFF,

		// Integer
		// Signed
		Int8 = 0x10000,
		Int16,
		Int32,
		Int64,

		// Unsigned
		UInt8 = 0x10010,
		UInt16,
		UInt32,
		UInt64,

		// Fixed point
		// Fractional part, unsigned integers normalized to [0, 1) (divided by 2^bits)
		Fract8 = 0x20000,
		Fract16,
		Fract32,
		Fract64,

		// Color, unsigned integers normalized to [0, 1] (divided by 2^bits - 1)
		Monochrome8 = 0x30000,
		Monochrome16,

		// Floating point
		Float16 = 0x40000, // Reserved
		Float32,
		Float64,
		FloatHR,



		//RG8 = Vector2 | Monochrome8,
		//RG16 = Vector2 | Monochrome16,

		RGB8 = Vector3 | Monochrome8,
		RGB16 = Vector3 | Monochrome16,
		
		RGBA8 = Vector4 | Monochrome8,
		RGBA16 = Vector4 | Monochrome16,

		RGB16F = Vector3 | Float16,
		RGB32F = Vector3 | Float32,

		RGBA16F = Vector4 | Float16,
		RGBA32F = Vector4 | Float32,

		SRReal = Float64,
		HRReal = FloatHR,

		SRComplex = Vector2 | SRReal,
		HRComplex = Vector2 | HRReal,
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
			return (vectorSize == PixelDataType::Vector3 || vectorSize == PixelDataType::Vector4) &&
				(scalarType == PixelDataType::Monochrome8 || scalarType == PixelDataType::Monochrome16 || scalarType == PixelDataType::Float16 || scalarType == PixelDataType::Float32);
		}
		template <typename T>
		T GetScalar(void *base) const;

		uint32_t GetRGBA8(void *base) const;

	private:
		static float clamp01(float x) { return (x < 0.0f) ? 0.0f : (x > 1.0f) ? 1.0f : x; }

		static uint32_t RGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) {
			return uint32_t(r)
				| (uint32_t(g) << 8)
				| (uint32_t(b) << 16)
				| (uint32_t(b) << 24);
		}
		static uint32_t RGBA8(float r, float g, float b, float a = 1.0) {
			return (uint32_t)std::round(clamp01(r) * 255.0f)
				| ((uint32_t)std::round(clamp01(g) * 255.0f) << 8)
				| ((uint32_t)std::round(clamp01(b) * 255.0f) << 16)
				| ((uint32_t)std::round(clamp01(a) * 255.0f) << 24);
		}
	};

	template<typename T>
	T &GetField(void *base, ptrdiff_t offset) {
		return *(T *)((char *)base + offset);
	}

	template <typename T>
	T FieldInfo::GetScalar(void *base) const {
		switch (Type) {
			case PixelDataType::Int8:			return GetField<int8_t>		(base, Offset);
			case PixelDataType::Int16:			return GetField<int16_t>	(base, Offset);
			case PixelDataType::Int32:			return GetField<int32_t>	(base, Offset);
			case PixelDataType::Int64:			return GetField<int64_t>	(base, Offset);

			case PixelDataType::UInt8:			return GetField<uint8_t>	(base, Offset);
			case PixelDataType::UInt16:			return GetField<uint16_t>	(base, Offset);
			case PixelDataType::UInt32:			return GetField<uint32_t>	(base, Offset);
			case PixelDataType::UInt64:			return GetField<uint64_t>	(base, Offset);

			case PixelDataType::Fract8:			return GetField<uint8_t>	(base, Offset) * 0x1p-8;
			case PixelDataType::Fract16:		return GetField<uint16_t>	(base, Offset) * 0x1p-16;
			case PixelDataType::Fract32:		return GetField<uint32_t>	(base, Offset) * 0x1p-32;
			case PixelDataType::Fract64:		return GetField<uint64_t>	(base, Offset) * 0x1p-64;

			case PixelDataType::Monochrome8:	return GetField<uint8_t>	(base, Offset) / 0xFFp0;
			case PixelDataType::Monochrome16:	return GetField<uint16_t>	(base, Offset) / 0xFFFFp0;

			//case PixelDataType::Float16:		not supported yet
			case PixelDataType::Float32:		return GetField<float>		(base, Offset);
			case PixelDataType::Float64:		return GetField<double>		(base, Offset);
			//case PixelDataType::FloatHR:		not supported yet

			default: return T(); // FIXME
		}
	}

	inline uint32_t FieldInfo::GetRGBA8(void *base) const {
		switch (Type) {
			case PixelDataType::RGB8: return RGBA8(
				GetField<uint8_t>(base, Offset),
				GetField<uint8_t>(base, Offset + 1),
				GetField<uint8_t>(base, Offset + 2));
			case PixelDataType::RGB32F: return RGBA8(
				GetField<float>(base, Offset),
				GetField<float>(base, Offset + 4),
				GetField<float>(base, Offset + 8));
			
			
			case PixelDataType::RGBA8: return GetField<uint32_t>(base, Offset);
			case PixelDataType::RGBA32F: return RGBA8(
				GetField<float>(base, Offset),
				GetField<float>(base, Offset + 4),
				GetField<float>(base, Offset + 8),
				GetField<float>(base, Offset + 12));

			default: return 0; // FIXME
		}
	}

	struct PixelDataInfo {
		size_t Size;
		size_t FieldCount;
		const FieldInfo *Fields;

		const FieldInfo *FindField(const char *name) const;
	};
}