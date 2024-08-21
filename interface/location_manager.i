#include <Imagina/types>
#include <Imagina/declarations>

namespace Imagina {
	interface IController;

	interface ILocationManager {
		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
	};
}