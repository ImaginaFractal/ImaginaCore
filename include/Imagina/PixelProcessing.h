#pragma once

#include "Declarations.h"
#include <string_view>

namespace Imagina {
	enum class DataType {
		Int8,
		Int16,
		Int32,
		Int64,

		Uint8,
		Uint16,
		Uint32,
		Uint64,

		Fract8, // Same as Uint, but binary point is before MSB
		Fract16,
		Fract32,
		Fract64,

		Float16, // Reserved
		Float32,
		Float64,
		Float128, // Reserved
	};

	struct FieldDescriptor {
		DataType Type; // In current version, only Float32 and Float64 are supported.
		size_t Offset;
		std::string_view Name;
	};

	struct PixelDataDescriptor {
		size_t Size;
		size_t FieldCount;
		const FieldDescriptor *Fields;

		im_export const FieldDescriptor *FindField(std::string_view name) const;
	};
}