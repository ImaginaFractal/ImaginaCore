#include "LocationManager.h"
#include "FractalContext.h"
#include "FloatingPoint.h"

namespace Imagina {
	void StandardLocationManager::SetEvaluator(IEvaluator *evaluator) {
		this->evaluator = evaluator;
	}

	void StandardLocationManager::LocationChanged(const HRLocation &location) {
		HRReal distance = std::max(std::abs(location.X), std::abs(location.Y));
		BCInt precision = std::max<ExpInt>(0, -ExponentOf(location.HalfHeight)) + 32;

		if (distance > 64.0 * location.HalfHeight || precision > referenceX.GetPrecision()) {
			referenceX.SetPrecision(precision + 16);
			referenceY.SetPrecision(precision + 16);

			referenceX += location.X;
			referenceY += location.Y;
			static_cast<StandardEvaluator *>(evaluator)->SetReferenceLocation(referenceX, referenceY, location.HalfHeight); // TEMPORARY
			if (OnReferenceChange) OnReferenceChange(-location.X, -location.Y);
		}
	}
}