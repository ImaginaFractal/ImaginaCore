#include "imagina.h"
#include "pixel_manager"

using namespace Imagina;

namespace ImCApi {
	void *ImAlloc(size_t size) {
		return malloc(size);
	}

	void ImFree(void *p) {
		free(p);
	}

	bool RasterizingInterface_GetPixel(IRasterizingInterface *rasterizingInterface, HRReal *x, HRReal *y) {
		return ((Imagina::IRasterizingInterface *)rasterizingInterface)->GetPixel(*x, *y);
	}

	void RasterizingInterface_WriteResults(IRasterizingInterface *rasterizingInterface, void *value) {
		((Imagina::IRasterizingInterface *)rasterizingInterface)->WriteResults(value);
	}
}