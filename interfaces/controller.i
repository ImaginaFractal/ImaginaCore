#include <Imagina/types>
#include <Imagina/declarations>
#include <Imagina/pixel_management>

namespace Imagina {
	interface IController {
		HRLocation GetRenderLocation();

		void SetPixelManager(IPixelManager pixelManager);
		void SetLocationManager(ILocationManager *locationManager);

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);

		void Update(SRReal deltaTime);
	};
}