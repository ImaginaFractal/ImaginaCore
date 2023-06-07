#pragma once

#include "Types.h"
#include "Declarations.h"
#include "Computation.h" // TODO: Use Declarations.h instead
#include "PixelProcessing.h"

namespace Imagina {
	class im_export IEvaluator {
	public:
		virtual ~IEvaluator() = default;

		virtual const PixelDataDescriptor *GetOutputDescriptor() = 0;

		virtual bool Ready();
		//virtual void Evaluate(IRasterizer &rasterizer) = 0;
		virtual ExecutionContext *RunEvaluation(const HRCircle &circle, IRasterizer *rasterizer) = 0;
	};

	struct StandardEvaluationParameters {
		ITUint Iterations = 1024;
		HRReal DivergentEscapeRadius = 0x1.0p8;
		HRReal ConvergentEscapeRadius = 0x1.0p-16;
	};

	class StandardEvaluator : public IEvaluator {
	public:
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) = 0; // TEMPORARY
		virtual void SetEvaluationParameters(const StandardEvaluationParameters &parameters) = 0; // TEMPORARY
	};

	class im_export SimpleEvaluator : public StandardEvaluator {
		class EvaluationTask;

		ExecutionContext *precomputeExecutionContext = nullptr;
		ExecutionContext *pixelExecutionContext = nullptr;

		void CancelTasks();

	protected:
		HPReal x, y;
		HRReal radius;
		StandardEvaluationParameters parameters;

	public:
		virtual bool Ready() override final;
		virtual ExecutionContext *RunEvaluation(const HRCircle &circle, IRasterizer *rasterizer) override final;
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) override final;
		virtual void SetEvaluationParameters(const StandardEvaluationParameters &parameters) override final;

		virtual void Precompute() = 0;
		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) = 0;
	};

	class im_export LowPrecisionEvaluator : public StandardEvaluator {
		class LPRasterizingInterface;
		class EvaluationTask;

		SRReal referenceX = 0.0, referenceY = 0.0;
		ExecutionContext *currentExecutionContext = nullptr;

	protected:
		StandardEvaluationParameters parameters;

	public:
		virtual ExecutionContext *RunEvaluation(const HRCircle &circle, IRasterizer *rasterizer) override final;
		virtual void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) override final;
		virtual void SetEvaluationParameters(const StandardEvaluationParameters &parameters) override final;

		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) = 0;
	};

	class im_export TestSimpleEvaluator : public SimpleEvaluator {
		struct Output {
			double Value;
		};

		uint64_t referenceLength;
		//SRComplex reference[1025];
		SRComplex *reference = nullptr;
		SRComplex referenceC;

	public:
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;

		virtual void Precompute() override;
		virtual void Evaluate(IRasterizingInterface &rasterizingInterface) override;
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