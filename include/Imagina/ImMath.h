#pragma once

#include <cmath>
#include <algorithm>
#include "BasicTypes.h"
#include "Constants.h"

namespace Imagina {
	template<typename T> inline T ManhattanHypot(T x, T y) {
		using namespace std;
		return abs(x) + abs(y);
	}

	template<typename T> inline T ChebyshevHypot(T x, T y) {
		using namespace std;
		return max(abs(x), abs(y));
	}

}