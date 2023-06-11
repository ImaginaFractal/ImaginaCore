#pragma once

#include "Declarations.h"
#include "PlatformDependent.h"
#include <string_view>
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
		virtual void SetInput(const PixelDataDescriptor *descriptor) = 0;
		virtual const PixelDataDescriptor *GetOutputDescriptor() = 0;

		virtual void Process(void *output, void *input) const = 0;
	};

	class im_export PixelPipeline {
		IPixelProcessor *stages[4]{};
		const PixelDataDescriptor *outputs[4]{};
		bool linked = false;

	public:
		enum class Stage {
			None = 0,
			Preprocess = 1,
			Postprocess = 2,
			Colorize = 3,
		};

		inline void Process(Stage stage, void *output, void *input) const {
			assert(linked);
			const IPixelProcessor *processor = stages[(size_t)stage];
			if (processor) {
				processor->Process(output, input);
			} else {
				memcpy(output, input, outputs[(size_t)stage]->Size);
			}
		}

		// Process two consecutive stages, unnecessary copy and allocation are eliminated
		inline void Process2(Stage stage1, void *output, void *input) const {
			assert(linked);
			const IPixelProcessor *processor1 = stages[(size_t)stage1];
			const IPixelProcessor *processor2 = stages[(size_t)stage1 + 1];

			if (!processor2) {
				if (processor1) {
					processor1->Process(output, input);
				} else {
					memcpy(output, input, outputs[(size_t)stage1]->Size);
				}
				return;
			}

			void *processor1Output;
			if (processor1) {
				processor1Output = alloca(outputs[(size_t)stage1]->Size);
				processor1->Process(processor1Output, input);
			} else {
				processor1Output = input;
			}

			processor2->Process(output, processor1Output);
		}

		inline void ProcessAll(void *output, void *input) const {
			void *data1 = nullptr, *data2 = nullptr;
			do {
				if (stages[3]) break;
				data2 = output;
				if (stages[2]) break;
				data1 = output;
				if (stages[1]) break;
				memcpy(output, input, outputs[3]->Size);
				return;
			} while (0);

			if (stages[1]) {
				if (!data1) data1 = alloca(outputs[1]->Size);
				stages[1]->Process(data1, input);
			} else {
				data1 = input;
			}
			if (stages[2]) {
				if (!data2) data2 = alloca(outputs[2]->Size);
				stages[2]->Process(data2, data1);
			} else {
				data2 = data1;
			}
			if (stages[3]) {
				stages[3]->Process(output, data2);
			}
		}

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

		inline void Preprocess(void *output, void *input)	const { Process(Stage::Preprocess, output, input); }
		inline void Postprocess(void *output, void *input)	const { Process(Stage::Postprocess, output, input); }
		inline void Colorize(void *output, void *input)		const { Process(Stage::Colorize, output, input); }
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