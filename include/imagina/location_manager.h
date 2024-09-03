#pragma once

#include <imagina/interface/location_manager.h>
#include <imagina/interface/evaluator.h>
#include <imagina/interface/controller.h>

namespace Imagina {
	class im_export StandardLocationManager {
		IComplexLocationSink evaluator = nullptr;
		//multi_precision &mp;
		real_hp referenceX, referenceY;
		IController controller = nullptr;

	public:
		StandardLocationManager(multi_precision &mp) : referenceX(0.0, mp), referenceY(0.0, mp) {}

		void SetController(IController controller);
		void SetEvaluator(IComplexLocationSink evaluator);
		void LocationChanged(const HRLocation &location);
	};

	IMPLEMENT_INTERFACE(StandardLocationManager, ILocationManager);
}