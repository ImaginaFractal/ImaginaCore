#include <imagina/module.h>
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>
#include <filesystem>

#include <imagina/platform_dependent.h>
#include <imagina/module_extension.h>

// Builtin modules
#include <imagina/basic_pixel_manager.h>
#include <imagina/imp_lite.h>

namespace filesystem = std::filesystem;

namespace Imagina {
	struct Component {
		ComponentInfo Info;
		size_t ModuleID;
		//std::string_view ModuleID;
		//Module *Module;
		IAny Create();
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

	IAny Component::Create() {
		return Info.Create(Info.Name);
	}

	ComponentInfo BuiltinComponents[]{
		{
			.Name = "BasicPixelManager",
			.DisplayName = "Basic Pixel Manager",
			.Create = [](const char *)->IAny { return IPixelManager(*new BasicPixelManager); },
			.Type = ComponentType::PixelManager,
		},
		{
			.Name = "IMPLite",
			.DisplayName = "IMP Lite",
			.Create = [](const char *)->IAny { return IAny((void *)&IMPLite); },
			.Type = ComponentType::MultiPrecision,
		},
	};

	ModuleInfo BuiltinModule("Imagina", "Imagina", BuiltinComponents);

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

	void Imagina_LoadBuiltinComponents() {
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

	bool Imagina_LoadModule(const char *filename) {
		return LoadModule(LoadLibrary(filename));
	}

	bool LoadModule(filesystem::path path) {
		return LoadModule(LoadLibrary(path));
	}

	bool Imagina_LoadModules(const char8_t *path, size_t pathLength, const char8_t *extension, size_t extensionLength) {
		filesystem::path Path = std::u8string_view(path, pathLength);
		filesystem::path Extension = std::u8string_view(extension, extensionLength);

		size_t loadedCount = 0;

		for (const auto &entry : filesystem::directory_iterator(Path)) {
			if (entry.is_regular_file() && entry.path().extension() == Extension) {
				loadedCount += LoadModule(entry);
			}
		}

		if (loadedCount == 0) return false;

		auto iterator = ComponentLists.find(ComponentType::ModuleExtension);
		if (iterator == ComponentLists.end()) return true;

		const std::vector<size_t> &ModuleExtensionList = iterator->second;
		for (size_t i = 0; i < ModuleExtensionList.size(); i++) {
			size_t id = ModuleExtensionList[i];

			IModuleExtension *moduleExtension = (IModuleExtension *)(void *)Components[id].Create(); // FIXME: release
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
	const size_t *Imagina_GetComponentList(ComponentType type, size_t *count) {
		const std::vector<size_t> *list = GetComponentList(type);
		if (!list) {
			count = 0;
			return nullptr;
		}
		*count = list->size();
		return list->data();
	}

	size_t Imagina_GetComponent_Name(const char *fullName, size_t length) {
		auto iterator = ComponentMap.find(std::string(fullName, length));
		if (iterator == ComponentMap.end()) return 0;
		return iterator->second;
	}
	size_t Imagina_GetComponent_Type(ComponentType type) {
		const std::vector<size_t> *list = GetComponentList(type);
		if (!list || list->empty()) return 0;

		return (*list)[list->size() - 1];
	}

	const ComponentInfo *Imagina_GetComponentInfo(size_t id) {
		assert(id != 0 && id < Components.size());
		return &Components[id].Info;
	}

	IAny Imagina_CreateComponent_Id(size_t id) {
		assert(id != 0 && id < Components.size());
		return Components[id].Create();
	}

	IAny Imagina_CreateComponent_Name(const char *fullName, size_t length) {
		return CreateComponent(Imagina_GetComponent_Name(fullName, length));
	}

	IAny Imagina_CreateComponent_Type(ComponentType type) { 
		return CreateComponent(Imagina_GetComponent_Type(type));
	}
}