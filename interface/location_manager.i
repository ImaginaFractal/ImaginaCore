#include <Imagina/types>

namespace Imagina {
	interface IController;

	interface ILocationManager {
		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
	};
}