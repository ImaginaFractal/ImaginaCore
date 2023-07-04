#include "proxy"
#include "output_info_helper"

namespace Imagina {
	LowPrecisionEvaluatorProxy::~LowPrecisionEvaluatorProxy() {
		release(instance);
	}
	const PixelDataInfo *LowPrecisionEvaluatorProxy::GetOutputInfo() {
		if (!outputInfo) {
			outputInfo = (PixelDataInfo *)getOutputInfo(instance);
		}
		return outputInfo;
	}
	void LowPrecisionEvaluatorProxy::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		setEvaluationParameters(instance, (StandardEvaluationParameters *)&parameters);
	}
	void LowPrecisionEvaluatorProxy::Evaluate(IRasterizingInterface &rasterizingInterface) {
		evaluate(instance, (ImCApi::IRasterizingInterface *)&rasterizingInterface);
	}


	SimpleEvaluatorProxy::~SimpleEvaluatorProxy() {
		release(instance);
	}
	const PixelDataInfo *SimpleEvaluatorProxy::GetOutputInfo() {
		if (!outputInfo) {
			outputInfo = (PixelDataInfo *)getOutputInfo(instance);
		}
		return outputInfo;
	}
	void SimpleEvaluatorProxy::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		setEvaluationParameters(instance, (StandardEvaluationParameters *)&parameters);
		SimpleEvaluator::SetEvaluationParameters(parameters);
	}
	void SimpleEvaluatorProxy::Precompute() {
		setReferenceLocationAndPrecompute(instance, x, y, radius);
	}
	void SimpleEvaluatorProxy::Evaluate(IRasterizingInterface &rasterizingInterface) {
		evaluate(instance, (ImCApi::IRasterizingInterface *)&rasterizingInterface);
	}
}