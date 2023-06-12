#pragma once

#include <stdint.h>

#ifndef im_export
#ifdef _WIN32
#ifdef IM_BUILD_CORE
#define im_export __declspec(dllexport)
#else
#define im_export __declspec(dllimport)
#endif
#else
#define im_export
#endif
#endif

#ifdef __cplusplus
extern "C" namespace ImCApi {
#else
#include <stdbool.h>
#endif

typedef float GRReal; // Real type for graphics
typedef double SRReal; // Real type for fractal with standard precision and standard range
typedef double HRReal; // Real type for fractal with standard precision and highest range

typedef int32_t GRInt;
typedef int64_t ITInt;
typedef uint64_t ITUint;

typedef int64_t BCInt; // Integer type for bit count
typedef uint64_t BCUint; // Integer type for bit count
typedef int64_t ExpInt;

struct StandardEvaluationParameters {
	ITUint Iterations;
	HRReal DivergentEscapeRadius;
	HRReal ConvergentEscapeRadius;
};

enum class PixelDataType : uint32_t {
	Invalid = 0x0,

	// Integer
	Int8 = 0x10000,
	Int16,
	Int32,
	Int64,

	Uint8 = 0x10010,
	Uint16,
	Uint32,
	Uint64,

	// Fixed point
	Fract8 = 0x20000, // Same as Uint, but binary point is before MSB
	Fract16,
	Fract32,
	Fract64,

	// Floating point
	Float16 = 0x30000, // Reserved
	Float32,
	Float64,
	FloatHR,

	// Complex
	SRComplex = 0x40000,
	HRComplex,

	// Color
	Monochrome8 = 0x80000,
	Monochrome16 = 0x80000,

	RGB8 = 0x80010,
	RGB16 = 0x80010,

	RGB16F = 0x80018,
	RGB32F = 0x80019,

	RGBA8 = 0x80020,
	RGBA16 = 0x80020,

	RGBA16F = 0x80028,
	RGBA32F = 0x80029,


	SRReal = Float64,
	HRReal = FloatHR,
};

struct FieldDescriptor {
	const char *Name;
	ptrdiff_t Offset;
	PixelDataType Type;
};

struct PixelDataDescriptor {
	size_t Size;
	size_t FieldCount;
	const FieldDescriptor *Fields;
};

typedef struct _IController IController;

class IPixelManager;
class IGpuTextureManager;
class IGpuPixelManager;
typedef struct _IRasterizingInterface IRasterizingInterface;

class IRasterizer;
typedef struct _IEvaluator IEvaluator;

class ILocationManager;

class IFractalContext;

im_export void *ImAlloc(size_t size);
im_export void ImFree(void *p);

im_export bool RasterizingInterface_GetCoordinate(IRasterizingInterface *rasterizingInterface, HRReal *x, HRReal *y);

im_export void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value);

typedef void (*pIEvaluator_Release)(IEvaluator *instance);
typedef const PixelDataDescriptor *(*pIEvaluator_GetOutputDescriptor)(IEvaluator *instance);
typedef void (*pStandardEvaluator_SetEvaluationParameters)(IEvaluator *instance, const StandardEvaluationParameters *parameters);
typedef void (*pSimpleEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);
typedef void (*pLowPrecisionEvaluator_Evaluate)(IEvaluator *instance, IRasterizingInterface *rasterizingInterface);

#ifdef __cplusplus
}
#endif