#include "Evaluator.h"
#include "PixelManager.h"

namespace Imagina {
	void Evaluator::SetReferenceLocation(const HPReal &x, const HPReal &y) {
		referenceX = x;
		referenceY = y;
	}
	void Evaluator::Evaluate(IRasterizer &rasterizer) {
		IRasterizingInterface &ri = rasterizer.GetRasterizingInterface();
	
		HRReal x, y;
		while (ri.GetCoordinate(x, y)) {
			SRComplex c = { x + referenceX, y + referenceY };
			SRComplex z = 0.0;
	
			long i;
			for (i = 0; i < 256; i++) {
				z = z * z + c;
				if (norm(z) > 4.0) break;
			}
	
			ri.WriteResults(i);
		}
		rasterizer.FreeRasterizingInterface(ri);
	}
}