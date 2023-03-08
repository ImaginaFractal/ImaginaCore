#pragma once

#include "PixelManager.h"

namespace Imagina {
	class BasicRasterizingInterface;
	class BasicPixelManager : public IGpuPixelManager, public IRasterizer {
		friend class BasicRasterizingInterface;
		IGpuTextureCreater *gpuTextureCreater = nullptr;
		IGpuTexture *gpuTexture = nullptr;

		HRLocation location;

		bool invalid = true;

		size_t i = 0;
		float Pixels[512 * 256];

	public:
		im_export virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) override;
		im_export virtual void DeactivateGpu() override;

		im_export virtual void SetLocation(HRLocation location) override;
		im_export virtual void SetResolution(GRInt width, GRInt height) override;

		im_export virtual void Update() override;

		im_export virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) override;

		im_export virtual IRasterizingInterface &GetRasterizingInterface() override;
	};

	class BasicRasterizingInterface : public IRasterizingInterface {
		friend class BasicPixelManager;

		BasicPixelManager *pixelManager;
		BasicRasterizingInterface(BasicPixelManager *pixelManager) : pixelManager(pixelManager) {}

		int pixelX, pixelY;

	public:
		virtual bool GetCoordinate(HRReal &x, HRReal &y) override;
		virtual void WriteResults(SRReal Value) override;
	};
}