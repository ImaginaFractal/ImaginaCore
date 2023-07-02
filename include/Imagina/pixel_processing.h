#pragma once

#include <stdint.h>
#include <stddef.h>
#include "declarations.h"

#ifdef __cplusplus
namespace Imagina {
#endif
	
enum _PixelDataType {
	IM_PDT_Invalid = 0x0,

	// Integer
	IM_PDT_Int8 = 0x10000,
	IM_PDT_Int16,
	IM_PDT_Int32,
	IM_PDT_Int64,

	IM_PDT_UInt8 = 0x10010,
	IM_PDT_UInt16,
	IM_PDT_UInt32,
	IM_PDT_UInt64,

	// Fixed point
	IM_PDT_Fract8 = 0x20000, // Same as UInt, but binary point is before MSB
	IM_PDT_Fract16,
	IM_PDT_Fract32,
	IM_PDT_Fract64,

	// Floating point
	IM_PDT_Float16 = 0x30000, // Reserved
	IM_PDT_Float32,
	IM_PDT_Float64,
	IM_PDT_FloatHR,

	// Complex
	IM_PDT_SRComplex = 0x40000,
	IM_PDT_HRComplex,

	// Color
	IM_PDT_Monochrome8 = 0x80000,
	IM_PDT_Monochrome16,

	IM_PDT_RGB8 = 0x80010,
	IM_PDT_RGB16,

	IM_PDT_RGB16F = 0x80018,
	IM_PDT_RGB32F,

	IM_PDT_RGBA8 = 0x80020,
	IM_PDT_RGBA16,

	IM_PDT_RGBA16F = 0x80028,
	IM_PDT_RGBA32F,


	IM_PDT_SRReal = IM_PDT_Float64,
	IM_PDT_HRReal = IM_PDT_FloatHR,
};

#ifndef __cplusplus
typedef uint32_t PixelDataType;
#else
enum class PixelDataType : uint32_t;
#endif

typedef struct _FieldInfo {
	const char *Name;
	ptrdiff_t Offset;
	PixelDataType Type;
}FieldInfo;

struct _PixelDataInfo {
	size_t Size;
	size_t FieldCount;
	const FieldInfo *Fields;
};

IM_C_STRUCT_ALIAS(PixelDataInfo);


#ifdef __cplusplus
}
#endif