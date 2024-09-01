#include <imagina/controller.h>

namespace Imagina {
	HRLocation NavigationController::GetRenderLocation() {
		return renderLocation;
	}
	void NavigationController::SetPixelManager(IPixelManager pixelManager) {
		this->pixelManager = pixelManager;
		pixelManager.SetImmediateTarget(immediateTarget);
	}
    void NavigationController::SetEngine(IEngine engine) {
		this->engine = engine;
    }
	void NavigationController::UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY) {
		targetLocation.X += differenceX;
		targetLocation.Y += differenceY;
		renderLocation.X += differenceX;
		renderLocation.Y += differenceY;
		immediateTarget.X += differenceX;
		immediateTarget.Y += differenceY;

		pixelManager.UpdateRelativeCoordinates(differenceX, differenceY);
	}

	SRReal NavigationController::UpdateZoomAnimation(SRReal deltaTime) {
		SRReal t = deltaTime / remainingZoomTime;
		SRReal ratio = SRReal(targetLocation.HalfHeight / renderLocation.HalfHeight);

		SRReal zoomFactor;

		switch (transitionFunction) {
			default:
			case Linear: {
				zoomFactor = pow(ratio, t);
				break;
			}
			case Power5_3: {
				t = 1.0 - t;
				t = 1.0 - pow(t, 5.0 / 3.0);
				zoomFactor = pow(ratio, t);
				break;
			}
			case Quadratic: {
				t = 1.0 - t;
				t = 1.0 - t * t;
				zoomFactor = pow(ratio, t);
				break;
			}
			case CubicHermite: {
				SRReal depthDifference = log2(ratio);
				SRReal t2 = t * t;
				SRReal t3 = t2 * t;
				SRReal x
					= (     t3 - 2 * t2 + t) * zoomVelocity * remainingZoomTime
					+ (-2 * t3 + 3 * t2    ) * depthDifference;

				zoomVelocity
					= ( 3 * t2 - 4 * t + 1) * zoomVelocity
					+ (-6 * t2 + 6 * t    ) * depthDifference / remainingZoomTime;

				zoomFactor = std::exp2(x);
				break;
			}
			case QuinticHermite: {
				SRReal depthDifference = log2(ratio);
				SRReal t2 = t * t;
				SRReal t3 = t2 * t;
				SRReal t4 = t2 * t2;
				SRReal t5 = t3 * t2;
				SRReal x
					= (t          -  6.0*t3 +   8.0*t4 -   3.0*t5) * zoomVelocity * remainingZoomTime
					+ (    0.5*t2 -  1.5*t3 +   1.5*t4 -   0.5*t5) * zoomAcceleration * remainingZoomTime * remainingZoomTime
					+ (           + 10.0*t3 -  15.0*t4 +   6.0*t5) * depthDifference;

				SRReal velocity
					= (1          - 18.0*t2 +  32.0*t3 -  15.0*t4) * zoomVelocity
					+ (        t  -  4.5*t2 +   6.0*t3 -   2.5*t4) * zoomAcceleration * remainingZoomTime
					+ (           + 30.0*t2 -  60.0*t3 +  30.0*t4) * depthDifference / remainingZoomTime;

				SRReal acceleration
					= (           - 36.0*t  +  96.0*t2 +  60.0*t3) * zoomVelocity / remainingZoomTime
					+ (         1 -  9.0*t  +  18.0*t2 -  10.0*t3) * zoomAcceleration
					+ (           + 60.0*t  - 180.0*t2 + 120.0*t3) * depthDifference / (remainingZoomTime * remainingZoomTime);

				zoomVelocity = velocity;
				zoomAcceleration = acceleration;

				zoomFactor = std::exp2(x);
				break;
			}
		}
		
		remainingZoomTime -= deltaTime;

		return zoomFactor;
	}

	void NavigationController::Update(SRReal deltaTime) {
		bool immediateTargetChanged = false;
		if (zooming) {
			SRReal zoomFactor = UpdateZoomAnimation(deltaTime);

			if (remainingZoomTime > 0.0) {
				HRReal ratio = renderLocation.HalfHeight / targetLocation.HalfHeight;
				HRReal a = (ratio * (1.0 - zoomFactor)) / (ratio - 1.0);
				renderLocation.HalfHeight *= zoomFactor;
				renderLocation.X += (targetLocation.X - renderLocation.X) * a;
				renderLocation.Y += (targetLocation.Y - renderLocation.Y) * a;

				if (targetLocation.HalfHeight < renderLocation.HalfHeight && immediateTarget.HalfHeight >= renderLocation.HalfHeight) {
					immediateTarget.HalfHeight *= 0.5;
					if (immediateTarget.HalfHeight <= targetLocation.HalfHeight) {
						immediateTarget = targetLocation;
					} else {
						HRReal r2 = renderLocation.HalfHeight / targetLocation.HalfHeight;
						HRReal fac2 = immediateTarget.HalfHeight / renderLocation.HalfHeight;
						HRReal a2 = (r2 * (1.0 - fac2)) / (r2 - 1.0);
						immediateTarget.X = renderLocation.X + (targetLocation.X - renderLocation.X) * a2;
						immediateTarget.Y = renderLocation.Y + (targetLocation.Y - renderLocation.Y) * a2;
					}
					immediateTargetChanged = true;
				} else if (targetLocation.HalfHeight > renderLocation.HalfHeight && immediateTarget.HalfHeight <= renderLocation.HalfHeight) {
					immediateTarget.HalfHeight *= 2.0;
					if (immediateTarget.HalfHeight >= targetLocation.HalfHeight) {
						immediateTarget = targetLocation;
					} else {
						HRReal r2 = renderLocation.HalfHeight / targetLocation.HalfHeight;
						HRReal fac2 = immediateTarget.HalfHeight / renderLocation.HalfHeight;
						HRReal a2 = (r2 * (1.0 - fac2)) / (r2 - 1.0);
						immediateTarget.X = renderLocation.X + (targetLocation.X - renderLocation.X) * a2;
						immediateTarget.Y = renderLocation.Y + (targetLocation.Y - renderLocation.Y) * a2;
					}
					immediateTargetChanged = true;
				}
			} else {
				renderLocation = targetLocation;
				remainingZoomTime = 0.0;
				zoomVelocity = 0.0;
				zoomAcceleration = 0.0;
				zooming = false;
				if (immediateTarget != targetLocation) {
					immediateTarget = targetLocation;
					immediateTargetChanged = true;
				}
			}
		} else {
			renderLocation = targetLocation;
			remainingZoomTime = 0.0;
			zoomVelocity = 0.0;
			zoomAcceleration = 0.0;
			zooming = false;
			if (immediateTarget != targetLocation) {
				immediateTarget = targetLocation;
				immediateTargetChanged = true;
			}
		}
		if (immediateTargetChanged) {
			if (pixelManager) pixelManager.SetImmediateTarget(immediateTarget);
			if (engine) {
				//if (immediateTarget.HalfHeight > ) TODO
				engine.LocationChanged(immediateTarget);
			}
		}
	}
	void NavigationController::ZoomIn(SRReal centerX, SRReal centerY) {
		//targetLocation = targetLocation.ZoomIn(centerX, centerY);
		targetLocation.HalfHeight *= 0.5;
		targetLocation.X = renderLocation.X + centerX * (renderLocation.HalfHeight - targetLocation.HalfHeight);
		targetLocation.Y = renderLocation.Y + centerY * (renderLocation.HalfHeight - targetLocation.HalfHeight);
		zooming = true;
		remainingZoomTime = animationDuration;
	}
	void NavigationController::ZoomOut(SRReal centerX, SRReal centerY) {
		//targetLocation = targetLocation.ZoomOut(centerX, centerY);
		targetLocation.HalfHeight *= 2.0;
		targetLocation.X = renderLocation.X + centerX * (renderLocation.HalfHeight - targetLocation.HalfHeight);
		targetLocation.Y = renderLocation.Y + centerY * (renderLocation.HalfHeight - targetLocation.HalfHeight);
		zooming = true;
		remainingZoomTime = animationDuration;
	}
	void NavigationController::Move(SRReal x, SRReal y) {
		if (zooming) return;
		targetLocation.X += x * targetLocation.HalfHeight;
		targetLocation.Y += y * targetLocation.HalfHeight;
	}
}