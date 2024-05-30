module;

#include <mp-units/bits/core_gmf.h>

export module mp_units.core;

#define MP_UNITS_IN_MODULE_INTERFACE

#include <mp-units/compat_macros.h>
#include <mp-units/concepts.h>
#include <mp-units/framework.h>

#if MP_UNITS_HOSTED
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/ostream.h>
#include <mp-units/random.h>
#endif
