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

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);
		void Update(SRReal deltaTime);
	};
}