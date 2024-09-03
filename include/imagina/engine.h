#pragma once

#include <imagina/interface/engine.h>
#include <imagina/interface/pixel_management.h>
#include <imagina/interface/controller.h>
#include "computation.h"
#include "location_manager.h"

namespace Imagina {
	class im_export StandardEngine {
		class EvaluationTask;

		ExecutionContext *precomputeExecutionContext = nullptr;
		ExecutionContext *pixelExecutionContext = nullptr;

		StandardLocationManager locationManager;

		void CancelTasks();

	protected:
		real_hp x, y;
		real_hr radius;
		StandardEvaluationParameters parameters;
		IEvaluator evaluator;

	public:
		StandardEngine(IEvaluator evaluator);
		~StandardEngine();

		const PixelDataInfo *GetOutputInfo();
		ITask AddTask(const HRCircle &circle, IRasterizer rasterizer);
		void SetEvaluationParameters(const StandardEvaluationParameters &parameters);

		bool Ready();

		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
		void SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius);
	};

	IMPLEMENT_INTERFACE(StandardEngine, IEngine);
}