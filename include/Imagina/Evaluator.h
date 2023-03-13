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
		HPReal referenceX = 0.0, referenceY = 0.0;

	public:
		void SetReferenceLocation(const HPReal &x, const HPReal &y);
		virtual void Evaluate(IRasterizer &rasterizer) override;
	};
}