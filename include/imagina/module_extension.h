#pragma once

#include "module.h"

namespace Imagina {
	class IModuleExtension {
	public:
		virtual const ModuleInfo *GetNextModule() = 0;
	};
}