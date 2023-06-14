#pragma once

#include "Declarations.h"
#include "PlatformDependent.h"
#include <cstdint>
#include <string_view>
#include <initializer_list>
#include <cassert>

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
		virtual ~IPixelProcessor() = default;

		virtual void SetInput(const PixelDataDescriptor *descriptor) = 0;
		virtual const PixelDataDescriptor *GetOutputDescriptor() = 0;

		virtual void Process(void *output, void *input) const = 0;
	};

	// A special case of SerialCompositeProcessor
	class im_export SerialCompositeProcessor2 : public IPixelProcessor {
		IPixelProcessor *processors[2];
		size_t intermediateDataSize = 0;

	public:
		SerialCompositeProcessor2(IPixelProcessor *first, IPixelProcessor *second) : processors{ first, second } { assert(first && second); }

		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *output, void *input) const override;
	};

	class im_export SerialCompositeProcessor : public IPixelProcessor {
		IPixelProcessor **processors;
		size_t processorCount;
		size_t intermediateDataSize = 0;

	public:
		SerialCompositeProcessor(std::initializer_list<IPixelProcessor *> processors);

		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *output, void *input) const override;
	};

	class im_export CopyProcessor : public IPixelProcessor {
		const PixelDataDescriptor *output;
		size_t dataSize;

	public:
		virtual void SetInput(const PixelDataDescriptor *descriptor) override;
		virtual const PixelDataDescriptor *GetOutputDescriptor() override;
		virtual void Process(void *output, void *input) const override;
	};

	class im_export PixelPipeline final {
		IPixelProcessor *stages[4]{};
		const PixelDataDescriptor *outputs[4]{};

		IPixelProcessor *composite2[2]{};
		IPixelProcessor *composite3 = nullptr;

		bool linked = false;

	public:
		enum Stage : uint8_t {
			None = 0,
			Evaluated = 0,

			Preprocess = 1,
			Preprocessed = 1,

			Postprocess = 2,
			Postprocessed = 2,

			Colorize = 3,
			Colorized = 3,
		};

		~PixelPipeline() {
			delete composite2[0];
			delete composite2[1];
			delete composite3;
		}

		IPixelProcessor *GetCompositeProcessor(Stage first, Stage last);

		static constexpr bool StageValid(Stage stage) { return stage > Stage::None && stage <= Stage::Colorize; }

		bool Equivalent(Stage a, Stage b) { return outputs[a] == outputs[b]; }
		
		void UseEvaluator(IEvaluator *evaluator);

		void UsePreprocessor(IPixelProcessor *processor);
		void UsePostprocessor(IPixelProcessor *processor);
		void UseColorizer(IPixelProcessor *processor);

		void Link();

		inline bool IsLinked() { return linked; }

		inline IPixelProcessor *GetPreprocessor()	{ return stages[1]; }
		inline IPixelProcessor *GetPostprocessor()	{ return stages[2]; }
		inline IPixelProcessor *GetColorizer()		{ return stages[3]; }

		inline size_t PreprocessedDataSize()	const { return outputs[1]->Size; }
		inline size_t PostprocessedDataSize()	const { return outputs[2]->Size; }
		inline size_t ColorizedDataSize()		const { return outputs[3]->Size; }

		const PixelDataDescriptor *GetOutputOfStage(Stage stage);
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