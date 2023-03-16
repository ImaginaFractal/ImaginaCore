#include "Evaluator.h"
#include "PixelManager.h"

namespace Imagina {
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

	void LowPrecisionEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y) {
		referenceX = x;
		referenceY = y;
	}

	const PixelDataDescriptor *TestEvaluator::GetOutputDescriptor() {
		using namespace std;
		static const FieldDescriptor OutputFields[1]{
			{ DataType::Float64, 0, "Iterations"sv }
		};

		static const PixelDataDescriptor OutputDescriptor{
			8, 1, OutputFields
		};

		return &OutputDescriptor;
	}

	void TestEvaluator::Evaluate(IRasterizingInterface &rasterizingInterface) {
		HRReal x, y;
		while (rasterizingInterface.GetCoordinate(x, y)) {
			SRComplex c = { x, y };
			SRComplex z = 0.0;
	
			long i;
			for (i = 0; i < 256; i++) {
				z = z * z + c;
				if (norm(z) > 4.0) break;
			}
			
			double result = i;

			rasterizingInterface.WriteResults(&result);
		}
	}
}