#pragma once

#include <stdint.h>
#include <vector>
#include "Declarations.h"
#include "Types.h"

namespace Imagina {
	class IGpuTexture {
	public:
		virtual ~IGpuTexture() = default;

		virtual void SetImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY
		virtual void UpdateImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY
	};

	class IGpuTextureCreater {
	public:
		virtual ~IGpuTextureCreater() = default;

		virtual IGpuTexture *CreateTexture() = 0;
		virtual void DeleteTexture(IGpuTexture *texture) = 0;
	};

	struct TextureMapping {
		IGpuTexture *Texture;
		GRRectangle TextureRectangle;
		HRRectangle FractalRectangle;
	};

	class IPixelManager {
	public:
		virtual ~IPixelManager() = default;

		virtual void SetEvaluator(IEvaluator *evaluator) = 0;
		virtual void UsePixelPipeline(PixelPipeline *pipeline) = 0;

		virtual void SetTargetLocation(const HRLocation &location) = 0;
		virtual void SetResolution(GRInt width, GRInt height) = 0;

		virtual void UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) = 0;
		virtual void Update() = 0;
	};

	class IGpuTextureManager {
	public:
		virtual ~IGpuTextureManager() = default;

		virtual void ActivateGpu(IGpuTextureCreater *gpuTextureCreater) = 0;
		virtual void DeactivateGpu(bool cleanup = true) = 0;

		virtual std::vector<TextureMapping> GetTextureMappings(const HRRectangle &location) = 0; // Return type may change
	};

	class IGpuPixelManager : public IPixelManager, public IGpuTextureManager {};

	class IRasterizingInterface {
	public:
		virtual ~IRasterizingInterface() = default;

		virtual bool GetCoordinate(HRReal &x, HRReal &y) = 0;
		virtual void WriteResults(void *value) = 0;
	};

	class im_export IRasterizer {
	public:
		virtual ~IRasterizer() = default;

		virtual void Cancel();

		virtual IRasterizingInterface &GetRasterizingInterface() = 0;
		virtual void FreeRasterizingInterface(IRasterizingInterface &Interface);
	};
}