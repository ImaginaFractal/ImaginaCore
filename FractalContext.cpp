#include "Controller.h"
#include "FractalContext.h"
#include "LocationManager.h"

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
		LocationManager->OnReferenceChange = std::bind_front(&FractalContext::UpdateRelativeCoordinate, this);
	}
	//void FractalContext::SetTargetLocation(const HRLocation &location) {
	//	PixelManager->SetTargetLocation(location);
	//	LocationManager->LocationChanged(location);
	//}

	void FractalContext::UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) {
		Controller->UpdateRelativeCoordinate(differenceX, differenceY);
		PixelManager->UpdateRelativeCoordinate(differenceX, differenceY);
		if (OnReferenceChange) OnReferenceChange(differenceX, differenceY);
	}

	void FractalContext::Update(SRReal deltaTime) {
		Controller->Update(deltaTime);
		PixelManager->Update();
	}
}