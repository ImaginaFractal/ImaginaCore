#pragma once

#include "Types.h"
#include "Declarations.h"
#include "Computation.h" // TODO: Use Declarations.h instead

namespace Imagina {
	class IEvaluator {
	public:
		//virtual void Evaluate(IRasterizer &rasterizer) = 0;
		virtual ExecutionContext *RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) = 0;
	};

	class StandardEvaluator : public IEvaluator {
	public:
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y) = 0;
	};

	class im_export LowPrecisionEvaluator : public StandardEvaluator {
		class LPRasterizingInterface;
		class EvaluationTask;

		SRReal referenceX = 0.0, referenceY = 0.0;
		ExecutionContext *currentExecutionContext = nullptr;

	public:
		virtual ExecutionContext *RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) override;
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y) override;

		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) = 0;
	};

	// TEMPORARY
	class im_export Evaluator : public LowPrecisionEvaluator {
	public:
		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) override;
	};
}