#include <imagina/fractal_context.h>

namespace Imagina {
	void FractalContext::Link() {
		assert(Controller);
		assert(PixelManager);
		assert(Engine);

		Controller.SetPixelManager(PixelManager);
		Controller.SetEngine(Engine);
		PixelManager.SetEngine(Engine);
		Engine.SetController(Controller);
	}

	void FractalContext::Update(real_sr deltaTime) {
		Controller.Update(deltaTime);
		PixelManager.Update();
	}
}