#pragma once

#include "PixelManager.h"
#include "Evaluator.h"

namespace Imagina {
	class im_export FractalContext {
	public:
		IPixelManager *PixelManager = nullptr;
		IGpuTextureManager *GpuTextureManager = nullptr;
		IEvaluator *Evaluator = nullptr;
		ILocationManager *LocationManager = nullptr;

		std::function<void(HRReal, HRReal)> OnReferenceChange;

		void UsePixelManager(IPixelManager *pixelManager);
		void UsePixelManager(IGpuPixelManager *pixelManager);
		void UseEvaluator(IEvaluator *evaluator);
		void UseLocationManager(ILocationManager *locationManager);

		void Link();

		void SetTargetLocation(const HRLocation &location);

		void UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY);
		void Update(SRReal deltaTime);
	};
}