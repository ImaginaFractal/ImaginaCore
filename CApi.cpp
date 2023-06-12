#include "CApi.h"
#include "PixelManager.h"

namespace ImCApi {
	bool RasterizingInterface_GetCoordinate(IRasterizingInterface *rasterizingInterface, HRReal *x, HRReal *y) {
		return ((Imagina::IRasterizingInterface *)rasterizingInterface)->GetCoordinate(*x, *y);
	}

	void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value) {
		((Imagina::IRasterizingInterface *)rasterizingInterface)->WriteResults(value);
	}
}