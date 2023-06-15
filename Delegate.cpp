#include "delegate"
#include "output_descriptor_helper"

namespace Imagina {
	LowPrecisionEvaluatorDelegate::~LowPrecisionEvaluatorDelegate() {
		release(instance);
	}
	const PixelDataDescriptor *LowPrecisionEvaluatorDelegate::GetOutputDescriptor() {
		if (!outputDescriptor) {
			outputDescriptor = (PixelDataDescriptor *)getOutputDescriptor(instance);
		}
		return outputDescriptor;
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
	const PixelDataDescriptor *SimpleEvaluatorDelegate::GetOutputDescriptor() {
		if (!outputDescriptor) {
			outputDescriptor = (PixelDataDescriptor *)getOutputDescriptor(instance);
		}
		return outputDescriptor;
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