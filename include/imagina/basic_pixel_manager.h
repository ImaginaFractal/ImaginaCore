#pragma once

#include <atomic>

#include "declarations.h"
#include "types.h"
#include "pixel_management.h"
#include <imagina/interface/task.h>
#include <imagina/interface/engine.h>

namespace Imagina {
	class BasicRasterizingInterface;
	class BasicPixelManager {
		friend class BasicRasterizingInterface;
		GRInt width = 0, height = 0;
		size_t pixelCount;
		static_assert(sizeof(std::atomic_size_t) == sizeof(size_t));
		std::atomic_size_t i = 0;
		IGraphics graphics = nullptr;
		GpuTexture gpuTexture = nullptr;
		PixelPipeline::Stage gpuTextureUploadPoint = PixelPipeline::Stage::Colorized;
		IEngine engine = nullptr;

		HRLocation location;

		ITask task = nullptr;

		bool initialized = false, valid = false;

		size_t preprocessedDataSize = 0;
		size_t finalDataSize = 0;
		const FieldInfo *outputFieldInfo;

		char *preprocessedPixels = nullptr;
		uint32_t *finalPixels = nullptr;

		PixelPipeline *pixelPipeline = nullptr;
		const IPixelProcessor preprocessor = nullptr;
		const IPixelProcessor finalProcessor = nullptr;
		CopyProcessor copyProcessor; // used as preprocessor if no preprocessor is provided

		void Initialize();

	public:
		void ActivateGpu(IGraphics graphics);
		void DeactivateGpu(bool cleanup = true);

		void UsePixelPipeline(PixelPipeline *pipeline);
		void GetPixelData(void *data, PixelPipeline::Stage stage);
		void GetPixelData(void *data, PixelPipeline::Stage stage, const char *field);

		void SetTextureUploadPoint(PixelPipeline::Stage uploadPoint);

		void SetEngine(IEngine engine);
		void SetImmediateTarget(const HRLocation &location);
		void SetResolution(GRInt width, GRInt height);
		void InvalidatePixels();

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);
		void Update();

		std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location);

		void Cancel();
		void CancelAndWait();
		IRasterizingInterface GetRasterizingInterface();
		void FreeRasterizingInterface(IRasterizingInterface Interface);
	};

	class BasicRasterizingInterface {
		friend class BasicPixelManager;

		BasicPixelManager *pixelManager;
		PixelPipeline *pixelPipeline;
		BasicRasterizingInterface(BasicPixelManager *pixelManager) : pixelManager(pixelManager), pixelPipeline(pixelManager->pixelPipeline) {}

		int pixelX, pixelY;

	public:
		bool GetPixel(HRReal &x, HRReal &y);
		void GetDdx(HRReal &x, HRReal &y);
		void GetDdy(HRReal &x, HRReal &y);
		void WriteResults(void *value);
	};

	IMPLEMENT_INTERFACE2(BasicPixelManager, IGpuPixelManager, IRasterizer);
	IMPLEMENT_INTERFACE(BasicRasterizingInterface, IRasterizingInterface);
}