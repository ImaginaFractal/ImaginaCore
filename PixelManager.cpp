#include "Includes.h"

#include "PixelManager.h"

namespace Imagina {
	void IRasterizer::Cancel() {}
	void IRasterizer::FreeRasterizingInterface(IRasterizingInterface &Interface) {
		delete &Interface;
	}
}