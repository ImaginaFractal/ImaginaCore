#pragma once

#include "PixelManager.h"
#include "Evaluator.h"

namespace Imagina {
	struct FractalContext {
		IPixelManager *PixelManager;
		IEvaluator *Evaluator;
		IGpuTextureManager *GpuTextureManager;
	};
}