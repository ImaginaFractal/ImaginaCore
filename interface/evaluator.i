#include <imagina/numerics.h>
#include <imagina/pixel_data.h>

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	interface IRasterizer;
	interface IRasterizingInterface;

	struct StandardEvaluationParameters {
		uint_iter Iterations = 1024;
		real_hr DivergentEscapeRadius = 0x1.0p8;
		real_hr ConvergentEscapeRadius = 0x1.0p-16;
	};

	interface IEvaluator {
		const PixelDataInfo *GetOutputInfo();
		void Prepare(const real_hp &x, const real_hp &y, real_hr radius, const StandardEvaluationParameters &parameters);
		void Evaluate(IRasterizingInterface rasterizingInterface);
	};

	interface IComplexLocationSink {
		void SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius); // TEMPORARY
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif