#pragma once

#ifndef im_export
#ifdef _WIN32
#ifdef IM_BUILD_CORE
#define im_export __declspec(dllexport)
#else
#define im_export __declspec(dllimport)
#endif
#else
#define im_export
#endif
#endif

#ifdef __cplusplus
#define IM_C_STRUCT_ALIAS(x) struct x
#else
#define IM_C_STRUCT_ALIAS(x) typedef struct _##x x
#endif

#ifdef __cplusplus
namespace ImCApi {
#endif

typedef struct _IController IController;

typedef struct _IPixelManager IPixelManager;
typedef struct _IGpuTextureManager IGpuTextureManager;
typedef struct _IGpuPixelManager IGpuPixelManager;
typedef struct _IRasterizingInterface IRasterizingInterface;

typedef struct _IRasterizer IRasterizer;
typedef struct _IEvaluator IEvaluator;

typedef struct _ILocationManager ILocationManager;

typedef struct _IFractalContext IFractalContext;


#ifdef __cplusplus
}
#endif