#include <imagina/engine.h>
#include <imagina/module.h>

namespace Imagina {
	class StandardEngine::EvaluationTask : public ParallelTask, public Task::Cancellable/*, public ProgressTrackable*/ {
		IEvaluator evaluator;
		IPixelReceiver pixelReceiver;
	public:
		EvaluationTask(IEvaluator evaluator, IPixelReceiver pixelReceiver)
			: evaluator(evaluator), pixelReceiver(pixelReceiver) {}
		//virtual std::string_view GetDescription() const override;
		virtual void Execute() override;
		//virtual bool GetProgress(real_sr &Numerator, real_sr &Denoninator) const override;
		virtual void Cancel() override;
	};

	void StandardEngine::EvaluationTask::Execute() {
		IRasterizer rasterizer = pixelReceiver.GetRasterizer();
		evaluator.Evaluate(rasterizer);
		pixelReceiver.FreeRasterizer(rasterizer);
	}

	void StandardEngine::EvaluationTask::Cancel() {
		pixelReceiver.Cancel();
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
		locationManager(*(multi_precision *)(void *)CreateComponent(ComponentType::MultiPrecision)),
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

	ITask StandardEngine::AddTask(const HRCircle &circle, IPixelReceiver pixelReceiver) {
		if (pixelExecutionContext) pixelExecutionContext->WaitAndRelease();
		pixelExecutionContext = Computation::AddTask(new EvaluationTask(evaluator, pixelReceiver));
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

	void StandardEngine::SetReferenceLocation(const real_hp &x, const real_hp &y, real_hr radius) {
		CancelTasks();
		this->x |= x;
		this->y |= y;
		this->radius = radius;
		precomputeExecutionContext = Computation::AddTask([this]() { evaluator.Prepare(this->x, this->y, this->radius, parameters); });
	}
}