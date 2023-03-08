#include "Includes.h"

#include "PixelManager.h"

__declspec(dllexport) void dummy() {

}

namespace Imagina {
	__declspec(dllexport) IGpuTexture::~IGpuTexture() {}
}