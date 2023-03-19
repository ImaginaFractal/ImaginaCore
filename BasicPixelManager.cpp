#include "BasicPixelManager.h"
#include "Evaluator.h"
#include <assert.h> // TEMPORARY

namespace Imagina {
	void BasicPixelManager::Initialize() {
		assert(pixelPipeline);
		assert(evaluator);

		IPixelProcessor *preprocessor = pixelPipeline->GetPreprocessor();
		IPixelProcessor *postprocessor = pixelPipeline->GetPostprocessor();

		const PixelDataDescriptor *preprocessedData = preprocessor ? preprocessor->GetOutputDescriptor() : evaluator->GetOutputDescriptor();
		const PixelDataDescriptor *finalData = postprocessor ? postprocessor->GetOutputDescriptor() : preprocessedData;

		assert(finalData->Size == 4 && finalData->FieldCount == 1 && finalData->Fields[0].Offset == 0 && finalData->Fields[0].Type == PixelDataType::Float32);

		preprocessedDataSize = preprocessedData->Size;
		finalDataSize = finalData->Size;

		if (preprocessedPixels) {
			delete[] preprocessedPixels;
			if ((char *)pixels == preprocessedPixels) pixels = nullptr;
			preprocessedPixels = nullptr;
		}
		if (pixels) {
			delete[] pixels;
			pixels = nullptr;
		}
		pixelCount = size_t(width) * size_t(height);

		preprocessedPixels = new char[pixelCount * preprocessedDataSize];
		if (preprocessedData == finalData) {
			pixels = (float *)preprocessedPixels;
		} else {
			pixels = new float[pixelCount];
		}
		initialized = true;
	}

	void BasicPixelManager::ActivateGpu(IGpuTextureCreater *gpuTextureCreater) {
		this->gpuTextureCreater = gpuTextureCreater;

		gpuTexture = gpuTextureCreater->CreateTexture();
	}

	void BasicPixelManager::DeactivateGpu() {
		// TODO: Implement
	}

	void BasicPixelManager::UsePixelPipeline(PixelPipeline *pipeline) {
		pixelPipeline = pipeline;
		valid = false;
		initialized = false;
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
		CancelAndWait();

		location.X += differenceX;
		location.Y += differenceY;
	}

	void BasicPixelManager::Update() {
		if (!initialized) {
			Initialize();
		}
		if (!valid && evaluator) {
			//Evaluator evaluator;
			//evaluator->Evaluate(*this);
			if (executionContext) CancelAndWait();

			i = 0;
			//evaluator->RunTaskForRectangle(location.ToRectangle((SRReal)width / height), this)->WaitAndRelease();
			executionContext = evaluator->RunTaskForRectangle(location.ToRectangle((SRReal)width / height), this);

			valid = true;
		}
		if (gpuTexture) {
			gpuTexture->SetImage(width, height, pixels);
		}
	}

	std::vector<TextureMapping> BasicPixelManager::GetTextureMappings(const HRRectangle &location) {
		if (!gpuTexture || !valid) return std::vector<TextureMapping>();

		std::vector<TextureMapping> TextureMappings;
		TextureMappings.resize(1);

		TextureMappings[0].Texture = gpuTexture;
		TextureMappings[0].FractalRectangle = this->location.ToRectangle((SRReal)width / height);
		TextureMappings[0].TextureRectangle = GRRectangle{ 0.0f, 0.0f, 1.0f, 1.0f };

		return TextureMappings;
	}
	void BasicPixelManager::Cancel() {
		i = pixelCount;
		if (executionContext) {
			if (!executionContext->Terminated()) executionContext->Cancel();
		}
	}
	void BasicPixelManager::CancelAndWait() {
		i = pixelCount;
		if (executionContext) {
			if (!executionContext->Terminated()) executionContext->Cancel();
			executionContext->WaitAndRelease();
			executionContext = nullptr;
		}
	}
	IRasterizingInterface &BasicPixelManager::GetRasterizingInterface() {
		return *new BasicRasterizingInterface(this);
	}

	bool BasicRasterizingInterface::GetCoordinate(HRReal &x, HRReal &y) {
		size_t i = pixelManager->i++;
		if (i >= pixelManager->pixelCount) return false;

		pixelX = i % pixelManager->width;
		pixelY = i / pixelManager->width;

		SRReal xsr = (SRReal)(pixelX * 2 + 1 - pixelManager->width) / pixelManager->height; // Map to (-AspectRatio, AspectRatio)
		SRReal ysr = (SRReal)(pixelY * 2 + 1 - pixelManager->height) / pixelManager->height; // Map to (-1, 1)

		x = xsr * pixelManager->location.HalfHeight + pixelManager->location.X;
		y = ysr * pixelManager->location.HalfHeight + pixelManager->location.Y;

		//x = (double)(pixelX - 256) / 128;
		//y = (double)(pixelY - 128) / 128;

		

		return true;
	}

	void BasicRasterizingInterface::WriteResults(void *value) {
		const IPixelProcessor *preprocessor = pixelManager->pixelPipeline->GetPreprocessor();
		const IPixelProcessor *postprocessor = pixelManager->pixelPipeline->GetPostprocessor();

		size_t pixelIndex = pixelX + pixelY * pixelManager->width;
		void *preprocessedOutput = &pixelManager->preprocessedPixels[pixelIndex * pixelManager->preprocessedDataSize];

		if (preprocessor) {
			preprocessor->Process(preprocessedOutput, value);
		} else {
			memcpy(preprocessedOutput, value, pixelManager->preprocessedDataSize);
		}

		if (postprocessor) {
			postprocessor->Process(&pixelManager->pixels[pixelIndex], preprocessedOutput);
		}
	}
}