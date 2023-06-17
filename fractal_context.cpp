#include "controller"
#include "fractal_context"
#include "location_manager"

namespace Imagina {
	void FractalContext::UseController(IController *controller) {
		Controller = controller;
	}
	void FractalContext::UsePixelManager(IPixelManager *pixelManager) {
		PixelManager = pixelManager;
	}
	void FractalContext::UsePixelManager(IGpuPixelManager *pixelManager) {
		PixelManager = pixelManager;
		GpuTextureManager = pixelManager;
	}
	void FractalContext::UseEvaluator(IEvaluator *evaluator) {
		Evaluator = evaluator;
	}
	void FractalContext::UseLocationManager(ILocationManager *locationManager) {
		LocationManager = locationManager;
	}
	void FractalContext::Link() {
		assert(Controller);
		assert(PixelManager);
		assert(Evaluator);
		assert(LocationManager);

		Controller->SetPixelManager(PixelManager);
		Controller->SetLocationManager(LocationManager);
		PixelManager->SetEvaluator(Evaluator);
		LocationManager->SetEvaluator(Evaluator);
		LocationManager->OnReferenceChange = std::bind_front(&FractalContext::UpdateRelativeCoordinates, this);
	}
	//void FractalContext::SetImmediateTarget(const HRLocation &location) {
	//	PixelManager->SetImmediateTarget(location);
	//	LocationManager->LocationChanged(location);
	//}

	void FractalContext::UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY) {
		Controller->UpdateRelativeCoordinates(differenceX, differenceY);
		PixelManager->UpdateRelativeCoordinates(differenceX, differenceY);
		if (OnReferenceChange) OnReferenceChange(differenceX, differenceY);
	}

	void FractalContext::Update(SRReal deltaTime) {
		Controller->Update(deltaTime);
		PixelManager->Update();
	}
}