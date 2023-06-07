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
	void PixelPipeline::UseEvaluator(IEvaluator *evaluator) {
		outputs[0] = evaluator->GetOutputDescriptor();
	}
	void PixelPipeline::UsePreprocessor(IPixelProcessor *processor) {
		stages[1] = processor;
		linked = false;
	}
	void PixelPipeline::UsePostprocessor(IPixelProcessor *processor) {
		stages[2] = processor;
		linked = false;
	}
	void PixelPipeline::UseColorizer(IPixelProcessor *processor) {
		stages[3] = processor;
		linked = false;
	}

	void PixelPipeline::Link() {
		const PixelDataDescriptor *pixelData = outputs[0];

		if (stages[1]) {
			stages[1]->SetInput(pixelData);
			pixelData = stages[1]->GetOutputDescriptor();
		}
		outputs[1] = pixelData;
		if (stages[2]) {
			stages[2]->SetInput(pixelData);
			pixelData = stages[2]->GetOutputDescriptor();
		}
		outputs[2] = pixelData;
		if (stages[3]) {
			stages[3]->SetInput(pixelData);
			pixelData = stages[3]->GetOutputDescriptor();
		}
		outputs[3] = pixelData;
		linked = true;
	}

	const PixelDataDescriptor *PixelPipeline::GetDataAtStage(Stage stage) {
		assert(linked);
		return outputs[(size_t)stage];
	}

	using namespace std;
	const PixelDataDescriptor TestProcessor::OutputDescriptor{
		4, 1, TestProcessor::OutputFields
	};

	const FieldDescriptor TestProcessor::OutputFields[1]{
		{ "Iterations"sv, PixelDataType::Float32, 0 }
	};

	void TestProcessor::SetInput(const PixelDataDescriptor *descriptor) {
		const FieldDescriptor *sourceField = descriptor->FindField("Value"sv);

		assert(sourceField->Type == PixelDataType::Float64);

		sourceOffset = sourceField->Offset;
	}
	const PixelDataDescriptor *TestProcessor::GetOutputDescriptor() {
		return &OutputDescriptor;
	}
	void TestProcessor::Process(void *output, void *input) const {
		*(float *)output = (float)*(double *)((char *)input + sourceOffset);
	}
	void TestProcessor2::SetInput(const PixelDataDescriptor *descriptor) {
		const FieldDescriptor *iterationsField = descriptor->FindField("Iterations"sv);
		const FieldDescriptor *finalZField = descriptor->FindField("FinalZ"sv);

		assert(iterationsField->Type == PixelDataType::Uint64);
		assert(finalZField->Type == PixelDataType::SRComplex);

		iterationsOffset = iterationsField->Offset;
		finalZOffset = finalZField->Offset;
	}
	const PixelDataDescriptor *TestProcessor2::GetOutputDescriptor() {
		using namespace std;
		static const FieldDescriptor OutputFields[1]{
			{ "Value"sv, PixelDataType::Float64, 0 }
		};

		static const PixelDataDescriptor OutputDescriptor{
			8, 1, OutputFields
		};

		return &OutputDescriptor;
	}
	template<typename T> T &GetField(void *base, ptrdiff_t offset) {
		return *(T *)((char *)base + offset);
	}

	void TestProcessor2::Process(void *output, void *input) const {
		const SRComplex &finalZ = GetField<SRComplex>(input, finalZOffset);
		SRReal finalMagnitude = norm(finalZ);
		double Iterations = GetField<uint64_t>(input, iterationsOffset);

		if (finalMagnitude > 4096.0) {
			Iterations -= log2(log2(finalMagnitude)) - log2(log2(4096.0));
		}

		*(double *)output = Iterations;
	}
}