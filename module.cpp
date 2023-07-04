#include "module"
#include "platform_dependent"
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>
#include <filesystem>

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
		void *Create() {
			return Info.Create(Info.Name);
		}
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
	bool AddModule(ModuleInfo *moduleInfo, void *handle) {
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
		return loadedCount != 0;
	}



	Component *GetComponent(std::string_view fullName) {
		auto iterator = ComponentMap.find(std::string(fullName));
		if (iterator == ComponentMap.end()) return nullptr;
		return &Components[iterator->second];
	}
	Component *GetComponent(ComponentType type) {
		auto iterator = ComponentLists.find(type);
		if (iterator == ComponentLists.end()) return nullptr;
		std::vector<size_t> &list = iterator->second;
		if (list.empty()) return nullptr;

		size_t id = list[list.size() - 1];
		return &Components[id];
	}
	void *CreateComponent(std::string_view fullName) {
		Component *component = GetComponent(fullName);
		return component ? component->Create() : nullptr;
	}
	void *CreateComponent(ComponentType type) {
		Component *component = GetComponent(type);
		return component ? component->Create() : nullptr;
	}
}