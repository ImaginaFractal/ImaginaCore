#include <imagina/numerics.h>

namespace Imagina {
	interface IPixelManager;
	interface IEngine;

	interface IController {
		HRLocation GetRenderLocation();

		void SetPixelManager(IPixelManager pixelManager);
		void SetEngine(IEngine engine);

		void UpdateRelativeCoordinates(real_hr differenceX, real_hr differenceY);

		void Update(real_sr deltaTime);
	};
}