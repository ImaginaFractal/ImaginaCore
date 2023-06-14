#include "pixel_processing"
#include "evaluator"
#include <assert.h>
#include <string.h>

namespace Imagina {
	im_export const FieldDescriptor *PixelDataDescriptor::FindField(const char *name) const {
		for (size_t i = 0; i < FieldCount; i++) {
			//if (Fields[i].Name == name) {
			if (strcmp(Fields[i].Name, name) == 0) {
				return &Fields[i];
			}
		}

		return nullptr;
	}

	void SerialCompositeProcessor2::SetInput(const PixelDataDescriptor *descriptor) {
		processors[0]->SetInput(descriptor);
		const PixelDataDescriptor *intermediateData = processors[0]->GetOutputDescriptor();
		intermediateDataSize = intermediateData->Size;
		processors[1]->SetInput(intermediateData);
	}

	const PixelDataDescriptor *SerialCompositeProcessor2::GetOutputDescriptor() {
		return processors[1]->GetOutputDescriptor();
	}

	void SerialCompositeProcessor2::Process(void *output, void *input) const {
		void *intermediateData = alloca(intermediateDataSize);

		processors[0]->Process(intermediateData, input);
		processors[1]->Process(output, intermediateData);
	}

	SerialCompositeProcessor::SerialCompositeProcessor(std::initializer_list<IPixelProcessor *> processors) {
		assert(std::find(processors.begin(), processors.end(), nullptr) == processors.end());
		assert(processors.size() > 1);

		processorCount = processors.size();
		this->processors = new IPixelProcessor * [processorCount];
		std::copy(processors.begin(), processors.end(), this->processors);
		//memcpy(this->processors, processors.begin(), processorCount * sizeof(IPixelProcessor *));
	}

	void SerialCompositeProcessor::SetInput(const PixelDataDescriptor *descriptor) {
		const PixelDataDescriptor *intermediateData;

		processors[0]->SetInput(descriptor);
		intermediateData = processors[0]->GetOutputDescriptor();

		for (size_t i = 1; i < processorCount; i++) {
			intermediateDataSize = std::max(intermediateDataSize, intermediateData->Size);
			processors[i]->SetInput(descriptor);
			intermediateData = processors[i]->GetOutputDescriptor();
		}
	}

	const PixelDataDescriptor *SerialCompositeProcessor::GetOutputDescriptor() {
		return processors[processorCount - 1]->GetOutputDescriptor();
	}

	void SerialCompositeProcessor::Process(void *output, void *input) const {
		void *intermediateData[2] = { alloca(intermediateDataSize * 2) };
		intermediateData[1] = (char *)intermediateData[0] + intermediateDataSize;

		processors[0]->Process(intermediateData[0], input);

		size_t i;
		for (i = 1; i < processorCount - 1; i++) {
			processors[i]->Process(intermediateData[i & 1], intermediateData[~i & 1]);
		}
		processors[i]->Process(output, intermediateData[~i & 1]);
	}

	void CopyProcessor::SetInput(const PixelDataDescriptor *descriptor) {
		output = descriptor;
		dataSize = descriptor->Size;
	}

	const PixelDataDescriptor *CopyProcessor::GetOutputDescriptor() {
		return output;
	}

	void CopyProcessor::Process(void *output, void *input) const {
		memcpy(output, input, dataSize);
	}

	IPixelProcessor *PixelPipeline::GetCompositeProcessor(Stage first, Stage last) {
		assert(StageValid(first) && StageValid(last) && first <= last);

		uint8_t First = (uint8_t)first;
		uint8_t Last = (uint8_t)last;

		while (!stages[First] && First < (uint8_t)Stage::Colorize) First++;
		while (!stages[Last] && Last > (uint8_t)Stage::Preprocess) Last--;

		if (Last < First) {
			return nullptr;
		} else if (First == Last) { // One
			return stages[First];
		} else if (First + 1 == Last || !stages[(uint8_t)Stage::Postprocess]) { // Two
			IPixelProcessor *&compositeProcessor = composite2[First - (uint8_t)Stage::Preprocess];
			if (!compositeProcessor) compositeProcessor = new SerialCompositeProcessor2(stages[First], stages[Last]);

			return compositeProcessor;
		} else { // Three
			if (!composite3) composite3 = new SerialCompositeProcessor({ stages[1], stages[2], stages[3] });

			return composite3;
		}
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

	const PixelDataDescriptor *PixelPipeline::GetOutputOfStage(Stage stage) {
		assert(linked);
		assert(StageValid(stage));
		return outputs[(size_t)stage];
	}

	using namespace std;
	const PixelDataDescriptor TestProcessor::OutputDescriptor{
		4, 1, TestProcessor::OutputFields
	};

	const FieldDescriptor TestProcessor::OutputFields[1]{
		{ "Iterations", 0, PixelDataType::Float32 }
	};

	void TestProcessor::SetInput(const PixelDataDescriptor *descriptor) {
		const FieldDescriptor *sourceField = descriptor->FindField("Value");

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
		const FieldDescriptor *iterationsField = descriptor->FindField("Iterations");
		const FieldDescriptor *finalZField = descriptor->FindField("FinalZ");

		assert(iterationsField->Type == PixelDataType::Uint64);
		assert(finalZField->Type == PixelDataType::SRComplex);

		iterationsOffset = iterationsField->Offset;
		finalZOffset = finalZField->Offset;
	}
	const PixelDataDescriptor *TestProcessor2::GetOutputDescriptor() {
		using namespace std;
		static const FieldDescriptor OutputFields[1]{
			{ "Value", 0, PixelDataType::Float64 }
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