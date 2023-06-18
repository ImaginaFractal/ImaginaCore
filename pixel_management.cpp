#include "pixel_management"

namespace Imagina {
	void IRasterizer::Cancel() {}
	void IRasterizer::FreeRasterizingInterface(IRasterizingInterface &Interface) {
		delete &Interface;
	}
}