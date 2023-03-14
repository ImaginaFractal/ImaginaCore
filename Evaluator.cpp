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
		virtual void WriteResults(SRReal Value) override;
	};

	bool LowPrecisionEvaluator::LPRasterizingInterface::GetCoordinate(HRReal &x, HRReal &y) {
		bool result = rasterizingInterface.GetCoordinate(x, y);
		x += referenceX;
		y += referenceY;
		return result;
	}
	void LowPrecisionEvaluator::LPRasterizingInterface::WriteResults(SRReal Value) {
		rasterizingInterface.WriteResults(Value);
	}

	class LowPrecisionEvaluator::EvaluationTask : public ParallelTask/*, public ProgressTrackable*/ {
		LowPrecisionEvaluator *evaluator;
		IRasterizer *rasterizer;
	public:
		EvaluationTask(LowPrecisionEvaluator *evaluator, IRasterizer *rasterizer) : evaluator(evaluator), rasterizer(rasterizer) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
	};

	void LowPrecisionEvaluator::EvaluationTask::Execute() {
		IRasterizingInterface &rasterizingInterface = rasterizer->GetRasterizingInterface();
		LPRasterizingInterface lpRasterizingInterface(rasterizingInterface, evaluator->referenceX, evaluator->referenceY);
		evaluator->Evaluate(lpRasterizingInterface);
		rasterizer->FreeRasterizingInterface(rasterizingInterface);
	}

	ExecutionContext *LowPrecisionEvaluator::RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}

	void LowPrecisionEvaluator::SetReferenceLocation(const HPReal &x, const HPReal &y) {
		referenceX = x;
		referenceY = y;
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
	
			rasterizingInterface.WriteResults(i);
		}
	}
}