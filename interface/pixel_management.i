#include <stdint.h>
#include <vector>
#include <Imagina/types>
#include <Imagina/pixel_processing>

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

	interface IPixelManager {
		void SetEngine(IEngine evaluator);
		void UsePixelPipeline(PixelPipeline *pipeline);
		void GetPixelData(void *data, PixelPipeline::Stage stage); // TODO: Add invertY
		// Interface generator doesn't support overloading yet
		//void GetPixelData(void *data, PixelPipeline::Stage stage, const char *field); // TODO: Add invertY

		void SetImmediateTarget(const HRLocation &location);
		void SetResolution(GRInt width, GRInt height);
		void InvalidatePixels();

		void UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY);
		void Update();
	};

	interface IGpuTextureManager {
		void ActivateGpu(IGraphics graphics);
		void DeactivateGpu(bool cleanup = true);

		void SetTextureUploadPoint(PixelPipeline::Stage uploadPoint);

		std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location); // Return type may change
	};

	interface IGpuPixelManager : IPixelManager, IGpuTextureManager {};

	interface IRasterizingInterface {
		bool GetPixel(HRReal &x, HRReal &y);
		void GetDdx(HRReal &x, HRReal &y);
		void GetDdy(HRReal &x, HRReal &y);
		void WriteResults(void *value);
	};

	interface IRasterizer {
		void Cancel();

		IRasterizingInterface GetRasterizingInterface();
		void FreeRasterizingInterface(IRasterizingInterface Interface);
	};
}

#ifdef __INTELLISENSE__
#undef interface
#endif