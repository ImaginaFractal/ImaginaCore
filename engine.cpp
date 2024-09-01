#include <imagina/engine.h>
#include <imagina/module.h>

namespace Imagina {
	class StandardEngine::EvaluationTask : public ParallelTask, public Task::Cancellable/*, public ProgressTrackable*/ {
		IEvaluator evaluator;
		IRasterizer rasterizer;
	public:
		EvaluationTask(IEvaluator evaluator, IRasterizer rasterizer)
			: evaluator(evaluator), rasterizer(rasterizer) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(SRReal &Numerator, SRReal &Denoninator) const override;
		virtual void Cancel() override;
	};

	void StandardEngine::EvaluationTask::Execute() {
		IRasterizingInterface rasterizingInterface = rasterizer.GetRasterizingInterface();
		evaluator.Evaluate(rasterizingInterface);
		rasterizer.FreeRasterizingInterface(rasterizingInterface);
	}

	void StandardEngine::EvaluationTask::Cancel() {
		rasterizer.Cancel();
	}

	void StandardEngine::CancelTasks() {
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

	IMPLEMENT_INTERFACE(StandardEngine, IComplexLocationSink);

	StandardEngine::StandardEngine(IEvaluator evaluator) :
		locationManager(*(MultiPrecision *)(void *)CreateComponent(ComponentType::MultiPrecision)),
		evaluator(evaluator) {
		locationManager.SetEvaluator(this);
	}

	StandardEngine::~StandardEngine() {
		CancelTasks();
		evaluator.Release();
	}

	const PixelDataInfo *StandardEngine::GetOutputInfo() {
		return evaluator.GetOutputInfo();
	}

	ITask StandardEngine::AddTask(const HRCircle &circle, IRasterizer rasterizer) {
		if (pixelExecutionContext) pixelExecutionContext->WaitAndRelease();
		pixelExecutionContext = Computation::AddTask(new EvaluationTask(evaluator, rasterizer));
		pixelExecutionContext->AddReference();
		return pixelExecutionContext;
	}

	void StandardEngine::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		CancelTasks();
		this->parameters = parameters;
		if (x.Valid()) {
			precomputeExecutionContext = Computation::AddTask([this]() {
				evaluator.Prepare(x, y, radius, this->parameters);
			});
		}
	}

	bool StandardEngine::Ready() {
		if (precomputeExecutionContext) {
			if (!precomputeExecutionContext->Terminated()) return false;
			precomputeExecutionContext->Release();
			precomputeExecutionContext = nullptr;
		}
		return true;
	}

	void StandardEngine::SetController(IController controller) {
		locationManager.SetController(controller);
	}

	void StandardEngine::LocationChanged(const HRLocation &location) {
		locationManager.LocationChanged(location);
	}

	void StandardEngine::SetReferenceLocation(const HPReal &x, const HPReal &y, HRReal radius) {
		CancelTasks();
		this->x |= x;
		this->y |= y;
		this->radius = radius;
		precomputeExecutionContext = Computation::AddTask([this]() { evaluator.Prepare(this->x, this->y, this->radius, parameters); });
	}
}