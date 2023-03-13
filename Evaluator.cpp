#include "Evaluator.h"
#include "PixelManager.h"

namespace Imagina {
	void Evaluator::SetReferenceLocation(const HPReal &x, const HPReal &y) {
		referenceX = x;
		referenceY = y;
	}
	void Evaluator::Evaluate(IRasterizer &rasterizer) {
		IRasterizingInterface &ri = rasterizer.GetRasterizingInterface();
	
		HRReal x, y;
		while (ri.GetCoordinate(x, y)) {
			SRComplex c = { x + referenceX, y + referenceY };
			SRComplex z = 0.0;
	
			long i;
			for (i = 0; i < 256; i++) {
				z = z * z + c;
				if (norm(z) > 4.0) break;
			}
	
			ri.WriteResults(i);
		}
		rasterizer.FreeRasterizingInterface(ri);
	}
	class Evaluator::EvaluationTask : public ParallelTask/*, public ProgressTrackable*/ {
		Evaluator *evaluator;
		IRasterizer *rasterizer;
	public:
		EvaluationTask(Evaluator *evaluator, IRasterizer *rasterizer) : evaluator(evaluator), rasterizer(rasterizer) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
	};
	void Evaluator::EvaluationTask::Execute() {
		evaluator->Evaluate(*rasterizer);
	}
	ExecutionContext *Evaluator::RunTaskForRectangle(const HRRectangle &rectangle, IRasterizer *rasterizer) {
		if (currentExecutionContext) currentExecutionContext->WaitAndRelease();
		currentExecutionContext = Computation::AddTask(new EvaluationTask(this, rasterizer));
		currentExecutionContext->AddReference();
		return currentExecutionContext;
	}
}