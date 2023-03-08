#include "BasicPixelManager.h"

namespace Imagina {
	void BasicPixelManager::ActivateGpu(IGpuTextureCreater *gpuTextureCreater) {
		this->gpuTextureCreater = gpuTextureCreater;

		gpuTexture = gpuTextureCreater->CreateTexture();

		using complex = Imagina::SRComplex;//std::complex<double>;

		float Pixels[256 * 256];

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 256; x++) {
				double real = (double)(x - 128) / 128;
				double imag = (double)(y - 128) / 128;

				complex c = { real, imag };
				complex z = 0.0;

				long i;
				for (i = 0; i < 256; i++) {
					z = z * z + c;
					if (norm(z) > 4.0) break;
				}

				Pixels[x + y * 256] = i;
			}
		}

		gpuTexture->SetImage(256, 256, Pixels);
	}

	void BasicPixelManager::DeactivateGpu() {
		// TODO: Implement
	}

	std::vector<TextureMapping> BasicPixelManager::GetTextureMappings(const HRRectangle &location) {
		if (!gpuTexture) return std::vector<TextureMapping>();

		std::vector<TextureMapping> TextureMappings;
		TextureMappings.resize(1);

		TextureMappings[0].Texture = gpuTexture;

		return TextureMappings;
	}
}