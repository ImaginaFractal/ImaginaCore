#include <Imagina/numerics>
#include <Imagina/computation>
#include <Imagina/pixel_data>
#include "location_manager"

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	interface IRasterizer;

	struct StandardEvaluationParameters {
		ITUInt Iterations = 1024;
		HRReal DivergentEscapeRadius = 0x1.0p8;
		HRReal ConvergentEscapeRadius = 0x1.0p-16;
	};

	interface IEvaluator { // TODO: virtual void PrepareFor(const HRCircle &circle) = 0;
		ILocationManager GetLocationManager();
		const PixelDataInfo *GetOutputInfo();
		void SetEvaluationParameters(const StandardEvaluationParameters &parameters); // TEMPORARY

		bool Ready();
		ExecutionContext *RunEvaluation(const HRCircle &circle, IRasterizer rasterizer);
	};

	//interface StandardEvaluator : IEvaluator {
	//	void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius); // TEMPORARY
	//};

	interface IComplexLocationSink {
		void SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius); // TEMPORARY
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif