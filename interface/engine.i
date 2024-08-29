#include <imagina/types.h>
#include <imagina/pixel_data.h>
#include <imagina/declarations.h>
#include "evaluator.h"
#include "task.h"

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

		ITask AddTask(const HRCircle &circle, IRasterizer rasterizer);

		void SetEvaluationParameters(const StandardEvaluationParameters &parameters); // TODO: Set parameter when adding task

		bool Ready();

		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif