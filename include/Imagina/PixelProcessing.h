#pragma once

#include "Declarations.h"
#include <string_view>

namespace Imagina {
	enum class PixelDataType {
		Invalid = 0x0,

		// Integer
		Int8 = 0x10000,
		Int16,
		Int32,
		Int64,

		Uint8 = 0x10010,
		Uint16,
		Uint32,
		Uint64,

		// Fixed point
		Fract8 = 0x20000, // Same as Uint, but binary point is before MSB
		Fract16,
		Fract32,
		Fract64,

		// Floating point
		Float16 = 0x30000, // Reserved
		Float32,
		Float64,
		FloatHR,

		// Complex
		SRComplex = 0x40000,
		HRComplex,

		// Color
		Monochrome8 = 0x80000,
		Monochrome16 = 0x80000,

		RGB8 = 0x80010,
		RGB16 = 0x80010,

		RGB16F = 0x80018,
		RGB32F = 0x80019,

		RGBA8 = 0x80020,
		RGBA16 = 0x80020,

		RGBA16F = 0x80028,
		RGBA32F = 0x80029,


		SRReal = Float64,
		HRReal = FloatHR,
	};

	struct FieldDescriptor {
		std::string_view Name;
		PixelDataType Type;
		ptrdiff_t Offset;
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

		virtual void Process(void *output, void *input) const = 0;
	};

	class im_export PixelPipeline {
		const PixelDataDescriptor *evaluatorOutput = nullptr;
		IPixelProcessor *preprocessor = nullptr;
		IPixelProcessor *postprocessor = nullptr;

	public:

		void UseEvaluator(IEvaluator *evaluator);

		void UsePreprocessor(IPixelProcessor *processor);
		void UsePostprocessor(IPixelProcessor *processor);

		void Link();

		inline IPixelProcessor *GetPreprocessor() { return preprocessor; }
		inline IPixelProcessor *GetPostprocessor() { return postprocessor; }

		inline void Preprocess(void *output, void *input) const { preprocessor->Process(output, input); } // FIXME: Doesn't work if preprocessor == nullptr
		inline void Postprocess(void *output, void *input) const { postprocessor->Process(output, input); } // FIXME: Doesn't work if postprocessor == nullptr
	};

	class im_export TestProcessor : public IPixelProcessor { // Converts Float64 to Float32
		static const PixelDataDescriptor OutputDescriptor;
		static const FieldDescriptor OutputFields[1];

		ptrdiff_t sourceOffset;

	public:
		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *output, void *input) const override;
	};

	class im_export TestProcessor2 : public IPixelProcessor { // Converts Float64 to Float32
		ptrdiff_t iterationsOffset;
		ptrdiff_t finalZOffset;

	public:
		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *output, void *input) const override;
	};
}