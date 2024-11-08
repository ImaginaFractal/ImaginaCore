#pragma once

#include "declarations.h"
#include "numerics.h"
#include "pixel_data.h"
#include <initializer_list>
#include <cassert>
#include <imagina/interface/pixel_processing.h>

namespace Imagina {
	// A special case of SerialCompositeProcessor
	class SerialCompositeProcessor2 {
		IPixelProcessor processors[2];
		size_t intermediateDataSize = 0;

	public:
		SerialCompositeProcessor2(IPixelProcessor first, IPixelProcessor second) : processors{ first, second } { assert(first && second); }

		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(SerialCompositeProcessor2, IPixelProcessor);

	class SerialCompositeProcessor {
		IPixelProcessor *processors;
		size_t processorCount;
		size_t intermediateDataSize = 0;

	public:
		SerialCompositeProcessor(std::initializer_list<IPixelProcessor> processors);

		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(SerialCompositeProcessor, IPixelProcessor);

	class CopyProcessor {
		const PixelDataInfo *output;
		size_t dataSize;

	public:
		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(CopyProcessor, IPixelProcessor);

	class PixelPipeline final {
		IPixelProcessor stages[4]{};
		const PixelDataInfo *outputs[4]{};

		IPixelProcessor composite2[2]{}; // TODO: mutable
		IPixelProcessor composite3 = nullptr;

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
			composite2[0].Release();
			composite2[1].Release();
			composite3.Release();
		}

		IPixelProcessor GetCompositeProcessor(Stage first, Stage last);

		static constexpr bool StageValid(Stage stage) { return stage > Stage::None && stage <= Stage::Colorize; }

		bool Equivalent(Stage a, Stage b) { return outputs[a] == outputs[b]; }
		
		void UseEngine(IEngine engine);

		void UsePreprocessor(IPixelProcessor processor);
		void UsePostprocessor(IPixelProcessor processor);
		void UseColorizer(IPixelProcessor processor);

		void Link();

		inline bool IsLinked() { return linked; }

		inline IPixelProcessor GetPreprocessor()	{ return stages[1]; }
		inline IPixelProcessor GetPostprocessor()	{ return stages[2]; }
		inline IPixelProcessor GetColorizer()		{ return stages[3]; }

		inline size_t PreprocessedDataSize()	const { return outputs[1]->Size; }
		inline size_t PostprocessedDataSize()	const { return outputs[2]->Size; }
		inline size_t ColorizedDataSize()		const { return outputs[3]->Size; }

		const PixelDataInfo *GetOutputOfStage(Stage stage);
	};

	class TestProcessor { // Converts Float64 to Float32 // TEMPORARY
		static const PixelDataInfo OutputInfo;
		static const FieldInfo OutputFields[1];

		ptrdiff_t sourceOffset;

	public:
		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(TestProcessor, IPixelProcessor);

	class TestProcessor2 {
		ptrdiff_t iterationsOffset;
		ptrdiff_t finalZOffset;

	public:
		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(TestProcessor2, IPixelProcessor);

	class PaletteLookup {
		const FieldInfo *inputField;
		const rgba32f *palette;
		size_t paletteSize;

	public:
		real_sr valueMultiplier = 0x1p-8;
		real_sr valueOffset = 0.0;

		PaletteLookup(const rgba32f *palette, size_t paletteSize);

		~PaletteLookup();

		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(PaletteLookup, IPixelProcessor);

	class BSplineInterpolator {
	protected:
		const FieldInfo *inputField;
		const rgba32f *palette;
		size_t paletteSize;

	public:
		real_sr valueMultiplier = 0x1p-8;
		real_sr valueOffset = 0.0;

		BSplineInterpolator(const rgba32f *palette, size_t paletteSize);

		~BSplineInterpolator();

		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();
		void Process(void *output, void *input) const;
	};

	IMPLEMENT_INTERFACE(BSplineInterpolator, IPixelProcessor);
}