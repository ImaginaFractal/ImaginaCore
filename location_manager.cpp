#include "location_manager"
#include "fractal_context"
#include "floating_point"

namespace Imagina {
	void StandardLocationManager::SetEvaluator(IEvaluator *evaluator) {
		this->evaluator = evaluator;
	}

	void StandardLocationManager::LocationChanged(const HRLocation &location) {
		HRReal distance = max(abs(location.X), abs(location.Y));
		BCUInt precision = max<ExpInt>(0, -ExponentOf(location.HalfHeight)) + 32;

		if (distance > 64.0 * location.HalfHeight || precision > referenceX.GetPrecision()) {
			referenceX.SetPrecision(precision + 16);
			referenceY.SetPrecision(precision + 16);

			referenceX += location.X;
			referenceY += location.Y;
			// FIXME: Calculate radius correctly
			static_cast<StandardEvaluator *>(evaluator)->SetReferenceLocation(referenceX, referenceY, location.HalfHeight); // TEMPORARY
			if (OnReferenceChange) OnReferenceChange(-location.X, -location.Y);
		}
	}
}