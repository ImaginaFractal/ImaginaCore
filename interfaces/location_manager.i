#include <Imagina/types>
#include <Imagina/declarations>

namespace Imagina {
	using pCoordinateUpdateCallback = void (*)(void *data, HRReal, HRReal);

	interface ILocationManager {
		void SetCoordinateUpdateCallback(pCoordinateUpdateCallback callback, void *data = nullptr);
		void SetEvaluator(IEvaluator *evaluator);
		void LocationChanged(const HRLocation &location);
	};
}