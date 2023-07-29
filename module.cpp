#include "module"
#include "platform_dependent"
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>
#include <filesystem>

#include "imagina.h"
#include "proxy"
#include "module_extension"

// Builtin modules
//#include "location_manager"
#include "basic_pixel_manager"
#include "imp_lite"

namespace filesystem = std::filesystem;

namespace Imagina {
	struct Component {
		ComponentInfo Info;
		size_t ModuleID;
		//std::string_view ModuleID;
		//Module *Module;
		void *Create();
	};
	struct Module {
		ModuleInfo Info;
		void *Handle;
		size_t FirstComponentID;
		size_t ComponentCount;
		//Component *Components;
	};

	std::vector<Module> Modules = { {} };
	std::vector<Component> Components = { {} };
	std::unordered_map<std::string_view, size_t> ModuleMap;
	std::unordered_map<std::string, size_t> ComponentMap;

	std::unordered_map<ComponentType, std::vector<size_t>> ComponentLists;
	//std::unordered_map<std::string_view, Module> Modules;
	//std::unordered_map<std::string, Component> Components;

	void *Component::Create() {
#if false
		if (Info.Flags & ComponentFlag::UseCApi) {
			if (!Info.FunctionTable) return nullptr;
			switch (Info.ProxyType) {
				case ProxyType::LowPrecisionEvaluator: {
					auto functionTable = (ImCApi::LowPrecisionEvaluatorFunctionTable *)Info.FunctionTable;
					return new LowPrecisionEvaluatorProxy((ImCApi::IEvaluator *)Info.Create(Info.Name), nullptr/*FIXME*/, functionTable->getOutputInfo, functionTable->setEvaluationParameters, functionTable->evaluate);
				}
				case ProxyType::SimpleEvaluator: {
					auto functionTable = (ImCApi::SimpleEvaluatorFunctionTable *)Info.FunctionTable;
					return new SimpleEvaluatorProxy((ImCApi::IEvaluator *)Info.Create(Info.Name), nullptr/*FIXME*/, functionTable->getOutputInfo, functionTable->setEvaluationParameters, functionTable->setReferenceLocationAndPrecompute, functionTable->evaluate);
				}
				default: return nullptr;
			}
		} else {
			return Info.Create(Info.Name);
		}
#endif
		return Info.Create(Info.Name);
	}

	ComponentInfo BuiltinComponents[]{
		{
			.Name = "BasicPixelManager",
			.DisplayName = "Basic Pixel Manager",
			.Create = [](const char *)->void *{ return new BasicPixelManager; },
			.Type = ComponentType::PixelManager,
		},
		{
			.Name = "IMPLite",
			.DisplayName = "IMP Lite",
			.Create = [](const char *)->void *{ return (void *)&IMPLite; },
			.Type = ComponentType::MultiPrecision,
		},
	};

	ModuleInfo BuiltinModule{
		.Name = "Imagina",
		.DisplayName = "Imagina",
		.ComponentCount = 2,
		.Components = BuiltinComponents,
	};

	// TODO: Validations
	bool AddModule(const ModuleInfo *moduleInfo, void *handle) {
		size_t moduleID = Modules.size();
		bool added = ModuleMap.try_emplace(moduleInfo->Name, moduleID).second;

		if (!added) return false;

		Modules.push_back({ *moduleInfo, handle, Components.size(), moduleInfo->ComponentCount });

		std::string prefix = moduleInfo->Name;
		prefix += '.';
		for (size_t i = 0; i < moduleInfo->ComponentCount; i++) {
			const ComponentInfo &componentInfo = moduleInfo->Components[i];
			size_t componentID = Components.size();
			ComponentMap.try_emplace(prefix + componentInfo.Name, componentID);
			Components.push_back({ componentInfo, moduleID });

			ComponentLists[componentInfo.Type].push_back(componentID);
		}

		return true;
	}

	im_export void LoadBuiltinComponents() {
		AddModule(&BuiltinModule, nullptr);
	}

	bool LoadModule(void *handle) {
		if (!handle) return false;

		pImGetModuleInfo GetModuleInfo = (pImGetModuleInfo)GetSymbol(handle, "ImGetModuleInfo");
		if (!GetModuleInfo) {
			UnloadLibrary(handle);
			return false;
		}

		pImInit Init = (pImInit)GetSymbol(handle, "ImInit");
		if (Init) Init();

		if (!AddModule(GetModuleInfo(), handle)) {
			UnloadLibrary(handle);
			return false;
		}
		return true;
	}

	bool LoadModule(const char *filename) {
		return LoadModule(LoadLibrary(filename));
	}

	bool LoadModule(filesystem::path path) {
		return LoadModule(LoadLibrary(path));
	}

	bool LoadModules(const char *path, const char *extension) {
		size_t loadedCount = 0;
		for (const auto &entry : filesystem::directory_iterator(path)) {
			if (!entry.is_regular_file() || entry.path().extension() != extension) continue;
			loadedCount += LoadModule(entry);
		}

		if (loadedCount == 0) return false;

		auto iterator = ComponentLists.find(ComponentType::ModuleExtension);
		if (iterator == ComponentLists.end()) return true;

		const std::vector<size_t> &ModuleExtensionList = iterator->second;
		for (size_t i = 0; i < ModuleExtensionList.size(); i++) {
			size_t id = ModuleExtensionList[i];

			IModuleExtension *moduleExtension = (IModuleExtension *)Components[id].Create(); // FIXME: release
			while (const ModuleInfo *info = moduleExtension->GetNextModule()) {
				AddModule(info, nullptr);
			}
		}
		return true;
	}



	const std::vector<size_t> *GetComponentList(ComponentType type) {
		auto iterator = ComponentLists.find(type);
		if (iterator == ComponentLists.end()) return nullptr;
		return &iterator->second;
	}
	const size_t *GetComponentList(ComponentType type, size_t &count) {
		const std::vector<size_t> *list = GetComponentList(type);
		if (!list) {
			count = 0;
			return nullptr;
		}
		count = list->size();
		return list->data();
	}

	size_t GetComponent(std::string_view fullName) {
		auto iterator = ComponentMap.find(std::string(fullName));
		if (iterator == ComponentMap.end()) return 0;
		return iterator->second;
	}
	size_t GetComponent(ComponentType type) {
		const std::vector<size_t> *list = GetComponentList(type);
		if (!list || list->empty()) return 0;

		return (*list)[list->size() - 1];
	}

	const ComponentInfo &GetComponentInfo(size_t id) {
		assert(id != 0 && id < Components.size());
		return Components[id].Info;
	}

	void *CreateComponent(size_t id) {
		assert(id != 0 && id < Components.size());
		return Components[id].Create();
	}
	void *CreateComponent(std::string_view fullName)	{ return CreateComponent(GetComponent(fullName)); }
	void *CreateComponent(ComponentType type)			{ return CreateComponent(GetComponent(type)); }
}