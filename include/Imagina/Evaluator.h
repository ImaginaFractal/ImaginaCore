#pragma once

#include "Types.h"
#include "Declarations.h"
#include "Computation.h" // TODO: Use Declarations.h instead

namespace Imagina {
	class IEvaluator {
	public:
		virtual void Evaluate(IRasterizer &rasterizer) = 0;
		virtual ExecutionContext *RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) = 0;
	};

	// TEMPORARY
	class im_export Evaluator : public IEvaluator {
		class EvaluationTask;

		HPReal referenceX = 0.0, referenceY = 0.0;
		ExecutionContext *currentExecutionContext = nullptr;

	public:
		void SetReferenceLocation(const HPReal &x, const HPReal &y);
		virtual void Evaluate(IRasterizer &rasterizer) override;
		virtual ExecutionContext *RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) override;
	};
}