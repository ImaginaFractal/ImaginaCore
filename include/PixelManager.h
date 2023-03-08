#pragma once

#include <stdint.h>
#include <vector>
#include "Types.h"

namespace Imagina {
	class IGpuTexture {
	public:
		virtual void SetImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY
		virtual void UpdateImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY

		im_decl virtual ~IGpuTexture();
	};

	class IGpuTextureCreater {
	public:
		virtual IGpuTexture *CreateTexture() = 0;
		virtual void DeleteTexture(IGpuTexture *texture) = 0;
	};

	struct TextureMapping {
		IGpuTexture *Texture;
		GRRectangle TextureRectangle;
		HRRectangle FractalRectangle;
	};

	class IPixelManager {

	};

	class IGpuTextureManager {
	public:
		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) = 0;
		virtual void DeactivateGpu() = 0;

		virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) = 0; // Return type may change
	};

	class IGpuPixelManager : public IPixelManager, public IGpuTextureManager {};

	class IRasterizer {

	};
}