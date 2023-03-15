#pragma once

#include "Declarations.h"
#include <string_view>

namespace Imagina {
	enum class DataType {
		Int8,
		Int16,
		Int32,
		Int64,

		Uint8,
		Uint16,
		Uint32,
		Uint64,

		Fract8, // Same as Uint, but binary point is before MSB
		Fract16,
		Fract32,
		Fract64,

		Float16, // Reserved
		Float32,
		Float64,
		Float128, // Reserved

		Rgba8,
	};

	struct FieldDescriptor {
		DataType Type; // In current version, only Float32 and Float64 are supported.
		ptrdiff_t Offset;
		std::string_view Name;
	};

	struct im_export PixelDataDescriptor {
		size_t Size;
		size_t FieldCount;
		const FieldDescriptor *Fields;

		const FieldDescriptor *FindField(std::string_view name) const;
	};

	class IPixelProcessor {
	public:
		virtual void SetInput(const PixelDataDescriptor *descriptor) = 0;
		virtual const PixelDataDescriptor *GetOutputDescriptor() = 0;

		virtual void Process(void *input, void *output) const = 0;
	};

	class im_export PixelPipeline {
		const PixelDataDescriptor *evaluatorOutput = nullptr;
		IPixelProcessor *preprocessor = nullptr;
		IPixelProcessor *postprocessor = nullptr;

	public:

		void ConnectEvaluator(IEvaluator *evaluator);

		void UsePreprocessor(IPixelProcessor *processor);
		void UsePostprocessor(IPixelProcessor *processor);

		virtual void Preprocess(void *input, void *output) { preprocessor->Process(input, output); } // FIXME: Doesn't work if preprocessor == nullptr
		virtual void Postprocess(void *input, void *output) { postprocessor->Process(input, output); } // FIXME: Doesn't work if postprocessor == nullptr
	};

	class im_export TestProcessor : public IPixelProcessor { // Converts Float64 to Float32
		static const PixelDataDescriptor OutputDescriptor;
		static const FieldDescriptor OutputFields[1];

		ptrdiff_t sourceOffset;

	public:
		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *input, void *output) const override;
	};
}