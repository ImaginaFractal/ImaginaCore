#pragma once

#include <stdint.h>

#ifdef __cplusplus
namespace Imagina {
#endif

typedef int64_t MPExpInt;
typedef int64_t MPBCInt;
typedef uint64_t MPBCUint;

#ifdef __cplusplus
struct MPReal;
#else
typedef struct _MPReal MPReal;
#endif

struct _MultiPrecisionMutable {
	const char *Name;

	void (*InitContent)(MPReal *, MPBCUint); // Do not use directly, use Init instead.
	void (*InitContentCopy)(MPReal *, const MPReal *); // Do not use directly, use InitCopy instead.

	void (*ClearContent)(MPReal *); // Do not use directly, use Clear instead.

	MPBCUint (*GetPrecision)(const MPReal *);
	void (*SetPrecision)(MPReal *, MPBCUint);

	void (*Set)(MPReal *, const MPReal *);
	void (*Copy)(MPReal *, const MPReal *); // Set value and precision
	void (*SetDouble)(MPReal *, double);
	double (*ToDouble)(const MPReal *);

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