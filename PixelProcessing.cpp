#include "PixelProcessing.h"

namespace Imagina {
	im_export const FieldDescriptor *PixelDataDescriptor::FindField(std::string_view name) const {
		for (size_t i = 0; i < FieldCount; i++) {
			if (Fields[i].Name == name) {
				return &Fields[i];
			}
		}

		return nullptr;
	}
}