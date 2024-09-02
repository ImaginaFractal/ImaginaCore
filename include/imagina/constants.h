#pragma once

#include <stdint.h>
#include <utility>

namespace Imagina::inline Numerics::Constants {
	template<intmax_t value>
	struct IntegerConstant {
		template<typename type>
		consteval explicit operator type() const requires std::integral<type> || std::floating_point<type> {
			return type(value);
		} // Unfortunately, if it can be implicitly coverted to multiple types, there will be overload ambiguity problem

		consteval operator intmax_t() const { return value; }
	};

	template<intmax_t value> constexpr IntegerConstant<value> Integer;

	constexpr IntegerConstant<0> Zero;
	constexpr IntegerConstant<1> One;
	constexpr IntegerConstant<2> Two;

	template<intmax_t value>
	struct IntegerRCPConstant {
		static_assert(value > 1 || value < -1);

		template<std::floating_point type>
		consteval explicit operator type() const {
			return type(1.0) / type(value);
		}

		consteval operator double() const { return 1.0 / value; }
	};

	constexpr IntegerRCPConstant<2> Half;
	constexpr IntegerRCPConstant<3> Third;

	template<typename T> constexpr bool is_power_of_two = false;
	template<intmax_t value> constexpr bool is_power_of_two<IntegerConstant<value> > = (value & (value - 1)) == 0 && value != 0;
	template<intmax_t value> constexpr bool is_power_of_two<IntegerRCPConstant<value> > = (value & (value - 1)) == 0 && value != 0;

	template<typename T>
	concept PowerOfTwo = is_power_of_two<T>;

	static_assert( PowerOfTwo<IntegerConstant<2>>);
	static_assert(!PowerOfTwo<IntegerConstant<3>>);
	static_assert( PowerOfTwo<IntegerConstant<4>>);
	static_assert(!PowerOfTwo<IntegerConstant<5>>);
	static_assert(!PowerOfTwo<IntegerConstant<6>>);
	
	static_assert( PowerOfTwo<IntegerRCPConstant<2>>);
	static_assert(!PowerOfTwo<IntegerRCPConstant<3>>);
	static_assert( PowerOfTwo<IntegerRCPConstant<4>>);
	static_assert(!PowerOfTwo<IntegerRCPConstant<5>>);
	static_assert(!PowerOfTwo<IntegerRCPConstant<6>>);
}