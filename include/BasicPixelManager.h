#pragma once

#include "PixelManager.h"

namespace Imagina {
	class BasicPixelManager : public IGpuPixelManager {
		IGpuTextureCreater *gpuTextureCreater = nullptr;
		IGpuTexture *gpuTexture = nullptr;

	public:
		im_decl virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) override;
		im_decl virtual void DeactivateGpu() override;

		im_decl virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) override;
	};
}