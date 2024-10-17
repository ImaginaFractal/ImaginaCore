#pragma once

#include "numerics.h"
#include "declarations.h"
#include "computation.h" // TODO: Use declarations instead
#include "pixel_data.h"
#include "location_manager.h"
#include <imagina/interface/evaluator.h>
#include <imagina/interface/pixel_management.h>

namespace Imagina {
	// TODO: Remove
	class im_export SimpleEvaluator {
		class EvaluationTask;

		ExecutionContext *precomputeExecutionContext = nullptr;
		ExecutionContext *pixelExecutionContext = nullptr;

		StandardLocationManager locationManager;

		void CancelTasks();

	protected:
		real_hp x, y;
		real_hr radius;
		StandardEvaluationParameters parameters;

	public:
		SimpleEvaluator();
		virtual ~SimpleEvaluator() = default; // This is not really necessary, but without it the compiler may complain

		ILocationManager GetLocationManager();
		bool Ready();
		ExecutionContext *RunEvaluation(const HRCircle &circle, IPixelReceiver pixelReceiver);
		void SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius);
		void SetEvaluationParameters(const StandardEvaluationParameters &parameters);

		virtual void Prepare() = 0;
		virtual void Evaluate(IRasterizingInterface rasterizingInterface) = 0;
	};

	IMPLEMENT_INTERFACE(SimpleEvaluator, IComplexLocationSink);

	class im_export LowPrecisionEvaluator {
		class LPRasterizingInterface;
		class EvaluationTask;
		friend struct IRasterizingInterfaceVTable;

		real_sr referenceX = 0.0, referenceY = 0.0;
		ExecutionContext *currentExecutionContext = nullptr;

		StandardLocationManager locationManager;

	protected:
		StandardEvaluationParameters parameters;

	public:
		LowPrecisionEvaluator();
		virtual ~LowPrecisionEvaluator() = default; // Same as above

		ILocationManager GetLocationManager();
		bool Ready();
		ExecutionContext *RunEvaluation(const HRCircle &circle, IPixelReceiver pixelReceiver);
		void SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius);
		void SetEvaluationParameters(const StandardEvaluationParameters &parameters);

		virtual void Evaluate(IRasterizingInterface rasterizingInterface) = 0;
	};

	IMPLEMENT_INTERFACE(LowPrecisionEvaluator, IComplexLocationSink);

	class im_export TestSimpleEvaluator : public SimpleEvaluator {
		using real = real_sr;
		using complex = complex_sr;
		struct Output {
			double Value;
		};

		uint64_t referenceLength;
		//complex_sr reference[1025];
		//complex_sr *reference = nullptr;
		complex *reference = nullptr;
		//complex_sr referenceC;

	public:
		const PixelDataInfo *GetOutputInfo();

		virtual void Prepare() override;
		virtual void Evaluate(IRasterizingInterface rasterizingInterface) override;
	};

	// TEMPORARY
	class im_export TestEvaluator : public LowPrecisionEvaluator {
		struct Output {
			uint64_t Iterations;
			complex_sr FinalZ;
		};

	public:
		const PixelDataInfo *GetOutputInfo();

		virtual void Evaluate(IRasterizingInterface rasterizingInterface) override;
	};
}