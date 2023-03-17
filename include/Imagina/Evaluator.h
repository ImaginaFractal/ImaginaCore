#pragma once

#include "Types.h"
#include "Declarations.h"
#include "Computation.h" // TODO: Use Declarations.h instead
#include "PixelProcessing.h"

namespace Imagina {
	class IEvaluator {
	public:
		virtual const PixelDataDescriptor *GetOutputDescriptor() = 0;

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
		virtual ExecutionContext *RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) override final;
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y) override final;

		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) = 0;
	};

	// TEMPORARY
	class im_export TestEvaluator : public LowPrecisionEvaluator {
		struct Output {
			uint64_t Iterations;
			SRComplex FinalZ;
		};

	public:
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;

		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) override;
	};
}