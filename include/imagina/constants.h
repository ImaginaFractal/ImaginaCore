#pragma once

#include <stdint.h>
#include <utility>

namespace Imagina::inline Numerics::Constants {
	//template<typename type, intmax_t value>
	//constexpr type Integer() noexcept { return type(value); }
	//
	//template<typename type> constexpr type Zero()	noexcept { return Integer<type, 0>(); }
	//template<typename type> constexpr type One()	noexcept { return Integer<type, 1>(); }
	//template<typename type> constexpr type Two()	noexcept { return Integer<type, 2>(); }

	template<intmax_t value>
	struct IntegerConstant {
		//template<typename type> constexpr explicit(!std::is_convertible_v<intmax_t, type>)
		//	operator type() const { return type(value); }
		constexpr operator double() const { return value; }
		constexpr operator intmax_t() const { return value; }
	};

	template<intmax_t value> constexpr IntegerConstant<value> Integer;

	constexpr IntegerConstant<0> Zero;
	constexpr IntegerConstant<1> One;
	constexpr IntegerConstant<2> Two;

	template<intmax_t value>
	struct IntegerRCPConstant {
		static_assert(value > 1 || value < -1);
		constexpr operator double() const { return 1.0 / value; }
	};

	constexpr IntegerRCPConstant<2> Half;
	constexpr IntegerRCPConstant<3> Third;
}