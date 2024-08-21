#include <Imagina/engine>

namespace Imagina {
	const PixelDataInfo *StandardEngine::GetOutputInfo() {
		return evaluator.GetOutputInfo();
	}

	ExecutionContext *StandardEngine::AddTask(const HRCircle &circle, IRasterizer rasterizer) {
		return evaluator.RunEvaluation(circle, rasterizer);
	}

	void StandardEngine::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		evaluator.SetEvaluationParameters(parameters);
	}
	bool StandardEngine::Ready() {
		return evaluator.Ready();
	}
	void StandardEngine::SetController(IController controller) {
		locationManager.SetController(controller);
	}
	void StandardEngine::LocationChanged(const HRLocation &location) {
		locationManager.LocationChanged(location);
	}
}