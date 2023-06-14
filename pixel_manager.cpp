#include "pixel_manager"

namespace Imagina {
	void IRasterizer::Cancel() {}
	void IRasterizer::FreeRasterizingInterface(IRasterizingInterface &Interface) {
		delete &Interface;
	}
}