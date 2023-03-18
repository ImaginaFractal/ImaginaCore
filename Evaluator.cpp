#include "Evaluator.h"
#include "PixelManager.h"
#include "OutputDescriptorHelper.h"

namespace Imagina {
	class SimpleEvaluator::EvaluationTask : public ParallelTask, public Task::Cancellable/*, public ProgressTrackable*/ {
		SimpleEvaluator *evaluator;
		IRasterizer *rasterizer;
	public:
		EvaluationTask(SimpleEvaluator *evaluator, IRasterizer *rasterizer)
			: evaluator(evaluator), rasterizer(rasterizer) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
		virtual void Cancel() override;
	};

	void SimpleEvaluator::EvaluationTask::Execute() {
		IRasterizingInterface &rasterizingInterface = rasterizer->GetRasterizingInterface();
		evaluator->Evaluate(rasterizingInterface);
		rasterizer->FreeRasterizingInterface(rasterizingInterface);
	}

	void SimpleEvaluator::EvaluationTask::Cancel() {
		rasterizer->Cancel();
	}


	ExecutionContext *SimpleEvaluator::RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}

	void SimpleEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) {
		if (currentExecutionContext) {
			if (!currentExecutionContext->Terminated()) currentExecutionContext->Cancel();
			currentExecutionContext->WaitAndRelease();
		}
		Precompute(x, y, radius);
	}

	class LowPrecisionEvaluator::LPRasterizingInterface : public IRasterizingInterface {
		IRasterizingInterface &rasterizingInterface;
		SRReal referenceX, referenceY;

	public:
		LPRasterizingInterface(IRasterizingInterface &rasterizingInterface, SRReal referenceX, SRReal referenceY)
			: rasterizingInterface(rasterizingInterface), referenceX(referenceX), referenceY(referenceY) {}

		virtual bool GetCoordinate(HRReal &x, HRReal &y) override;
		virtual void WriteResults(void *value) override;
	};

	bool LowPrecisionEvaluator::LPRasterizingInterface::GetCoordinate(HRReal &x, HRReal &y) {
		bool result = rasterizingInterface.GetCoordinate(x, y);
		x += referenceX;
		y += referenceY;
		return result;
	}
	void LowPrecisionEvaluator::LPRasterizingInterface::WriteResults(void *value) {
		rasterizingInterface.WriteResults(value);
	}


	class LowPrecisionEvaluator::EvaluationTask : public ParallelTask/*, public ProgressTrackable*/ {
		LowPrecisionEvaluator *evaluator;
		IRasterizer *rasterizer;
		SRReal referenceX, referenceY;
	public:
		EvaluationTask(LowPrecisionEvaluator *evaluator, IRasterizer *rasterizer, SRReal referenceX, SRReal referenceY)
			: evaluator(evaluator), rasterizer(rasterizer), referenceX(referenceX), referenceY(referenceY) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
	};

	void LowPrecisionEvaluator::EvaluationTask::Execute() {
		IRasterizingInterface &rasterizingInterface = rasterizer->GetRasterizingInterface();
		LPRasterizingInterface lpRasterizingInterface(rasterizingInterface, referenceX, referenceY);
		evaluator->Evaluate(lpRasterizingInterface);
		rasterizer->FreeRasterizingInterface(rasterizingInterface);
	}


	ExecutionContext *LowPrecisionEvaluator::RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer, referenceX, referenceY));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}

	void LowPrecisionEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) {
		referenceX = SRReal(x);
		referenceY = SRReal(y);
	}


	const PixelDataDescriptor *TestSimpleEvaluator::GetOutputDescriptor() {
		IM_GET_OUTPUT_DESCRIPTOR_IMPL(Output, Value);
	}

	void TestSimpleEvaluator::Precompute(const HPReal &x, const HPReal &y, HRReal radius) {
		HPComplex C = HPComplex(x, y);
		referenceC = HRComplex(HRReal(x), HRReal(y));

		reference[0] = 0.0;
		reference[1] = referenceC;

		HPComplex Z = C;

		size_t i;
		for (i = 2; i <= 1024; i++) {
			Z = Z * Z + C;
			SRComplex z = SRComplex(Z);
			reference[i] = z;

			if (norm(z) > 16.0) {
				i++;
				break;
			}
		}

		referenceLength = i - 1;
	}

	void TestSimpleEvaluator::Evaluate(IRasterizingInterface &rasterizingInterface) {
		HRReal x, y;
		while (rasterizingInterface.GetCoordinate(x, y)) {
			SRComplex dc = { x, y };
			SRComplex Z = 0.0, z = 0.0, dz = 0.0;

			long i = 0, j = 0;
			while (i < 1024) {
				dz = dz * (Z + z) + dc;
				i++; j++;

				Z = reference[j];
				z = Z + dz;

				if (norm(z) > 4096.0) break;

				if (j == referenceLength || norm(z) < norm(dz)) {
					Z = 0.0;
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


	const PixelDataDescriptor *TestEvaluator::GetOutputDescriptor() {
		//struct output {
		//	double Iterations;
		//	double Iterations2;
		//};
		//IM_OUTPUT_FIELD(a, b);
		//IM_OUTPUT_DESCRIPTOR(output, Iterations, Iterations2);
		//return &OutputDescriptor;
		/*using namespace std;
		static const FieldDescriptor OutputFields[1]{
			{ PixelDataType::Float64, 0, "Iterations"sv }
		};

		static const PixelDataDescriptor OutputDescriptor{
			8, 1, OutputFields
		};

		return &OutputDescriptor;*/
		IM_GET_OUTPUT_DESCRIPTOR_IMPL(Output, Iterations, FinalZ);
	}

	void TestEvaluator::Evaluate(IRasterizingInterface &rasterizingInterface) {
		HRReal x, y;
		while (rasterizingInterface.GetCoordinate(x, y)) {
			SRComplex c = { x, y };
			SRComplex z = 0.0;
	
			long i;
			for (i = 0; i < 256; i++) {
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