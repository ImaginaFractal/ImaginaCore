#include <imagina/location_manager.h>
#include <imagina/float_f64ei64.h>

namespace Imagina {
	void StandardLocationManager::SetController(IController controller) {
		this->controller = controller;
	}

	void StandardLocationManager::SetEvaluator(IComplexLocationSink evaluator) {
		this->evaluator = evaluator;
		evaluator.SetReferenceLocation(referenceX, referenceY, 2.0); // TEMPORARY
	}

	void StandardLocationManager::LocationChanged(const HRLocation &location) {
		real_hr distance = max(abs(location.X), abs(location.Y));
		uint_bc precision = max<int_exp>(0, -exponent_of(location.HalfHeight)) + 32;

		if (distance > 64.0 * location.HalfHeight || precision > referenceX.GetPrecision()) {
			referenceX.SetPrecision(precision + 16);
			referenceY.SetPrecision(precision + 16);

			referenceX += location.X;
			referenceY += location.Y;
			// FIXME: Calculate radius correctly
			evaluator.SetReferenceLocation(referenceX, referenceY, location.HalfHeight); // TEMPORARY
			if (controller) controller.UpdateRelativeCoordinates(-location.X, -location.Y);
		}
	}
}