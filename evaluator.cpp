#include <imagina/evaluator.h>
#include <imagina/pixel_management.h>
#include <imagina/output_info_helper.h>
#include <imagina/module.h>

namespace Imagina {
	class SimpleEvaluator::EvaluationTask : public ParallelTask, public Task::Cancellable/*, public ProgressTrackable*/ {
		SimpleEvaluator *evaluator;
		IPixelReceiver pixelReceiver;
	public:
		EvaluationTask(SimpleEvaluator *evaluator, IPixelReceiver pixelReceiver)
			: evaluator(evaluator), pixelReceiver(pixelReceiver) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(real_sr &Numerator, real_sr &Denoninator) const override;
		virtual void Cancel() override;
	};

	void SimpleEvaluator::EvaluationTask::Execute() {
		IRasterizer rasterizer = pixelReceiver.GetRasterizer();
		evaluator->Evaluate(rasterizer);
		pixelReceiver.FreeRasterizer(rasterizer);
	}

	void SimpleEvaluator::EvaluationTask::Cancel() {
		pixelReceiver.Cancel();
	}


	void SimpleEvaluator::CancelTasks() {
		if (precomputeExecutionContext && !precomputeExecutionContext->Terminated()) precomputeExecutionContext->Cancel();
		if (pixelExecutionContext) {
			if (!pixelExecutionContext->Terminated()) pixelExecutionContext->Cancel();
			pixelExecutionContext->WaitAndRelease();
			pixelExecutionContext = nullptr;
		}
		if (precomputeExecutionContext) {
			precomputeExecutionContext->WaitAndRelease();
			precomputeExecutionContext = nullptr;
		}
	}

	SimpleEvaluator::SimpleEvaluator() : locationManager(*(multi_precision *)(void *)CreateComponent(ComponentType::MultiPrecision)) {
		locationManager.SetEvaluator(this);
	}

	ILocationManager SimpleEvaluator::GetLocationManager() {
		return locationManager;
	}

	bool SimpleEvaluator::Ready() {
		if (precomputeExecutionContext) {
			if (!precomputeExecutionContext->Terminated()) return false;
			precomputeExecutionContext->Release();
			precomputeExecutionContext = nullptr;
		}
		return true;
	}

	ExecutionContext *SimpleEvaluator::RunEvaluation(const HRCircle &, IPixelReceiver pixelReceiver) {
		if (pixelExecutionContext) pixelExecutionContext->WaitAndRelease();
		pixelExecutionContext = Computation::AddTask(new EvaluationTask(this, pixelReceiver));
		pixelExecutionContext->AddReference();
		return pixelExecutionContext;
	}

	void SimpleEvaluator::SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius) {
		CancelTasks();
		this->x |= x;
		this->y |= y;
		this->radius = radius;
		precomputeExecutionContext = Computation::AddTask(std::bind(&SimpleEvaluator::Prepare, this));
	}

	void SimpleEvaluator::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		CancelTasks();
		this->parameters = parameters;
		if (x.Valid()) {
			precomputeExecutionContext = Computation::AddTask(std::bind(&SimpleEvaluator::Prepare, this));
		}
	}

	class LowPrecisionEvaluator::LPRasterizer {
		IRasterizer rasterizer;
		real_sr referenceX, referenceY;

	public:
		LPRasterizer(IRasterizer rasterizer, real_sr referenceX, real_sr referenceY)
			: rasterizer(rasterizer), referenceX(referenceX), referenceY(referenceY) {}

		bool GetPixel(real_hr &x, real_hr &y);
		void GetDdx(real_hr &x, real_hr &y);
		void GetDdy(real_hr &x, real_hr &y);
		void WriteResults(void *value);
	};

	IMPLEMENT_INTERFACE(LowPrecisionEvaluator::LPRasterizer, IRasterizer);

	bool LowPrecisionEvaluator::LPRasterizer::GetPixel(real_hr &x, real_hr &y) {
		bool result = rasterizer.GetPixel(x, y);
		x += referenceX;
		y += referenceY;
		return result;
	}
	void LowPrecisionEvaluator::LPRasterizer::GetDdx(real_hr &x, real_hr &y) {
		rasterizer.GetDdx(x, y);
	}
	void LowPrecisionEvaluator::LPRasterizer::GetDdy(real_hr &x, real_hr &y) {
		rasterizer.GetDdy(x, y);
	}
	void LowPrecisionEvaluator::LPRasterizer::WriteResults(void *value) {
		rasterizer.WriteResults(value);
	}


	class LowPrecisionEvaluator::EvaluationTask : public ParallelTask/*, public ProgressTrackable*/ {
		LowPrecisionEvaluator *evaluator;
		IPixelReceiver pixelReceiver;
		real_sr referenceX, referenceY;
	public:
		EvaluationTask(LowPrecisionEvaluator *evaluator, IPixelReceiver pixelReceiver, real_sr referenceX, real_sr referenceY)
			: evaluator(evaluator), pixelReceiver(pixelReceiver), referenceX(referenceX), referenceY(referenceY) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(real_sr &Numerator, real_sr &Denoninator) const override;
	};

	void LowPrecisionEvaluator::EvaluationTask::Execute() {
		IRasterizer rasterizer = pixelReceiver.GetRasterizer();
		LPRasterizer lpRasterizer(rasterizer, referenceX, referenceY);
		evaluator->Evaluate(lpRasterizer);
		pixelReceiver.FreeRasterizer(rasterizer);
	}

	
	LowPrecisionEvaluator::LowPrecisionEvaluator() : locationManager(*(multi_precision *)(void *)CreateComponent(ComponentType::MultiPrecision)) {
		locationManager.SetEvaluator(this);
	}

	ILocationManager LowPrecisionEvaluator::GetLocationManager() {
		return locationManager;
	}

	bool LowPrecisionEvaluator::Ready() {
		return true;
	}

	ExecutionContext *LowPrecisionEvaluator::RunEvaluation(const HRCircle &, IPixelReceiver pixelReceiver) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, pixelReceiver, referenceX, referenceY));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}

	void LowPrecisionEvaluator::SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr) {
		referenceX = real_sr(x);
		referenceY = real_sr(y);
	}

	void LowPrecisionEvaluator::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		// FIXME
		//CancelTasks();
		this->parameters = parameters;
	}


	const PixelDataInfo *TestSimpleEvaluator::GetOutputInfo() {
		IM_GET_OUTPUT_INFO_IMPL(Output, Value);
	}

	void TestSimpleEvaluator::Prepare() {
		delete[] reference;
		//reference = new complex_sr[parameters.Iterations + 1];
		reference = new complex[parameters.Iterations + 1];
		complex_hp C = complex_hp(x, y);
		//referenceC = complex_sr(real_sr(x), real_sr(y));

		reference[0] = real(0.0);
		reference[1] = complex(real(x), real(y));

		complex_hp Z = C;

		size_t i;
		for (i = 2; i <= parameters.Iterations; i++) {
			Z = Z * Z + C;
			complex z = complex(Z);
			reference[i] = z;

			if (norm(z) > 16.0) {
				i++;
				break;
			}
		}

		referenceLength = i - 1;
	}

	void TestSimpleEvaluator::Evaluate(IRasterizer rasterizer) {
		real_hr x, y;
		while (rasterizer.GetPixel(x, y)) {
			complex dc = { real(x), real(y) };
			complex Z = real(0.0), z = real(0.0), dz = real(0.0);

			uint_iter i = 0, j = 0;
			while (i < parameters.Iterations) {
				dz = dz * (Z + z) + dc;
				i++; j++;

				Z = reference[j];
				z = Z + dz;

				if (norm(z) > 4096.0) break;

				if (j == referenceLength || norm(z) < norm(dz)) {
					Z = real(0.0);
					dz = z;
					j = 0;
				}
			}

			//double result = i;
			Output output;
			output.Value = i;

			rasterizer.WriteResults(&output);
		}
	}


	const PixelDataInfo *TestEvaluator::GetOutputInfo() {
		//struct output {
		//	double Iterations;
		//	double Iterations2;
		//};
		//IM_OUTPUT_FIELD(a, b);
		//IM_OUTPUT_INFO(output, Iterations, Iterations2);
		//return &OutputInfo;
		/*using namespace std;
		static const FieldInfo OutputFields[1]{
			{ PixelDataType::Float64, 0, "Iterations"sv }
		};

		static const PixelDataInfo OutputInfo{
			8, 1, OutputFields
		};

		return &OutputInfo;*/
		IM_GET_OUTPUT_INFO_IMPL(Output, Iterations, FinalZ);
	}

	void TestEvaluator::Evaluate(IRasterizer rasterizer) {
		real_hr x, y;
		while (rasterizer.GetPixel(x, y)) {
			complex_sr c = { real_sr(x), real_sr(y) };
			complex_sr z = 0.0;

			uint_iter i;
			for (i = 0; i < parameters.Iterations; i++) {
				z = z * z + c;
				if (norm(z) > 4096.0) break;
			}

			//double result = i;
			Output output;
			output.Iterations = i;
			output.FinalZ = z;

			rasterizer.WriteResults(&output);
		}
	}
}