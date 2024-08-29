#include <imagina/numerics.h>
#include <imagina/pixel_data.h>

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	interface IRasterizer;
	interface IRasterizingInterface;

	struct StandardEvaluationParameters {
		ITUInt Iterations = 1024;
		HRReal DivergentEscapeRadius = 0x1.0p8;
		HRReal ConvergentEscapeRadius = 0x1.0p-16;
	};

	interface IEvaluator {
		const PixelDataInfo *GetOutputInfo();
		void Prepare(const HPReal &x, const HPReal &y, HRReal radius, const StandardEvaluationParameters &parameters);
		void Evaluate(IRasterizingInterface rasterizingInterface);
	};

	interface IComplexLocationSink {
		void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius); // TEMPORARY
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif