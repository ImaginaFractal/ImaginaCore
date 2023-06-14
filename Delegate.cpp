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
		//IM_GET_OUTPUT_DESCRIPTOR_IMPL(Output, Value);
	}
	void LowPrecisionEvaluatorDelegate::SetEvaluationParameters(const StandardEvaluationParameters &parameters) {
		setEvaluationParameters(instance, (StandardEvaluationParameters *)&parameters);
	}
	void LowPrecisionEvaluatorDelegate::Evaluate(IRasterizingInterface &rasterizingInterface) {
		evaluate(instance, (ImCApi::IRasterizingInterface *)&rasterizingInterface);
	}
}