#pragma once

#ifdef _WIN32
#define im_api extern "C" __declspec(dllexport)
#ifdef IM_BUILD_CORE
#define imcore_api extern "C" __declspec(dllexport)
#define im_export __declspec(dllexport) // deprecated
#else
#define imcore_api extern "C" __declspec(dllimport)
#define im_export __declspec(dllimport) // deprecated
#endif
#else
#define im_api extern "C"
#define imcore_api extern "C"
#define im_export // deprecated
#endif

#define if_likely(...) if (__VA_ARGS__) [[likely]]
#define if_unlikely(...) if (__VA_ARGS__) [[unlikely]]

namespace Imagina {
	class IModuleExtension;

	class IController;

	class IPixelManager;
	class IGpuTextureManager;

	class IEvaluator;
	class IEngine;

	class ILocationManager;

	class IFractalContext;

	class IPixelProcessor;
	class PixelPipeline;

	inline namespace Numerics {
		struct multi_precision_mutable;
		using multi_precision = const multi_precision_mutable;
	}
}