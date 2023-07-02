#include "module"
#include "platform_dependent"
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>

namespace Imagina {
	std::vector<Module> Modules = { {} };
	std::vector<Component> Components = { {} };
	std::unordered_map<std::string_view, size_t> ModuleMap;
	std::unordered_map<std::string, size_t> ComponentMap;

	std::vector<size_t> ComponentLists[32];
	//std::unordered_map<std::string_view, Module> Modules;
	//std::unordered_map<std::string, Component> Components;

	// TODO: Validations
	bool LoadModule(const char *filename) {
		void *handle = LoadLibrary(filename);
		if (!handle) return false;

		pImGetModuleInfo GetModuleInfo = (pImGetModuleInfo)GetSymbol(handle, "ImGetModuleInfo");
		if (!GetModuleInfo) {
			UnloadLibrary(handle);
			return false;
		}

		pImInit Init = (pImInit)GetSymbol(handle, "ImInit");
		if (Init) Init();

		bool added;
		ModuleInfo *moduleInfo = GetModuleInfo();
		size_t moduleID = Modules.size();
		std::tie(std::ignore, added) = ModuleMap.try_emplace(moduleInfo->Name, moduleID);

		if (!added) {
			UnloadLibrary(handle);
			return false;
		}

		Modules.push_back({ *moduleInfo,handle, Components.size(), moduleInfo->ComponentCount });

		std::string prefix = moduleInfo->Name;
		prefix += '.';
		for (size_t i = 0; i < moduleInfo->ComponentCount; i++) {
			const ComponentInfo &componentInfo = moduleInfo->Components[i];
			size_t componentID = Components.size();
			ComponentMap.try_emplace(prefix + componentInfo.Name, componentID);
			Components.push_back({ componentInfo, moduleID });

			for (size_t j = 0; j < 32; j++) {
				if (!((uint32_t)componentInfo.Type & (1 << j))) continue;
				ComponentLists[j].push_back(componentID);
			}
		}

		//iterator->second.Info = *moduleInfo;
		//iterator->second.Handle = handle;
		//Component *components = new Component[moduleInfo->ComponentCount];
		//iterator->second.ComponentCount = moduleInfo->ComponentCount;
		//iterator->second.Components = components;
		return true;
	}
	Component *GetComponent(std::string_view fullName) {
		auto iterator = ComponentMap.find(std::string(fullName));
		if (iterator == ComponentMap.end()) return nullptr;
		return &Components[iterator->second];
	}
	Component *GetComponent(ComponentTypeIndex type) {
		assert((uint32_t)type < 32);

		std::vector<size_t> &list = ComponentLists[(uint32_t)type];
		if (list.empty()) return nullptr;

		size_t id = list[list.size() - 1];
		return &Components[id];
	}
}