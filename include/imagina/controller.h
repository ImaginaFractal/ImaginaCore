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
		SRReal animationDuration = 0.25;
		SRReal remainingZoomTime = 0.0;
		SRReal zoomVelocity = 0.0;
		SRReal zoomAcceleration = 0.0;
		TransitionFunction transitionFunction = Power5_3;

		SRReal UpdateZoomAnimation(SRReal deltaTime);

	public:
		HRLocation GetRenderLocation();

		void SetAnimationDuration(SRReal duration) { animationDuration = duration; }
		void SetTransitionFunction(TransitionFunction function) { transitionFunction = function; }

		void SetPixelManager(IPixelManager pixelManager);
		void SetEngine(IEngine engine);

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);

		void Update(SRReal deltaTime);

		void ZoomIn(SRReal centerX, SRReal centerY);
		void ZoomOut(SRReal centerX, SRReal centerY);
		void Move(SRReal x, SRReal y);
	};

	IMPLEMENT_INTERFACE(NavigationController, IController);
}