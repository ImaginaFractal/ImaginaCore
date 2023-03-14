#include "Includes.h"

#include "PixelManager.h"

namespace Imagina {
	IGpuTexture::~IGpuTexture() {}
	IRasterizingInterface::~IRasterizingInterface() {}
	void IRasterizer::Cancel() {}
	void IRasterizer::FreeRasterizingInterface(IRasterizingInterface &Interface) {
		delete &Interface;
	}
}