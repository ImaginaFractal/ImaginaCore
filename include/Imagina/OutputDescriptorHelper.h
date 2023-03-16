#pragma once

#include <stdint.h>
#include "Types.h"
#include "PixelProcessing.h"

//https://www.scs.stanford.edu/~dm/blog/va-opt.html
#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(Struct, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(Struct, __VA_ARGS__)))
#define FOR_EACH_HELPER(Struct, a1, ...)                         \
  IM_OUTPUT_FIELD(Struct, a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (Struct, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER


#define _NUM_ARGS2(X,X64,X63,X62,X61,X60,X59,X58,X57,X56,X55,X54,X53,X52,X51,X50,X49,X48,X47,X46,X45,X44,X43,X42,X41,X40,X39,X38,X37,X36,X35,X34,X33,X32,X31,X30,X29,X28,X27,X26,X25,X24,X23,X22,X21,X20,X19,X18,X17,X16,X15,X14,X13,X12,X11,X10,X9,X8,X7,X6,X5,X4,X3,X2,X1,N,...) N
#define NUM_ARGS(...) _NUM_ARGS2(0, __VA_ARGS__ ,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define IM_OUTPUT_FIELD(Struct, Field) { Imagina::OutputDescriptorHelper::DataTypeToEnumValue<decltype(Struct::Field)>, offsetof(Struct, Field), #Field##sv },

#define IM_GET_OUTPUT_DESCRIPTOR_IMPL(Struct, ...) {	\
	using namespace std;	\
	static const Imagina::FieldDescriptor Fields[NUM_ARGS(__VA_ARGS__)] {	\
		FOR_EACH(Struct, __VA_ARGS__)\
	};	\
	static const Imagina::PixelDataDescriptor OutputDescriptor {	\
		sizeof(Struct), NUM_ARGS(__VA_ARGS__), Fields		\
	};	\
	return &OutputDescriptor;\
}

namespace Imagina {
	namespace OutputDescriptorHelper {
		template<typename T> constexpr PixelDataType DataTypeToEnumValue = PixelDataType(0);

		template<> constexpr PixelDataType DataTypeToEnumValue<int8_t>  = PixelDataType::Int8;
		template<> constexpr PixelDataType DataTypeToEnumValue<int16_t> = PixelDataType::Int16;
		template<> constexpr PixelDataType DataTypeToEnumValue<int32_t> = PixelDataType::Int32;
		template<> constexpr PixelDataType DataTypeToEnumValue<int64_t> = PixelDataType::Int64;

		template<> constexpr PixelDataType DataTypeToEnumValue<uint8_t>  = PixelDataType::Uint8;
		template<> constexpr PixelDataType DataTypeToEnumValue<uint16_t> = PixelDataType::Uint16;
		template<> constexpr PixelDataType DataTypeToEnumValue<uint32_t> = PixelDataType::Uint32;
		template<> constexpr PixelDataType DataTypeToEnumValue<uint64_t> = PixelDataType::Uint64;

		template<> constexpr PixelDataType DataTypeToEnumValue<float>  = PixelDataType::Float32;
		template<> constexpr PixelDataType DataTypeToEnumValue<double> = PixelDataType::Float64;

		template<> constexpr PixelDataType DataTypeToEnumValue<SRComplex> = PixelDataType::SRComplex;
		//template<> constexpr PixelDataType DataTypeToEnumValue<HRComplex> = PixelDataType::HRComplex;
	}
}