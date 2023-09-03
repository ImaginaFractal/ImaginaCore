#include <Imagina/controller>
#include <Imagina/fractal_context>
#include <Imagina/location_manager>

namespace Imagina {
	void FractalContext::Link() {
		assert(Controller);
		assert(PixelManager);
		assert(Evaluator);
		//assert(LocationManager);
		LocationManager = Evaluator.GetLocationManager();

		Controller.SetPixelManager(PixelManager);
		Controller.SetLocationManager(LocationManager);
		PixelManager.SetEvaluator(Evaluator);
		LocationManager.SetController(Controller);
	}

	void FractalContext::Update(SRReal deltaTime) {
		Controller.Update(deltaTime);
		PixelManager.Update();
	}
}