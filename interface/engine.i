#include <Imagina/types>
#include <Imagina/computation>
#include <Imagina/pixel_processing>
#include <Imagina/declarations>
#include "evaluator"

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	interface IRasterizer;
	interface IController;

	/*struct StandardEvaluationParameters {
		ITUInt Iterations = 1024;
		HRReal DivergentEscapeRadius = 0x1.0p8;
		HRReal ConvergentEscapeRadius = 0x1.0p-16;
	};*/

	interface IEngine {
		const PixelDataInfo *GetOutputInfo();

		ExecutionContext *AddTask(const HRCircle &circle, IRasterizer rasterizer);

		void SetEvaluationParameters(const StandardEvaluationParameters &parameters); // TODO: Set parameter when adding task

		bool Ready();

		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif