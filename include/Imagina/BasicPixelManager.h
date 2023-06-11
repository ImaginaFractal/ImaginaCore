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
		PixelPipeline::Stage gpuTextureUploadPoint = PixelPipeline::Stage::None;
		IEvaluator *evaluator = nullptr;

		HRLocation location;

		ExecutionContext *executionContext = nullptr;

		bool initialized = false, valid = false;

		size_t preprocessedDataSize = 0;
		size_t postprocessedDataSize = 0;
		size_t finalDataSize = 0;

		char *preprocessedPixels = nullptr;
		float *finalPixels = nullptr; // Equal to preprocessedPixels when no preprocessing is needed

		PixelPipeline *pixelPipeline;

		void Initialize();

	public:
		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) override;
		virtual void DeactivateGpu(bool cleanup = true) override;

		virtual void UsePixelPipeline(PixelPipeline *pipeline) override;
		virtual void GetPixelData(void *data, PixelPipeline::Stage stage) override;
		virtual void GetPixelData(void *data, PixelPipeline::Stage stage, std::string_view field) override;

		virtual void SetTextureUploadPoint(PixelPipeline::Stage uploadPoint) override;

		virtual void SetEvaluator(IEvaluator *evaluator) override;
		virtual void SetTargetLocation(const HRLocation &location) override;
		virtual void SetResolution(GRInt width, GRInt height) override;
		virtual void InvalidatePixels() override;

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
		PixelPipeline *pixelPipeline;
		BasicRasterizingInterface(BasicPixelManager *pixelManager) : pixelManager(pixelManager), pixelPipeline(pixelManager->pixelPipeline) {}

		int pixelX, pixelY;

	public:
		virtual bool GetCoordinate(HRReal &x, HRReal &y) override;
		virtual void WriteResults(void *value) override;
	};
}