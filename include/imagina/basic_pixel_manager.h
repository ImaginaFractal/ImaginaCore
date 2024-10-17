#pragma once

#include <atomic>

#include "declarations.h"
#include "types.h"
#include "pixel_management.h"
#include <imagina/interface/task.h>
#include <imagina/interface/engine.h>

namespace Imagina {
	class BasicRasterizer;
	class BasicPixelManager {
		friend class BasicRasterizer;
		int_gr width = 0, height = 0;
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

		bool Initialize();

	public:
		~BasicPixelManager();

		IGpuTextureManager GetGpuTextureManager();

		void ActivateGpu(IGraphics graphics);
		void DeactivateGpu(bool cleanup = true);

		void UsePixelPipeline(PixelPipeline *pipeline);
		void GetPixelData(void *data, PixelPipeline::Stage stage);
		void GetPixelData(void *data, PixelPipeline::Stage stage, const char *field);

		void SetTextureUploadPoint(PixelPipeline::Stage uploadPoint);

		void SetEngine(IEngine engine);
		void SetImmediateTarget(const HRLocation &location);
		void SetResolution(int_gr width, int_gr height);
		void InvalidatePixels();

		void UpdateRelativeCoordinates(real_hr differenceX, real_hr differenceY);
		void Update();

		std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location);

		bool Finished();
		void Wait();
		void Cancel();
		void CancelAndWait();
		IRasterizer GetRasterizer();
		void FreeRasterizer(IRasterizer rasterizer);
	};

	class BasicRasterizer {
		friend class BasicPixelManager;

		BasicPixelManager *pixelManager;
		PixelPipeline *pixelPipeline;
		BasicRasterizer(BasicPixelManager *pixelManager) : pixelManager(pixelManager), pixelPipeline(pixelManager->pixelPipeline) {}

		static constexpr size_t groupSize = 16;

		size_t i = 0, end = 0;
		int pixelX, pixelY;

	public:
		bool GetPixel(real_hr &x, real_hr &y);
		void GetDdx(real_hr &x, real_hr &y);
		void GetDdy(real_hr &x, real_hr &y);
		void WriteResults(void *value);
	};

	IMPLEMENT_INTERFACE3(BasicPixelManager, IPixelManager, IGpuTextureManager, IPixelReceiver);
	IMPLEMENT_INTERFACE(BasicRasterizer, IRasterizer);
}