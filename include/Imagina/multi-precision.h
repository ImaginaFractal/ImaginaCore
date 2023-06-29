#pragma once

#include <stdint.h>
#include "floating_point.h"

#ifdef __cplusplus
namespace Imagina {
#endif

typedef intptr_t MPExpInt;
typedef intptr_t MPBCInt;
typedef uintptr_t MPBCUInt;

#ifdef __cplusplus
struct MPReal;
#else
typedef struct _MPReal MPReal;
#endif

typedef void		(*pMultiPrecision_InitContent)		(MPReal *, MPBCUInt); // Do not use directly, use Init instead.
typedef void		(*pMultiPrecision_InitContentCopy)	(MPReal *, const MPReal *); // Do not use directly, use InitCopy instead.

typedef void		(*pMultiPrecision_ClearContent)		(MPReal *); // Do not use directly, use Clear instead.

typedef MPBCUInt	(*pMultiPrecision_GetPrecision)		(const MPReal *);
typedef void		(*pMultiPrecision_SetPrecision)		(MPReal *, MPBCUInt);

typedef void		(*pMultiPrecision_Set)				(MPReal *, const MPReal *);
typedef void		(*pMultiPrecision_Copy)				(MPReal *, const MPReal *); // Set value and precision
typedef void		(*pMultiPrecision_SetDouble)		(MPReal *, double);
typedef void		(*pMultiPrecision_SetFloatF64eI64)	(MPReal *, FloatF64eI64);

typedef double		(*pMultiPrecision_GetDouble)		(const MPReal *);
typedef FloatF64eI64(*pMultiPrecision_GetFloatF64eI64)	(const MPReal *);

typedef void		(*pMultiPrecision_Add)				(MPReal *, const MPReal *, const MPReal *);
typedef void		(*pMultiPrecision_Sub)				(MPReal *, const MPReal *, const MPReal *);
typedef void		(*pMultiPrecision_Mul)				(MPReal *, const MPReal *, const MPReal *);
typedef void		(*pMultiPrecision_Div)				(MPReal *, const MPReal *, const MPReal *);

struct _MultiPrecisionMutable {
	const char *Name;

	pMultiPrecision_InitContent		InitContent;
	pMultiPrecision_InitContentCopy	InitContentCopy;

	pMultiPrecision_ClearContent	ClearContent;

	pMultiPrecision_GetPrecision	GetPrecision;
	pMultiPrecision_SetPrecision	SetPrecision;

	pMultiPrecision_Set				Set;
	pMultiPrecision_Copy			Copy;
	pMultiPrecision_SetDouble		SetDouble;
	pMultiPrecision_SetFloatF64eI64	SetFloatF64eI64;

	pMultiPrecision_GetDouble		GetDouble;
	pMultiPrecision_GetFloatF64eI64	GetFloatF64eI64;

	pMultiPrecision_Add				Add;
	pMultiPrecision_Sub				Sub;
	pMultiPrecision_Mul				Mul;
	pMultiPrecision_Div				Div;
};

#ifndef __cplusplus
typedef struct _MultiPrecisionMutable MultiPrecisionMutable;
typedef const struct MultiPrecisionMutable MultiPrecision;

struct _MPReal {
	const void *PlaceHolder[7]; // This is for saving the contents of the actual number type (like mpf_t or mpfr_t)
	MultiPrecision *const MP;
};
#endif

#ifdef __cplusplus
}
#endif