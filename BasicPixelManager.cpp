#include "BasicPixelManager.h"
#include "Evaluator.h"

namespace Imagina {
	void BasicPixelManager::ActivateGpu(IGpuTextureCreater *gpuTextureCreater) {
		this->gpuTextureCreater = gpuTextureCreater;

		gpuTexture = gpuTextureCreater->CreateTexture();
	}

	void BasicPixelManager::DeactivateGpu() {
		// TODO: Implement
	}

	void BasicPixelManager::SetLocation(HRLocation location) {
		this->location = location;
		invalid = true;
	}

	void BasicPixelManager::SetResolution(GRInt width, GRInt height) {
		// TODO: Implement
	}

	void BasicPixelManager::Update() {
		if (invalid) {
			i = 0;

			Evaluator evaluator;
			evaluator.Evaluate(*this);

			gpuTexture->SetImage(512, 256, Pixels);

			invalid = false;
		}
	}

	std::vector<TextureMapping> BasicPixelManager::GetTextureMappings(const HRRectangle &location) {
		if (!gpuTexture || invalid) return std::vector<TextureMapping>();

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

		SRReal xsr = (SRReal)(pixelX * 2 + 1 - 512) / 256; // Map to (-AspectRatio, AspectRatio)
		SRReal ysr = (SRReal)(pixelY * 2 + 1 - 256) / 256; // Map to (-1, 1)

		x = xsr * pixelManager->location.HalfHeight + pixelManager->location.X;
		y = ysr * pixelManager->location.HalfHeight + pixelManager->location.Y;

		//x = (double)(pixelX - 256) / 128;
		//y = (double)(pixelY - 128) / 128;

		pixelManager->i++;

		return true;
	}

	void BasicRasterizingInterface::WriteResults(SRReal Value) {
		pixelManager->Pixels[pixelX + pixelY * 512] = Value;
	}
}