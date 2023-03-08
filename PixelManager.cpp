#include "Includes.h"

#include "PixelManager.h"

namespace Imagina {
	IGpuTexture::~IGpuTexture() {}
	IRasterizingInterface::~IRasterizingInterface() {}
	void IRasterizer::FreeRasterizingInterface(IRasterizingInterface &Interface) {
		delete &Interface;
	}
}