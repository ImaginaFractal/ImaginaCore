#include <Imagina/types>
#include <Imagina/declarations>

namespace Imagina {
	interface IEvaluator;

	using pCoordinateUpdateCallback = void (*)(void *data, HRReal, HRReal);

	interface ILocationManager {
		void SetCoordinateUpdateCallback(pCoordinateUpdateCallback callback, void *data = nullptr);
		void LocationChanged(const HRLocation &location);
	};
}