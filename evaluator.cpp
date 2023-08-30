#include <Imagina/evaluator>
#include <Imagina/pixel_management>
#include <Imagina/output_info_helper>
#include <Imagina/module>

namespace Imagina {
	class SimpleEvaluator::EvaluationTask : public ParallelTask, public Task::Cancellable/*, public ProgressTrackable*/ {
		SimpleEvaluator *evaluator;
		IRasterizer rasterizer;
	public:
		EvaluationTask(SimpleEvaluator *evaluator, IRasterizer rasterizer)
			: evaluator(evaluator), rasterizer(rasterizer) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
		virtual void Cancel() override;
	};

	void SimpleEvaluator::EvaluationTask::Execute() {
		IRasterizingInterface rasterizingInterface = rasterizer.GetRasterizingInterface();
		evaluator->Evaluate(rasterizingInterface);
		rasterizer.FreeRasterizingInterface(rasterizingInterface);
	}

	void SimpleEvaluator::EvaluationTask::Cancel() {
		rasterizer.Cancel();
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

	template<>
	Imagina::IComplexLocationSinkVTable Imagina::IComplexLocationSinkVTable::value<SimpleEvaluator> = Imagina::IComplexLocationSinkVTable::OfType<SimpleEvaluator>();

	SimpleEvaluator::SimpleEvaluator() : locationManager(*(MultiPrecision *)(void *)CreateComponent(ComponentType::MultiPrecision)) {
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

	ExecutionContext *SimpleEvaluator::RunEvaluation(const HRCircle &, IRasterizer rasterizer) {
		if (pixelExecutionContext) pixelExecutionContext->WaitAndRelease();
		pixelExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer));
		pixelExecutionContext->AddReference();
		return pixelExecutionContext;
	}

	void SimpleEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) {
		CancelTasks();
		this->x |= x;
		this->y |= y;
		this->radius = radius;
		precomputeExecutionContext = Computation::AddTask(std::bind(&SimpleEvaluator::Precompute, this));
	}

	void SimpleEvaluator::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		CancelTasks();
		this->parameters = parameters;
		if (x.Valid()) {
			precomputeExecutionContext = Computation::AddTask(std::bind(&SimpleEvaluator::Precompute, this));
		}
	}

	class LowPrecisionEvaluator::LPRasterizingInterface {
		IRasterizingInterface rasterizingInterface;
		SRReal referenceX, referenceY;

	public:
		LPRasterizingInterface(IRasterizingInterface rasterizingInterface, SRReal referenceX, SRReal referenceY)
			: rasterizingInterface(rasterizingInterface), referenceX(referenceX), referenceY(referenceY) {}

		bool GetPixel(HRReal &x, HRReal &y);
		void GetDdx(HRReal &x, HRReal &y);
		void GetDdy(HRReal &x, HRReal &y);
		void WriteResults(void *value);
	};

	template<>
	IRasterizingInterfaceVTable IRasterizingInterfaceVTable::value<LowPrecisionEvaluator::LPRasterizingInterface> = IRasterizingInterfaceVTable::OfType<LowPrecisionEvaluator::LPRasterizingInterface>();

	bool LowPrecisionEvaluator::LPRasterizingInterface::GetPixel(HRReal &x, HRReal &y) {
		bool result = rasterizingInterface.GetPixel(x, y);
		x += referenceX;
		y += referenceY;
		return result;
	}
	void LowPrecisionEvaluator::LPRasterizingInterface::GetDdx(HRReal &x, HRReal &y) {
		rasterizingInterface.GetDdx(x, y);
	}
	void LowPrecisionEvaluator::LPRasterizingInterface::GetDdy(HRReal &x, HRReal &y) {
		rasterizingInterface.GetDdy(x, y);
	}
	void LowPrecisionEvaluator::LPRasterizingInterface::WriteResults(void *value) {
		rasterizingInterface.WriteResults(value);
	}


	class LowPrecisionEvaluator::EvaluationTask : public ParallelTask/*, public ProgressTrackable*/ {
		LowPrecisionEvaluator *evaluator;
		IRasterizer rasterizer;
		SRReal referenceX, referenceY;
	public:
		EvaluationTask(LowPrecisionEvaluator *evaluator, IRasterizer rasterizer, SRReal referenceX, SRReal referenceY)
			: evaluator(evaluator), rasterizer(rasterizer), referenceX(referenceX), referenceY(referenceY) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
	};

	void LowPrecisionEvaluator::EvaluationTask::Execute() {
		IRasterizingInterface rasterizingInterface = rasterizer.GetRasterizingInterface();
		LPRasterizingInterface lpRasterizingInterface(rasterizingInterface, referenceX, referenceY);
		evaluator->Evaluate(lpRasterizingInterface);
		rasterizer.FreeRasterizingInterface(rasterizingInterface);
	}

	
	template<>
	Imagina::IComplexLocationSinkVTable Imagina::IComplexLocationSinkVTable::value<LowPrecisionEvaluator> = Imagina::IComplexLocationSinkVTable::OfType<LowPrecisionEvaluator>();

	LowPrecisionEvaluator::LowPrecisionEvaluator() : locationManager(*(MultiPrecision *)(void *)CreateComponent(ComponentType::MultiPrecision)) {
		locationManager.SetEvaluator(this);
	}

	ILocationManager LowPrecisionEvaluator::GetLocationManager() {
		return locationManager;
	}

	bool LowPrecisionEvaluator::Ready() {
		return true;
	}

	ExecutionContext *LowPrecisionEvaluator::RunEvaluation(const HRCircle &, IRasterizer rasterizer) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer, referenceX, referenceY));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}

	void LowPrecisionEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal) {
		referenceX = SRReal(x);
		referenceY = SRReal(y);
	}

	void LowPrecisionEvaluator::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		// FIXME
		//CancelTasks();
		this->parameters = parameters;
	}


	const PixelDataInfo *TestSimpleEvaluator::GetOutputInfo() {
		IM_GET_OUTPUT_INFO_IMPL(Output, Value);
	}

	void TestSimpleEvaluator::Precompute() {
		delete[] reference;
		//reference = new SRComplex[parameters.Iterations + 1];
		reference = new complex[parameters.Iterations + 1];
		HPComplex C = HPComplex(x, y);
		//referenceC = SRComplex(SRReal(x), SRReal(y));

		reference[0] = real(0.0);
		reference[1] = complex(real(x), real(y));

		HPComplex Z = C;

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

	void TestSimpleEvaluator::Evaluate(IRasterizingInterface rasterizingInterface) {
		HRReal x, y;
		while (rasterizingInterface.GetPixel(x, y)) {
			complex dc = { real(x), real(y) };
			complex Z = real(0.0), z = real(0.0), dz = real(0.0);

			ITUInt i = 0, j = 0;
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

			rasterizingInterface.WriteResults(&output);
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

	void TestEvaluator::Evaluate(IRasterizingInterface rasterizingInterface) {
		HRReal x, y;
		while (rasterizingInterface.GetPixel(x, y)) {
			SRComplex c = { SRReal(x), SRReal(y) };
			SRComplex z = 0.0;

			ITUInt i;
			for (i = 0; i < parameters.Iterations; i++) {
				z = z * z + c;
				if (norm(z) > 4096.0) break;
			}

			//double result = i;
			Output output;
			output.Iterations = i;
			output.FinalZ = z;

			rasterizingInterface.WriteResults(&output);
		}
	}
}