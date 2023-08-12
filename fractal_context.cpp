#include <Imagina/controller>
#include <Imagina/fractal_context>
#include <Imagina/location_manager>

namespace Imagina {
	void FractalContext::Link() {
		assert(Controller);
		assert(PixelManager);
		assert(Evaluator);
		assert(LocationManager);

		Controller.SetPixelManager(PixelManager);
		Controller.SetLocationManager(LocationManager);
		PixelManager.SetEvaluator(Evaluator);
		LocationManager.SetEvaluator(Evaluator);
		//LocationManager->OnReferenceChange = std::bind_front(&FractalContext::UpdateRelativeCoordinates, this);
		LocationManager.SetCoordinateUpdateCallback([](void *data, HRReal x, HRReal y) { ((FractalContext *)data)->UpdateRelativeCoordinates(x, y); }, this);
	}
	//void FractalContext::SetImmediateTarget(const HRLocation &location) {
	//	PixelManager->SetImmediateTarget(location);
	//	LocationManager->LocationChanged(location);
	//}

	void FractalContext::UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY) {
		Controller.UpdateRelativeCoordinates(differenceX, differenceY);
		PixelManager.UpdateRelativeCoordinates(differenceX, differenceY);
		if (OnReferenceChange) OnReferenceChange(differenceX, differenceY);
	}

	void FractalContext::Update(SRReal deltaTime) {
		Controller.Update(deltaTime);
		PixelManager.Update();
	}
}