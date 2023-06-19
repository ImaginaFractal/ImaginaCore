#pragma once

#include <stdint.h>
#include "floating_point.h"

#ifdef __cplusplus
namespace Imagina {
#endif

typedef int64_t MPExpInt;
typedef int64_t MPBCInt;
typedef uint64_t MPBCUInt;

#ifdef __cplusplus
struct MPReal;
#else
typedef struct _MPReal MPReal;
#endif

struct _MultiPrecisionMutable {
	const char *Name;

	void (*InitContent)(MPReal *, MPBCUInt); // Do not use directly, use Init instead.
	void (*InitContentCopy)(MPReal *, const MPReal *); // Do not use directly, use InitCopy instead.

	void (*ClearContent)(MPReal *); // Do not use directly, use Clear instead.

	MPBCUInt (*GetPrecision)(const MPReal *);
	void (*SetPrecision)(MPReal *, MPBCUInt);

	void (*Set)(MPReal *, const MPReal *);
	void (*Copy)(MPReal *, const MPReal *); // Set value and precision
	void (*SetDouble)(MPReal *, double);
	void (*SetFloatF64eI64)(MPReal *, FloatF64eI64);

	double (*GetDouble)(const MPReal *);
	FloatF64eI64 (*GetFloatF64eI64)(const MPReal *);

	void (*Add)(MPReal *, const MPReal *, const MPReal *);
	void (*Sub)(MPReal *, const MPReal *, const MPReal *);
	void (*Mul)(MPReal *, const MPReal *, const MPReal *);
	void (*Div)(MPReal *, const MPReal *, const MPReal *);
};

#ifndef __cplusplus
typedef struct _MultiPrecisionMutable MultiPrecisionMutable;
typedef const struct MultiPrecisionMutable MultiPrecision;

struct _MPReal {
	const void *Content[7]; // This is for saving the contents of the actual number type (like mpf_t or mpfr_t)
	MultiPrecision *const MP;
};
#endif

#ifdef __cplusplus
}
#endif