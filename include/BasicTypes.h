#pragma once

#include "Complex.h"

namespace Imagina {
	using GRReal = float; // Real type for graphics
	using SRReal = double; // Real type for fractal with standard precision and standard range
	using HRReal = double; // Real type for fractal with standard precision and highest range
	using HPReal = double; // Real type for fractal with high precision

	using SRComplex = Complex<SRReal>;
	using HRComplex = Complex<HRReal>;
	using HPComplex = Complex<HPReal>;
}