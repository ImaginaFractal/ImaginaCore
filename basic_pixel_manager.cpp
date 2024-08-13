#include <Imagina/basic_pixel_manager>
#include <Imagina/evaluator>
#include <Imagina/platform_dependent>
#include <assert.h>

namespace Imagina {
	void BasicPixelManager::Initialize() {
		assert(pixelPipeline);
		assert(evaluator);
#ifdef _DEBUG
		if (gpuTextureUploadPoint != PixelPipeline::None) {
			const PixelDataInfo *finalData = pixelPipeline->GetOutputOfStage(gpuTextureUploadPoint);//postprocessor ? postprocessor->GetOutputInfo() : preprocessedData;
			//assert(finalData->Size == 4 && finalData->FieldCount == 1 && finalData->Fields[0].Offset == 0 && finalData->Fields[0].Type == PixelDataType::Float32);
		}
#endif

		if (preprocessedPixels) {
			delete[] preprocessedPixels;
			if ((char *)finalPixels == preprocessedPixels) finalPixels = nullptr;
			preprocessedPixels = nullptr;
		}
		if (finalPixels) {
			delete[] finalPixels;
			finalPixels = nullptr;
		}

		pixelCount = size_t(width) * size_t(height);
		preprocessedDataSize = pixelPipeline->GetOutputOfStage(PixelPipeline::Preprocess)->Size;
		const PixelDataInfo *finalOutput = pixelPipeline->GetOutputOfStage(gpuTextureUploadPoint);
		finalDataSize = finalOutput->Size;
		//outputFieldInfo = finalOutput->FindField("Value");
		outputFieldInfo = finalOutput->FindField("Color");

		preprocessor = pixelPipeline->GetPreprocessor();
		if (!preprocessor) {
			copyProcessor.SetInput(evaluator.GetOutputInfo());
			preprocessor = &copyProcessor;
		}

		preprocessedPixels = new char[pixelCount * preprocessedDataSize];

		if (gpuTextureUploadPoint >= PixelPipeline::Postprocess) {
			finalProcessor = pixelPipeline->GetCompositeProcessor(PixelPipeline::Postprocess, gpuTextureUploadPoint);
		} else {
			finalProcessor = nullptr;
		}

		finalPixels = new uint32_t[pixelCount];

		initialized = true;
	}

	void BasicPixelManager::ActivateGpu(IGraphics graphics) {
		this->graphics = graphics;

		gpuTexture = graphics.CreateTexture();
	}

	void BasicPixelManager::DeactivateGpu(bool cleanup) {
		if (cleanup && graphics) {
			graphics.DeleteTexture(gpuTexture);
		}

		graphics = nullptr;
		gpuTexture = nullptr;
	}

	void BasicPixelManager::UsePixelPipeline(PixelPipeline *pipeline) {
		pixelPipeline = pipeline;
		valid = false;
		initialized = false;
	}

	void BasicPixelManager::GetPixelData(void *data, PixelPipeline::Stage stage) {
		if (pixelPipeline->Equivalent(stage, PixelPipeline::Preprocessed)) {
			memcpy(data, preprocessedPixels, pixelCount * preprocessedDataSize);
		} else if (pixelPipeline->Equivalent(stage, gpuTextureUploadPoint)) {
			memcpy(data, finalPixels, pixelCount * sizeof(*finalPixels));
		} else {
			IPixelProcessor *processor = pixelPipeline->GetCompositeProcessor(PixelPipeline::Postprocess, stage);
			size_t inputSize = pixelPipeline->PreprocessedDataSize();
			size_t outputSize = processor->GetOutputInfo()->Size;

			for (size_t i = 0; i < pixelCount; i++) {
				void *input = &preprocessedPixels[i * inputSize];
				void *output = &((char *)data)[i * outputSize];
				processor->Process(output, input);
			}
		}
	}

	void BasicPixelManager::GetPixelData(void *data, PixelPipeline::Stage stage, const char *field) {
		assert(PixelPipeline::StageValid(stage));
		throw not_implemented();
	}

	void BasicPixelManager::SetTextureUploadPoint(PixelPipeline::Stage uploadPoint) {
		assert(PixelPipeline::StageValid(uploadPoint));
		gpuTextureUploadPoint = uploadPoint;
	}

	void BasicPixelManager::SetEvaluator(IEvaluator evaluator) {
		this->evaluator = evaluator;
		valid = false;
	}

	void BasicPixelManager::SetImmediateTarget(const HRLocation &location) {
		this->location = location;
		valid = false;
	}

	void BasicPixelManager::SetResolution(GRInt width, GRInt height) {
		this->width = width;
		this->height = height;
		valid = false;
		initialized = false;
	}

	void BasicPixelManager::InvalidatePixels() {
		valid = false;
	}

	void BasicPixelManager::UpdateRelativeCoordinates(HRReal differenceX, HRReal differenceY) {
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

			if (evaluator.Ready()) {
				i = 0;
				//evaluator->RunTaskForRectangle(location.ToRectangle((SRReal)width / height), this)->WaitAndRelease();
				executionContext = evaluator.RunEvaluation(location.ToRectangle((SRReal)width / height).Circumcircle(), *this);

				valid = true;
			}
		}
		if (gpuTexture && gpuTextureUploadPoint != PixelPipeline::None) {
			graphics.SetTextureImage(gpuTexture, width, height, finalPixels);
		}
	}

	std::vector<TextureMapping> BasicPixelManager::GetTextureMappings(const HRRectangle &location) {
		//if (!gpuTexture || !valid) return std::vector<TextureMapping>();
		if (!gpuTexture || gpuTextureUploadPoint == PixelPipeline::None) return std::vector<TextureMapping>();

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
	IRasterizingInterface BasicPixelManager::GetRasterizingInterface() {
		return *new BasicRasterizingInterface(this);
	}
	void BasicPixelManager::FreeRasterizingInterface(IRasterizingInterface Interface) {
		delete (BasicRasterizingInterface *)Interface;
	}

	bool BasicRasterizingInterface::GetPixel(HRReal &x, HRReal &y) {
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

	void BasicRasterizingInterface::GetDdx(HRReal &x, HRReal &y) {
		x = pixelManager->location.Height() / pixelManager->height;
		y = 0.0;
	}

	void BasicRasterizingInterface::GetDdy(HRReal &x, HRReal &y) {
		x = 0.0;
		y = pixelManager->location.Height() / pixelManager->height;
	}

	void BasicRasterizingInterface::WriteResults(void *value) {
		size_t pixelIndex = pixelX + pixelY * pixelManager->width;
		void *preprocessedOutput = &pixelManager->preprocessedPixels[pixelIndex * pixelPipeline->PreprocessedDataSize()];

		pixelManager->preprocessor->Process(preprocessedOutput, value);

		void *finalOutput;
		if (pixelManager->finalProcessor) {
			finalOutput = alloca(pixelManager->finalDataSize);

			pixelManager->finalProcessor->Process(finalOutput, preprocessedOutput);
		} else {
			finalOutput = preprocessedOutput;
		}
		//pixelManager->finalPixels[pixelIndex] = pixelManager->outputFieldInfo->GetScalar<float>(finalOutput);
		pixelManager->finalPixels[pixelIndex] = pixelManager->outputFieldInfo->GetRGBA8(finalOutput);
	}
}