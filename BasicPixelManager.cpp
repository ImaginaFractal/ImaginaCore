#include "BasicPixelManager.h"
#include "Evaluator.h"

namespace Imagina {
	void BasicPixelManager::ActivateGpu(IGpuTextureCreater *gpuTextureCreater) {
		this->gpuTextureCreater = gpuTextureCreater;

		gpuTexture = gpuTextureCreater->CreateTexture();

		/*using complex = Imagina::SRComplex;//std::complex<double>;

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 512; x++) {
				double real = (double)(x - 256) / 128;
				double imag = (double)(y - 128) / 128;

				complex c = { real, imag };
				complex z = 0.0;

				long i;
				for (i = 0; i < 256; i++) {
					z = z * z + c;
					if (norm(z) > 4.0) break;
				}

				Pixels[x + y * 512] = i;
			}
		}*/

		Evaluator evaluator;
		evaluator.Evaluate(*this);

		gpuTexture->SetImage(512, 256, Pixels);
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
	IRasterizingInterface &BasicPixelManager::GetRasterizingInterface() {
		return *new BasicRasterizingInterface(this);
	}

	bool BasicRasterizingInterface::GetCoordinate(HRReal &x, HRReal &y) {
		if (pixelManager->i >= 256 * 512) return false;

		pixelX = pixelManager->i % 512;
		pixelY = pixelManager->i / 512;

		x = (double)(pixelX - 256) / 128;
		y = (double)(pixelY - 128) / 128;

		pixelManager->i++;

		return true;
	}

	void BasicRasterizingInterface::WriteResults(SRReal Value) {
		pixelManager->Pixels[pixelX + pixelY * 512] = Value;
	}
}