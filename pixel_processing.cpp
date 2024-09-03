#include <imagina/pixel_processing.h>
#include <imagina/interface/engine.h>
#include <imagina/platform_dependent.h>
#include <assert.h>
#include <cstring>
#include <cmath>

namespace Imagina {
	const FieldInfo *PixelDataInfo::FindField(const char *name) const {
		for (size_t i = 0; i < FieldCount; i++) {
			//if (Fields[i].Name == name) {
			if (strcmp(Fields[i].Name, name) == 0) {
				return &Fields[i];
			}
		}

		return nullptr;
	}

	void SerialCompositeProcessor2::SetInput(const PixelDataInfo *info) {
		processors[0].SetInput(info);
		const PixelDataInfo *intermediateData = processors[0].GetOutputInfo();
		intermediateDataSize = intermediateData->Size;
		processors[1].SetInput(intermediateData);
	}

	const PixelDataInfo *SerialCompositeProcessor2::GetOutputInfo() {
		return processors[1].GetOutputInfo();
	}

	void SerialCompositeProcessor2::Process(void *output, void *input) const {
		void *intermediateData = alloca(intermediateDataSize);

		processors[0].Process(intermediateData, input);
		processors[1].Process(output, intermediateData);
	}

	SerialCompositeProcessor::SerialCompositeProcessor(std::initializer_list<IPixelProcessor> processors) {
		assert(std::find(processors.begin(), processors.end(), nullptr) == processors.end());
		assert(processors.size() > 1);

		processorCount = processors.size();
		this->processors = new IPixelProcessor[processorCount];
		std::copy(processors.begin(), processors.end(), this->processors);
		//memcpy(this->processors, processors.begin(), processorCount * sizeof(IPixelProcessor *));
	}

	void SerialCompositeProcessor::SetInput(const PixelDataInfo *info) {
		const PixelDataInfo *intermediateData;

		processors[0].SetInput(info);
		intermediateData = processors[0].GetOutputInfo();

		for (size_t i = 1; i < processorCount; i++) {
			intermediateDataSize = std::max(intermediateDataSize, intermediateData->Size);
			processors[i].SetInput(info);
			intermediateData = processors[i].GetOutputInfo();
		}
	}

	const PixelDataInfo *SerialCompositeProcessor::GetOutputInfo() {
		return processors[processorCount - 1].GetOutputInfo();
	}

	void SerialCompositeProcessor::Process(void *output, void *input) const {
		void *intermediateData[2] = { alloca(intermediateDataSize * 2) };
		intermediateData[1] = (char *)intermediateData[0] + intermediateDataSize;

		processors[0].Process(intermediateData[0], input);

		size_t i;
		for (i = 1; i < processorCount - 1; i++) {
			processors[i].Process(intermediateData[i & 1], intermediateData[~i & 1]);
		}
		processors[i].Process(output, intermediateData[~i & 1]);
	}

	void CopyProcessor::SetInput(const PixelDataInfo *info) {
		output = info;
		dataSize = info->Size;
	}

	const PixelDataInfo *CopyProcessor::GetOutputInfo() {
		return output;
	}

	void CopyProcessor::Process(void *output, void *input) const {
		memcpy(output, input, dataSize);
	}

	IPixelProcessor PixelPipeline::GetCompositeProcessor(Stage first, Stage last) {
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
			IPixelProcessor &compositeProcessor = composite2[First - (uint8_t)Stage::Preprocess];
			if (!compositeProcessor) compositeProcessor = new SerialCompositeProcessor2(stages[First], stages[Last]);

			return compositeProcessor;
		} else { // Three
			if (!composite3) composite3 = new SerialCompositeProcessor({ stages[1], stages[2], stages[3] });

			return composite3;
		}
	}

	void PixelPipeline::UseEngine(IEngine engine) {
		outputs[0] = engine.GetOutputInfo();
	}
	void PixelPipeline::UsePreprocessor(IPixelProcessor processor) {
		stages[1] = processor;
		linked = false;
	}
	void PixelPipeline::UsePostprocessor(IPixelProcessor processor) {
		stages[2] = processor;
		linked = false;
	}
	void PixelPipeline::UseColorizer(IPixelProcessor processor) {
		stages[3] = processor;
		linked = false;
	}

	void PixelPipeline::Link() {
		const PixelDataInfo *pixelData = outputs[0];

		if (stages[1]) {
			stages[1].SetInput(pixelData);
			pixelData = stages[1].GetOutputInfo();
		}
		outputs[1] = pixelData;
		if (stages[2]) {
			stages[2].SetInput(pixelData);
			pixelData = stages[2].GetOutputInfo();
		}
		outputs[2] = pixelData;
		if (stages[3]) {
			stages[3].SetInput(pixelData);
			pixelData = stages[3].GetOutputInfo();
		}
		outputs[3] = pixelData;
		linked = true;
	}

	const PixelDataInfo *PixelPipeline::GetOutputOfStage(Stage stage) {
		assert(linked);
		assert(StageValid(stage));
		return outputs[(size_t)stage];
	}

	using namespace std;
	const PixelDataInfo TestProcessor::OutputInfo{
		4, 1, TestProcessor::OutputFields
	};

	const FieldInfo TestProcessor::OutputFields[1]{
		{ "Value", 0, PixelDataType::Float32 }
	};

	void TestProcessor::SetInput(const PixelDataInfo *info) {
		const FieldInfo *sourceField = info->FindField("Value");

		assert(sourceField->Type == PixelDataType::Float64);

		sourceOffset = sourceField->Offset;
	}
	const PixelDataInfo *TestProcessor::GetOutputInfo() {
		return &OutputInfo;
	}
	void TestProcessor::Process(void *output, void *input) const {
		*(float *)output = (float)*(double *)((char *)input + sourceOffset);
	}
	void TestProcessor2::SetInput(const PixelDataInfo *info) {
		const FieldInfo *iterationsField = info->FindField("Iterations");
		const FieldInfo *finalZField = info->FindField("FinalZ");

		assert(iterationsField->Type == PixelDataType::UInt64);
		assert(finalZField->Type == PixelDataType::complex_sr);

		iterationsOffset = iterationsField->Offset;
		finalZOffset = finalZField->Offset;
	}
	const PixelDataInfo *TestProcessor2::GetOutputInfo() {
		using namespace std;
		static const FieldInfo OutputFields[1]{
			{ "Value", 0, PixelDataType::Float64 }
		};

		static const PixelDataInfo OutputInfo{
			8, 1, OutputFields
		};

		return &OutputInfo;
	}

	void TestProcessor2::Process(void *output, void *input) const {
		const complex_sr &finalZ = GetField<complex_sr>(input, finalZOffset);
		real_sr finalMagnitude = norm(finalZ);
		double Iterations = GetField<uint64_t>(input, iterationsOffset);

		if (finalMagnitude > 4096.0) {
			Iterations -= log2(log2(finalMagnitude)) - log2(log2(4096.0));
		}

		*(double *)output = Iterations;
	}

	PaletteLookup::PaletteLookup(const rgba32f *palette, size_t paletteSize) : paletteSize(paletteSize) {
		rgba32f *temp = new rgba32f[paletteSize];
		memcpy(temp, palette, paletteSize * sizeof(rgba32f));
		this->palette = temp;
	}

	PaletteLookup::~PaletteLookup() {
		delete[]palette;
	}

	void PaletteLookup::SetInput(const PixelDataInfo *info) {
		inputField = info->FindField("Value");

		assert(inputField);
		assert(inputField->IsScalar());
	}

	const PixelDataInfo *PaletteLookup::GetOutputInfo() {
		static const FieldInfo OutputFields[1]{
			{ "Color", 0, PixelDataType::RGBA32F }
		};

		static const PixelDataInfo OutputInfo{
			16, 1, OutputFields
		};

		return &OutputInfo;
	}

	void PaletteLookup::Process(void *output, void *input) const {
		real_sr value = inputField->GetScalar<real_sr>(input);

		if (!std::isfinite(value)) {
			*(rgba32f *)output = rgba32f(0.0, 0.0, 0.0, 1.0);
			return;
		}

		value *= valueMultiplier;
		value += valueOffset;

		value -= floor(value);
		value *= paletteSize;

		size_t index1 = floor(value);
		size_t index2 = index1 + 1;
		if (index2 == paletteSize) index2 = 0;

		value -= floor(value);

		*(rgba32f *)output = lerp(palette[index1], palette[index2], (real_gr)value);
		((rgba32f *)output)->x = std::sqrt(((rgba32f *)output)->x);
		((rgba32f *)output)->y = std::sqrt(((rgba32f *)output)->y);
		((rgba32f *)output)->z = std::sqrt(((rgba32f *)output)->z);
	}

	BSplineInterpolator::BSplineInterpolator(const rgba32f *palette, size_t paletteSize) : paletteSize(paletteSize) {
		rgba32f *temp = new rgba32f[paletteSize + 3];
		memcpy(temp + 1, palette, paletteSize * sizeof(rgba32f));
		temp[0] = temp[paletteSize];
		temp[paletteSize + 1] = temp[1];
		temp[paletteSize + 2] = temp[2];
		this->palette = temp;
	}

	BSplineInterpolator::~BSplineInterpolator() {
		delete[]palette;
	}

	void BSplineInterpolator::SetInput(const PixelDataInfo *info) {
		inputField = info->FindField("Value");

		assert(inputField);
		assert(inputField->IsScalar());
	}

	const PixelDataInfo *BSplineInterpolator::GetOutputInfo() {
		static const FieldInfo OutputFields[1]{
			{ "Color", 0, PixelDataType::RGBA32F }
		};

		static const PixelDataInfo OutputInfo{
			16, 1, OutputFields
		};

		return &OutputInfo;
	}
	
	void BSplineInterpolator::Process(void *output, void *input) const {
		real_sr value = inputField->GetScalar<real_sr>(input);
	
		if (!std::isfinite(value)) {
			*(rgba32f *)output = rgba32f(0.0, 0.0, 0.0, 1.0);
			return;
		}
	
		value *= valueMultiplier;
		value += valueOffset;
	
		value -= floor(value);
		value *= paletteSize;
	
		size_t index = floor(value);
		float t = value - floor(value);
		float t2 = t * t;
		float t3 = t2 * t;
	
		*(rgba32f *)output = palette[index + 0] * (       -t3 + 3.0f * t2 - 3.0f * t + 1.0f)
						+ palette[index + 1] * ( 3.0f * t3 - 6.0f * t2            + 4.0f)
						+ palette[index + 2] * (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f)
						+ palette[index + 3] * (        t3								);
		*(rgba32f *)output *= 1.0f / 6.0f;
	
		((rgba32f *)output)->x = std::sqrt(((rgba32f *)output)->x);
		((rgba32f *)output)->y = std::sqrt(((rgba32f *)output)->y);
		((rgba32f *)output)->z = std::sqrt(((rgba32f *)output)->z);
	}
}