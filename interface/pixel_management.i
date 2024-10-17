#include <stdint.h>
#include <vector>
#include <imagina/types.h>
#include <imagina/pixel_processing.h>

#ifdef __INTELLISENSE__
#define interface class
#endif

namespace Imagina {
	interface IEngine;

	struct _GpuTexture;
	using GpuTexture = _GpuTexture *;

	interface IGraphics {
		GpuTexture CreateTexture();
		void DeleteTexture(GpuTexture texture);

		void SetTextureImage(GpuTexture texture, size_t width, size_t height, uint32_t *pixels);
		void UpdateTextureImage(GpuTexture texture, size_t width, size_t height, uint32_t *pixels);
	};
	
	struct TextureMapping {
		GpuTexture Texture;
		GRRectangle TextureRectangle;
		HRRectangle FractalRectangle;
	};

	interface IGpuTextureManager {
		void ActivateGpu(IGraphics graphics);
		void DeactivateGpu(bool cleanup = true);

		void SetTextureUploadPoint(PixelPipeline::Stage uploadPoint);

		// FIXME: vector can't be safely passed between modules compiled by different compilers
		std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location);
	};

	interface IPixelManager { // TODO: Add init
		IGpuTextureManager GetGpuTextureManager();

		void SetEngine(IEngine evaluator);
		void UsePixelPipeline(PixelPipeline *pipeline);
		void GetPixelData(void *data, PixelPipeline::Stage stage); // TODO: Add invertY
		// Interface generator doesn't support overloading yet
		//void GetPixelData(void *data, PixelPipeline::Stage stage, const char *field); // TODO: Add invertY

		void SetImmediateTarget(const HRLocation &location);
		void SetResolution(int_gr width, int_gr height);
		void InvalidatePixels();

		void UpdateRelativeCoordinates(real_hr differenceX, real_hr differenceY);
		void Update();

		bool Finished(); // TODO: Consider changing it to Status()
		void Wait();
	};

	interface IRasterizer {
		bool GetPixel(real_hr &x, real_hr &y);
		void GetDdx(real_hr &x, real_hr &y);
		void GetDdy(real_hr &x, real_hr &y);
		void WriteResults(void *value);
	};

	interface IPixelReceiver {
		void Cancel();

		IRasterizer GetRasterizer();
		void FreeRasterizer(IRasterizer rasterizer);
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif