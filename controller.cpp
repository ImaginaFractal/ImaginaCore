#include <Imagina/controller>
#include <Imagina/pixel_management>
#include <Imagina/location_manager>

namespace Imagina {
	HRLocation NavigationController::GetRenderLocation() {
		return renderLocation;
	}
	void NavigationController::SetPixelManager(IPixelManager pixelManager) {
		this->pixelManager = pixelManager;
		pixelManager.SetImmediateTarget(immediateTarget);
	}
    void NavigationController::SetLocationManager(ILocationManager locationManager) {
		this->locationManager = locationManager;
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
	void NavigationController::Update(SRReal deltaTime) {
		bool immediateTargetChanged = false;
		if (zooming) {
			SRReal x = deltaTime / remainingZoomTime;
			x = 1.0 - x;
			x = 1.0 - pow(x, 1.6666);
			remainingZoomTime -= deltaTime;

			if (remainingZoomTime > 0.0) {
				HRReal ratio = renderLocation.HalfHeight / targetLocation.HalfHeight;
				SRReal zoomFactor = pow((SRReal)(targetLocation.HalfHeight / renderLocation.HalfHeight), x);
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
				zooming = false;
				if (immediateTarget != targetLocation) {
					immediateTarget = targetLocation;
					immediateTargetChanged = true;
				}
			}
		} else {
			renderLocation = targetLocation;
			remainingZoomTime = 0.0;
			zooming = false;
			if (immediateTarget != targetLocation) {
				immediateTarget = targetLocation;
				immediateTargetChanged = true;
			}
		}
		if (immediateTargetChanged) {
			if (pixelManager) pixelManager.SetImmediateTarget(immediateTarget);
			if (locationManager) {
				//if (immediateTarget.HalfHeight > ) TODO
				locationManager.LocationChanged(immediateTarget);
			}
		}
	}
	void NavigationController::ZoomIn(SRReal centerX, SRReal centerY) {
		targetLocation = targetLocation.ZoomIn(centerX, centerY);
		zooming = true;
		remainingZoomTime = 0.25;
	}
	void NavigationController::ZoomOut(SRReal centerX, SRReal centerY) {
		targetLocation = targetLocation.ZoomOut(centerX, centerY);
		zooming = true;
		remainingZoomTime = 0.25;
	}
	void NavigationController::Move(SRReal x, SRReal y) {
		if (zooming) return;
		targetLocation.X += x * targetLocation.HalfHeight;
		targetLocation.Y += y * targetLocation.HalfHeight;
	}
}