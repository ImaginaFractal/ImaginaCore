#include "FractalContext.h"
#include "LocationManager.h"

namespace Imagina {
	void FractalContext::UsePixelManager(IPixelManager *pixelManager) {
		PixelManager = pixelManager;

		if (Evaluator) PixelManager->SetEvaluator(Evaluator);
	}
	void FractalContext::UsePixelManager(IGpuPixelManager *pixelManager) {
		UsePixelManager(static_cast<IPixelManager *>(pixelManager));
		GpuTextureManager = pixelManager;
	}
	void FractalContext::UseEvaluator(IEvaluator *evaluator) {
		Evaluator = evaluator;

		if (PixelManager) PixelManager->SetEvaluator(Evaluator);
		if (LocationManager) LocationManager->SetEvaluator(Evaluator);
	}
	void FractalContext::UseLocationManager(ILocationManager *locationManager) {
		LocationManager = locationManager;
		LocationManager->SetFractalContext(this);

		if (Evaluator) LocationManager->SetEvaluator(Evaluator);
	}
	void FractalContext::SetTargetLocation(const HRLocation &location) {
		temporaryLocation = location;
		PixelManager->SetTargetLocation(location);
		LocationManager->LocationChanged(location);
	}

	void FractalContext::UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) {
		PixelManager->UpdateRelativeCoordinate(differenceX, differenceY);
		temporaryLocation.X += differenceX;
		temporaryLocation.Y += differenceY;
	}

	void FractalContext::Update(SRReal deltaTime) {
		PixelManager->Update();
	}
}