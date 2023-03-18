#pragma once

#include "Types.h"
#include "Declarations.h"

namespace Imagina {
	class ILocationManager {
	public:
		virtual void SetEvaluator(IEvaluator *evaluator) = 0;
		virtual void SetFractalContext(IFractalContext *fractalContext) = 0; // TODO: Use callback function instead?
		virtual void LocationChanged(const HRLocation &location) = 0;
	};

	class im_export StandardLocationManager : public ILocationManager {
		IEvaluator *evaluator = nullptr;
		HPReal referenceX, referenceY;
		IFractalContext *fractalContext = nullptr;
		MultiPrecision &mp;

	public:
		StandardLocationManager(MultiPrecision &mp) : mp(mp), referenceX(mp, 0.0), referenceY(mp, 0.0) {}

		virtual void SetEvaluator(IEvaluator *evaluator) override;
		virtual void SetFractalContext(IFractalContext *fractalContext) override;
		virtual void LocationChanged(const HRLocation &location) override;
	};
}