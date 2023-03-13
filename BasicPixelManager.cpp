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

	void BasicPixelManager::SetEvaluator(IEvaluator *evaluator) {
		this->evaluator = evaluator;
		valid = false;
	}

	void BasicPixelManager::SetTargetLocation(const HRLocation &location) {
		this->location = location;
		valid = false;
	}

	void BasicPixelManager::SetResolution(GRInt width, GRInt height) {
		this->width = width;
		this->height = height;
		valid = false;
		initialized = false;
	}

	void BasicPixelManager::UpdateRelativeCoordinate(HRReal differenceX, HRReal differenceY) {
		// TODO: Cancel computation;

		location.X += differenceX;
		location.Y += differenceY;
	}

	void BasicPixelManager::Update() {
		if (!initialized) {
			if (pixels) {
				delete[] pixels;
				pixels = nullptr;
			}
			pixelCount = size_t(width) * size_t(height);
			pixels = new float[pixelCount];
			initialized = true;
		}
		if (!valid && evaluator) {
			i = 0;

			//Evaluator evaluator;
			//evaluator->Evaluate(*this);
			evaluator->RunTaskForRectangle(location.ToRectangle((SRReal)width / height), this)->WaitAndRelease();

			gpuTexture->SetImage(width, height, pixels);

			valid = true;
		}
	}

	std::vector<TextureMapping> BasicPixelManager::GetTextureMappings(const HRRectangle &location) {
		if (!gpuTexture || !valid) return std::vector<TextureMapping>();

		std::vector<TextureMapping> TextureMappings;
		TextureMappings.resize(1);

		TextureMappings[0].Texture = gpuTexture;

		return TextureMappings;
	}
	IRasterizingInterface &BasicPixelManager::GetRasterizingInterface() {
		return *new BasicRasterizingInterface(this);
	}

	bool BasicRasterizingInterface::GetCoordinate(HRReal &x, HRReal &y) {
		if (pixelManager->i >= pixelManager->pixelCount) return false;

		pixelX = pixelManager->i % pixelManager->width;
		pixelY = pixelManager->i / pixelManager->width;

		SRReal xsr = (SRReal)(pixelX * 2 + 1 - pixelManager->width) / pixelManager->height; // Map to (-AspectRatio, AspectRatio)
		SRReal ysr = (SRReal)(pixelY * 2 + 1 - pixelManager->height) / pixelManager->height; // Map to (-1, 1)

		x = xsr * pixelManager->location.HalfHeight + pixelManager->location.X;
		y = ysr * pixelManager->location.HalfHeight + pixelManager->location.Y;

		//x = (double)(pixelX - 256) / 128;
		//y = (double)(pixelY - 128) / 128;

		pixelManager->i++;

		return true;
	}

	void BasicRasterizingInterface::WriteResults(SRReal Value) {
		pixelManager->pixels[pixelX + pixelY * pixelManager->width] = Value;
	}
}