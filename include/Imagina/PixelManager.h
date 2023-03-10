#pragma once

#include <stdint.h>
#include <vector>
#include "Declarations.h"
#include "Types.h"

namespace Imagina {
	class im_export IGpuTexture {
	public:
		virtual void SetImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY
		virtual void UpdateImage(size_t width, size_t height, float *pixels) = 0; // TEMPORARY

		virtual ~IGpuTexture();
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
	public:
		virtual void SetEvaluator(IEvaluator *evaluator) = 0;

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

	class im_export IRasterizingInterface {
	public:
		virtual ~IRasterizingInterface();

		virtual bool GetCoordinate(HRReal &x, HRReal &y) = 0;
		virtual void WriteResults(SRReal Value) = 0;
	};

	class im_export IRasterizer {
	public:
		virtual IRasterizingInterface &GetRasterizingInterface() = 0;
		virtual void FreeRasterizingInterface(IRasterizingInterface &Interface);
	};
}