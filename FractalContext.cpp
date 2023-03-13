#include "FractalContext.h"
#include "LocationManager.h"

namespace Imagina {
	void FractalContext::SetTargetLocation(const HRLocation &location) {
		PixelManager->SetTargetLocation(location);
		LocationManager->LocationChanged(location);
	}

	void FractalContext::UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) {
		PixelManager->UpdateRelativeCoordinate(differenceX, differenceY);
	}

	void FractalContext::Update(SRReal deltaTime) {
		PixelManager->Update();
	}
}