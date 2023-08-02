#include <Imagina/types>
#include <Imagina/computation>
#include <Imagina/pixel_processing>
#include <Imagina/evaluator.h>
#include "pixel_management"

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	struct StandardEvaluationParameters : _StandardEvaluationParameters {
		StandardEvaluationParameters() : _StandardEvaluationParameters{
			.Iterations = 1024,
			.DivergentEscapeRadius = 0x1.0p8,
			.ConvergentEscapeRadius = 0x1.0p-16,
		} {}
	};

	interface IEvaluator { // TODO: virtual void PrepareFor(const HRCircle &circle) = 0;
		const PixelDataInfo *GetOutputInfo();
		void SetEvaluationParameters(const StandardEvaluationParameters &parameters); // TEMPORARY

		bool Ready();
		ExecutionContext *RunEvaluation(const HRCircle &circle, IRasterizer rasterizer);
	};

	interface StandardEvaluator : IEvaluator {
		void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius); // TEMPORARY
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif