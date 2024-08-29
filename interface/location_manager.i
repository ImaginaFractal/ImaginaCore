#include <imagina/types.h>

namespace Imagina {
	interface IController;

	interface ILocationManager {
		void SetController(IController controller);
		void LocationChanged(const HRLocation &location);
	};
}