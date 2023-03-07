#pragma once

#include <stdint.h>

namespace Imagina {
	namespace Constants {
		template<typename type, intmax_t value>
		constexpr type Integer() noexcept { return type(value); }

		template<typename type> constexpr type Zero()	noexcept { return Integer<type, 0>(); }
		template<typename type> constexpr type One()	noexcept { return Integer<type, 1>(); }
		template<typename type> constexpr type Two()	noexcept { return Integer<type, 2>(); }
	}
}