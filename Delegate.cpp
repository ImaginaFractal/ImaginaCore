#include "delegate"
#include "output_info_helper"

namespace Imagina {
	LowPrecisionEvaluatorDelegate::~LowPrecisionEvaluatorDelegate() {
		release(instance);
	}
	const PixelDataInfo *LowPrecisionEvaluatorDelegate::GetOutputInfo() {
		if (!outputInfo) {
			outputInfo = (PixelDataInfo *)getOutputInfo(instance);
		}
		return outputInfo;
	}
	void LowPrecisionEvaluatorDelegate::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		setEvaluationParameters(instance, (StandardEvaluationParameters *)&parameters);
	}
	void LowPrecisionEvaluatorDelegate::Evaluate(IRasterizingInterface &rasterizingInterface) {
		evaluate(instance, (ImCApi::IRasterizingInterface *)&rasterizingInterface);
	}


	SimpleEvaluatorDelegate::~SimpleEvaluatorDelegate() {
		release(instance);
	}
	const PixelDataInfo *SimpleEvaluatorDelegate::GetOutputInfo() {
		if (!outputInfo) {
			outputInfo = (PixelDataInfo *)getOutputInfo(instance);
		}
		return outputInfo;
	}
	void SimpleEvaluatorDelegate::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		setEvaluationParameters(instance, (StandardEvaluationParameters *)&parameters);
		SimpleEvaluator::SetEvaluationParameters(parameters);
	}
	void SimpleEvaluatorDelegate::Precompute() {
		setReferenceLocationAndPrecompute(instance, x, y, radius);
	}
	void SimpleEvaluatorDelegate::Evaluate(IRasterizingInterface &rasterizingInterface) {
		evaluate(instance, (ImCApi::IRasterizingInterface *)&rasterizingInterface);
	}
}