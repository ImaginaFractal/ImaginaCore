#pragma once

#include "numerics.h"
#include <imagina/interface/pixel_management.h>
#include <imagina/interface/controller.h>
#include <imagina/interface/engine.h>

namespace Imagina {
	class im_export FractalContext {
	public:
		IController Controller = nullptr;
		IPixelManager PixelManager = nullptr;
		IGpuTextureManager GpuTextureManager = nullptr;
		IEngine Engine = nullptr;

		void Link();

		//void SetImmediateTarget(const HRLocation &location);

		void UpdateRelativeCoordinates(real_hr differenceX, real_hr differenceY);
		void Update(real_sr deltaTime);
	};
}