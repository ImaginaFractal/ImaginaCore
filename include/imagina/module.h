#pragma once

#include <string_view>
#include <stddef.h>
#include <stdint.h>
#include <iterator>
#include "declarations.h"
#include "interface.h"

#include <imagina/interface/evaluator.h>
#include <imagina/interface/engine.h>

#ifdef _WIN32
#define IM_DEFAULT_MODULE_EXTENSION = u8".dll"
#elif defined(__unix__) || defined(__linux__) || defined(__FreeBSD__)
#define IM_DEFAULT_MODULE_EXTENSION = u8".so"
#elif defined(__APPLE__)
#define IM_DEFAULT_MODULE_EXTENSION = u8".dylib";
#else
#define IM_DEFAULT_MODULE_EXTENSION
#endif

namespace Imagina {
	using pCreateComponent = IAny (*)(const char *ID);
	using pReleaseComponent = void (*)(const char *ID, void *instance);
	
	enum class ComponentType : uint32_t {
		None			= 0,
		Evaluator		= 1,
		Engine			= 2,
		PixelProcessor	= 3,
		PixelManager	= 16,
		Controller		= 20,
		MultiPrecision	= 28, // TODO: Create dedicated API for multi-precision. It should be a part of im-numerics.
		ModuleExtension	= 31,
	};

	enum class ComponentFlag : uint32_t {
		None = 0,
	};

	struct ComponentInfo {
		const char *Name = nullptr;
		const char *DisplayName = nullptr;
		pCreateComponent Create = nullptr;
		pReleaseComponent Release = nullptr;
		ComponentType Type;
		ComponentFlag Flags;

		template<IEvaluatorImpl T>
		static constexpr ComponentInfo Evaluator(const char *Name, const char *DisplayName) {
			return {
				.Name = Name,
				.DisplayName = DisplayName,
				.Create = [](const char *)->Imagina::IAny { return Imagina::IEvaluator(new T); },
				.Type = ComponentType::Evaluator,
			};
		}

		template<IEngineImpl T>
		static constexpr ComponentInfo Engine(const char *Name, const char *DisplayName) {
			return {
				.Name = Name,
				.DisplayName = DisplayName,
				.Create = [](const char *)->Imagina::IAny { return Imagina::IEngine(new T); },
				.Type = ComponentType::Engine,
			};
		}
	};

	struct ModuleInfo {
		const char *Name;
		const char *DisplayName;
		size_t ComponentCount;
		const ComponentInfo *Components;

		template <size_t componentCount>
		constexpr ModuleInfo(const char *name, const char *displayName, const ComponentInfo(&components)[componentCount]) :
			Name(name), DisplayName(displayName), ComponentCount(componentCount), Components(components) {}

		constexpr ModuleInfo(const char *name, const char *displayName, const ComponentInfo &component) :
			Name(name), DisplayName(displayName), ComponentCount(1), Components(&component) {}
	};

	using pImInit = void (*)();
	using pImGetModuleInfo = const ModuleInfo *(*)();


	inline uint32_t operator&(ComponentFlag a, ComponentFlag b) { return uint32_t(a) & uint32_t(b); }

	template<typename T> inline constexpr ComponentType TypeToComponentType = ComponentType::None;

	template<> inline constexpr ComponentType TypeToComponentType<IEvaluator>		= ComponentType::Evaluator;
	template<> inline constexpr ComponentType TypeToComponentType<IEngine>			= ComponentType::Engine;
	template<> inline constexpr ComponentType TypeToComponentType<IPixelProcessor>	= ComponentType::PixelProcessor;
	template<> inline constexpr ComponentType TypeToComponentType<IPixelManager>	= ComponentType::PixelManager;
	template<> inline constexpr ComponentType TypeToComponentType<IController>		= ComponentType::Controller;
	template<> inline constexpr ComponentType TypeToComponentType<multi_precision>	= ComponentType::MultiPrecision;

	imcore_api void Imagina_LoadBuiltinComponents();

	imcore_api bool Imagina_LoadModule(const char *filename);
	imcore_api bool Imagina_LoadModules(const char8_t *path, size_t pathLength, const char8_t *extension, size_t extensionLength);

	// Return value is invalidated when new a component is added
	imcore_api const size_t *Imagina_GetComponentList(ComponentType type, size_t *count);

	imcore_api size_t Imagina_GetComponent_Name(const char *fullName, size_t length);
	imcore_api size_t Imagina_GetComponent_Type(ComponentType type);

	imcore_api const ComponentInfo *Imagina_GetComponentInfo(size_t id);

	imcore_api IAny Imagina_CreateComponent_Id(size_t id);
	imcore_api IAny Imagina_CreateComponent_Name(const char *fullName, size_t length);
	imcore_api IAny Imagina_CreateComponent_Type(ComponentType type);



	inline void LoadBuiltinComponents()											{ Imagina_LoadBuiltinComponents(); }

	inline bool LoadModule(const char *filename)								{ return Imagina_LoadModule(filename); }
	inline bool LoadModules(std::u8string_view path, std::u8string_view extension IM_DEFAULT_MODULE_EXTENSION) {
		return Imagina_LoadModules(path.data(), path.length(), extension.data(), extension.length());
	}

	// Return value is invalidated when new a component is added
	inline const size_t *GetComponentList(ComponentType type, size_t &count)	{ return Imagina_GetComponentList(type, &count); }

	inline size_t GetComponent(std::string_view fullName)						{ return Imagina_GetComponent_Name(fullName.data(), fullName.length()); }
	inline size_t GetComponent(ComponentType type)								{ return Imagina_GetComponent_Type(type); }

	inline const ComponentInfo &GetComponentInfo(size_t id)						{ return *Imagina_GetComponentInfo(id); }

	inline IAny CreateComponent(size_t id)										{ return Imagina_CreateComponent_Id(id); }
	inline IAny CreateComponent(std::string_view fullName)						{ return Imagina_CreateComponent_Name(fullName.data(), fullName.length()); }
	inline IAny CreateComponent(ComponentType type)								{ return Imagina_CreateComponent_Type(type); }

	//template<typename T>
	//inline T CreateComponent() {
	//	return (T)CreateComponent(TypeToComponentType<T>);
	//}
}