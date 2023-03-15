#include "PixelProcessing.h"
#include "Evaluator.h"
#include <assert.h> // TEMPORARY

namespace Imagina {
	im_export const FieldDescriptor *PixelDataDescriptor::FindField(std::string_view name) const {
		for (size_t i = 0; i < FieldCount; i++) {
			if (Fields[i].Name == name) {
				return &Fields[i];
			}
		}

		return nullptr;
	}
	void PixelPipeline::ConnectEvaluator(IEvaluator *evaluator) {
		evaluatorOutput = evaluator->GetOutputDescriptor();
	}
	void PixelPipeline::UsePreprocessor(IPixelProcessor *processor) {
		preprocessor = processor;
	}
	void PixelPipeline::UsePostprocessor(IPixelProcessor *processor) {
		postprocessor = processor;
	}

	void PixelPipeline::Connect() {
		const PixelDataDescriptor *pixelData = evaluatorOutput;

		if (preprocessor) {
			preprocessor->SetInput(pixelData);
			pixelData = preprocessor->GetOutputDescriptor();
		}
		if (postprocessor) {
			postprocessor->SetInput(pixelData);
		}
	}

	using namespace std;
	const PixelDataDescriptor TestProcessor::OutputDescriptor{
		4, 1, TestProcessor::OutputFields
	};

	const FieldDescriptor TestProcessor::OutputFields[1]{
		{ DataType::Float32, 0, "Iterations"sv }
	};

	void TestProcessor::SetInput(const PixelDataDescriptor *descriptor) {
		const FieldDescriptor *sourceField = descriptor->FindField("Iterations"sv);

		assert(sourceField->Type == DataType::Float64);

		sourceOffset = sourceField->Offset;
	}
	const PixelDataDescriptor *TestProcessor::GetOutputDescriptor() {
		return &OutputDescriptor;
	}
	void TestProcessor::Process(void *output, void *input) const {
		*(float *)output = (float)*(double *)((char *)input + sourceOffset);
	}
}