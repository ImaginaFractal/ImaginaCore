#include "LocationManager.h"
#include "FractalContext.h"

namespace Imagina {
	void StandardLocationManager::SetEvaluator(IEvaluator *evaluator) {
		this->evaluator = evaluator;
	}
	void StandardLocationManager::SetFractalContext(IFractalContext *fractalContext) {
		this->fractalContext = fractalContext;
	}

	void StandardLocationManager::LocationChanged(const HRLocation &location) {
		referenceX += location.X;
		referenceY += location.Y;
		static_cast<StandardEvaluator *>(evaluator)->SetReferenceLocation(referenceX, referenceY); // TEMPORARY
		fractalContext->UpdateRelativeCoordinate(-location.X, -location.Y);
	}
}