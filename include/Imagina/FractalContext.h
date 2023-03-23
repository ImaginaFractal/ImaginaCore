#pragma once

#include "PixelManager.h"
#include "Evaluator.h"

namespace Imagina {
	class IFractalContext {
	public:
		virtual void SetTargetLocation(const HRLocation &location) = 0;

		virtual void UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) = 0;
		virtual void Update(SRReal deltaTime) = 0;
	};

	class im_export FractalContext : public IFractalContext {
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

		virtual void SetTargetLocation(const HRLocation &location) override;

		virtual void UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) override;
		virtual void Update(SRReal deltaTime) override;
	};
}