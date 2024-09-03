#pragma once
#include "types.h"
#include "declarations.h"
#include <imagina/interface/pixel_management.h>
#include <imagina/interface/controller.h>
#include <imagina/interface/engine.h>

namespace Imagina {
	class im_export NavigationController {
	public:
		enum TransitionFunction {
			Linear,
			Power5_3,
			Quadratic,
			CubicHermite,
			QuinticHermite,
		};

	private:
		HRLocation targetLocation = HRLocation(0.0, 0.0, 2.0);
		HRLocation renderLocation = HRLocation(0.0, 0.0, 2.0);
		HRLocation immediateTarget = HRLocation(0.0, 0.0, 2.0);

		IPixelManager pixelManager = nullptr;
		IEngine engine = nullptr;

		bool zooming = false;
		real_sr animationDuration = 0.25;
		real_sr remainingZoomTime = 0.0;
		real_sr zoomVelocity = 0.0;
		real_sr zoomAcceleration = 0.0;
		TransitionFunction transitionFunction = Power5_3;

		real_sr UpdateZoomAnimation(real_sr deltaTime);

	public:
		HRLocation GetRenderLocation();

		void SetAnimationDuration(real_sr duration) { animationDuration = duration; }
		void SetTransitionFunction(TransitionFunction function) { transitionFunction = function; }

		void SetPixelManager(IPixelManager pixelManager);
		void SetEngine(IEngine engine);

		void UpdateRelativeCoordinates(real_hr differenceX, real_hr differenceY);

		void Update(real_sr deltaTime);

		void ZoomIn(real_sr centerX, real_sr centerY);
		void ZoomOut(real_sr centerX, real_sr centerY);
		void Move(real_sr x, real_sr y);
	};

	IMPLEMENT_INTERFACE(NavigationController, IController);
}