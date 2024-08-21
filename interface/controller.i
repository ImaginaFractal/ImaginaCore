#include <Imagina/numerics>

namespace Imagina {
	interface IPixelManager;
	interface IEngine;

	interface IController {
		HRLocation GetRenderLocation();

		void SetPixelManager(IPixelManager pixelManager);
		void SetEngine(IEngine engine);

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);

		void Update(SRReal deltaTime);
	};
}