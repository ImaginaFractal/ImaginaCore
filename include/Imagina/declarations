#pragma once

#ifdef _WIN32
#ifdef IM_BUILD_CORE
#define im_export __declspec(dllexport)
#else
#define im_export __declspec(dllimport)
#endif
#else
#define im_export
#endif

namespace Imagina {
	class IController;

	class IPixelManager;
	class IGpuTextureManager;
	class IGpuPixelManager;
	class IRasterizingInterface;

	class IRasterizer;
	class IEvaluator;

	class ILocationManager;

	class IFractalContext;

	class PixelPipeline;
}