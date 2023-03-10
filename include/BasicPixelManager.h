#pragma once

#include "Declarations.h"
#include "PixelManager.h"

namespace Imagina {
	class BasicRasterizingInterface;
	class im_export BasicPixelManager : public IGpuPixelManager, public IRasterizer {
		friend class BasicRasterizingInterface;
		GRInt width = 0, height = 0;
		size_t pixelCount, i = 0;
		IGpuTextureCreater *gpuTextureCreater = nullptr;
		IGpuTexture *gpuTexture = nullptr;
		IEvaluator *evaluator = nullptr;

		HRLocation location;

		bool initialized = false, valid = false;

		float *pixels = nullptr;

	public:
		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) override;
		virtual void DeactivateGpu() override;

		virtual void SetEvaluator(IEvaluator *evaluator) override;
		virtual void SetLocation(HRLocation location) override;
		virtual void SetResolution(GRInt width, GRInt height) override;

		virtual void Update() override;

		virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) override;

		virtual IRasterizingInterface &GetRasterizingInterface() override;
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