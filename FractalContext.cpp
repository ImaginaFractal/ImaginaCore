#include "Controller.h"
#include "FractalContext.h"
#include "LocationManager.h"

namespace Imagina {
	void FractalContext::UseController(IController *controller) {
		Controller = controller;
	}
	void FractalContext::UsePixelManager(IPixelManager *pixelManager) {
		PixelManager = pixelManager;

		//if (Evaluator) PixelManager->SetEvaluator(Evaluator);
	}
	void FractalContext::UsePixelManager(IGpuPixelManager *pixelManager) {
		//UsePixelManager(static_cast<IPixelManager *>(pixelManager));
		PixelManager = pixelManager;
		GpuTextureManager = pixelManager;
	}
	void FractalContext::UseEvaluator(IEvaluator *evaluator) {
		Evaluator = evaluator;

		//if (PixelManager) PixelManager->SetEvaluator(Evaluator);
		//if (LocationManager) LocationManager->SetEvaluator(Evaluator);
	}
	void FractalContext::UseLocationManager(ILocationManager *locationManager) {
		LocationManager = locationManager;
		//locationManager->OnReferenceChange = std::bind_front(&FractalContext::UpdateRelativeCoordinate, this);
		//
		//if (Evaluator) LocationManager->SetEvaluator(Evaluator);
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
	void FractalContext::SetTargetLocation(const HRLocation &location) {
		PixelManager->SetTargetLocation(location);
		LocationManager->LocationChanged(location);
	}

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