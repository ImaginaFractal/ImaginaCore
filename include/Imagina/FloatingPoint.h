#pragma once

#include <bit>
#include "BasicTypes.h"

namespace Imagina {
	constexpr ExpInt ExponentOf(double x)	{ return ExpInt((std::bit_cast<uint64_t>(x) >> 52) & 0x7FF) - 0x3FF; }
	constexpr ExpInt ExponentOf(float x)	{ return ExpInt((std::bit_cast<uint32_t>(x) >> 23) & 0x0FF) - 0x07F; }
}