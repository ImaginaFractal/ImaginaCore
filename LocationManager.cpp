#include "LocationManager.h"
#include "FractalContext.h"
#include "FloatingPoint.h"

namespace Imagina {
	void StandardLocationManager::SetEvaluator(IEvaluator *evaluator) {
		this->evaluator = evaluator;
	}
	void StandardLocationManager::SetFractalContext(IFractalContext *fractalContext) {
		this->fractalContext = fractalContext;
	}

	void StandardLocationManager::LocationChanged(const HRLocation &location) {
		BCInt precision = std::max<ExpInt>(32, -ExponentOf(location.HalfHeight)) + 32;

		referenceX.SetPrecision(precision);
		referenceY.SetPrecision(precision);

		referenceX += location.X;
		referenceY += location.Y;
		static_cast<StandardEvaluator *>(evaluator)->SetReferenceLocation(referenceX, referenceY, location.HalfHeight); // TEMPORARY
		fractalContext->UpdateRelativeCoordinate(-location.X, -location.Y);
	}
}