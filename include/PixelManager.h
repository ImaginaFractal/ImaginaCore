#pragma once

#include <stdint.h>
#include <vector>
#include "Types.h"

namespace Imagina {
	class IGpuTexture {
	public:
		virtual void SetImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY
		virtual void UpdateImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY

		im_export virtual ~IGpuTexture();
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
		virtual void SetLocation(HRLocation location) = 0;
		virtual void SetResolution(GRInt width, GRInt height) = 0;

		virtual void Update() = 0;
	};

	class IGpuTextureManager {
	public:
		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) = 0;
		virtual void DeactivateGpu() = 0;

		virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) = 0; // Return type may change
	};

	class IGpuPixelManager : public IPixelManager, public IGpuTextureManager {};

	class IRasterizingInterface {
	public:
		im_export virtual ~IRasterizingInterface();

		virtual bool GetCoordinate(HRReal &x, HRReal &y) = 0;
		virtual void WriteResults(SRReal Value) = 0;
	};

	class IRasterizer {
	public:
		virtual IRasterizingInterface &GetRasterizingInterface() = 0;
		im_export virtual void FreeRasterizingInterface(IRasterizingInterface &Interface);
	};
}