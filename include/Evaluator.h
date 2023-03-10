#pragma once

#include "Types.h"
#include "Declarations.h"

namespace Imagina {
	class IEvaluator {
	public:
		virtual void Evaluate(IRasterizer &rasterizer) = 0;
	};

	// TEMPORARY
	class im_export Evaluator : public IEvaluator {
	public:
		virtual void Evaluate(IRasterizer &rasterizer) override;
	};
}