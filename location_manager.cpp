#include <Imagina/location_manager>
#include <Imagina/fractal_context>
#include <Imagina/floating_point>

namespace Imagina {
	void StandardLocationManager::SetController(IController controller) {
		this->controller = controller;
	}

	void StandardLocationManager::SetEvaluator(IComplexLocationSink evaluator) {
		this->evaluator = evaluator;
		evaluator.SetReferenceLocation(referenceX, referenceY, 2.0); // TEMPORARY
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
			evaluator.SetReferenceLocation(referenceX, referenceY, location.HalfHeight); // TEMPORARY
			if (controller) controller.UpdateRelativeCoordinates(-location.X, -location.Y);
		}
	}

	IMPLEMENT_INTERFACE(StandardLocationManager, ILocationManager);
}