#pragma once

#include <atomic>

#include "Declarations.h"
#include "PixelManager.h"
#include "Computation.h" // TODO: Replace with Declarations.h

namespace Imagina {
	class BasicRasterizingInterface;
	class im_export BasicPixelManager : public IGpuPixelManager, public IRasterizer {
		friend class BasicRasterizingInterface;
		GRInt width = 0, height = 0;
		size_t pixelCount;
		std::atomic_size_t i = 0;
		IGpuTextureCreater *gpuTextureCreater = nullptr;
		IGpuTexture *gpuTexture = nullptr;
		IEvaluator *evaluator = nullptr;

		HRLocation location;

		ExecutionContext *executionContext = nullptr;

		bool initialized = false, valid = false;

		float *pixels = nullptr;

		PixelPipeline *pixelPipeline;

	public:
		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) override;
		virtual void DeactivateGpu() override;

		virtual void UsePixelPipeline(PixelPipeline *pipeline) override;

		virtual void SetEvaluator(IEvaluator *evaluator) override;
		virtual void SetTargetLocation(const HRLocation &location) override;
		virtual void SetResolution(GRInt width, GRInt height) override;

		virtual void UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) override;
		virtual void Update() override;

		virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) override;

		virtual void Cancel() override;
		void CancelAndWait();
		virtual IRasterizingInterface &GetRasterizingInterface() override;
	};

	class BasicRasterizingInterface : public IRasterizingInterface {
		friend class BasicPixelManager;

		BasicPixelManager *pixelManager;
		BasicRasterizingInterface(BasicPixelManager *pixelManager) : pixelManager(pixelManager) {}

		int pixelX, pixelY;

	public:
		virtual bool GetCoordinate(HRReal &x, HRReal &y) override;
		virtual void WriteResults(void *value) override;
	};
}