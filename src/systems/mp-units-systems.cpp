module;

#include <mp-units/bits/core_gmf.h>
#include <chrono>

export module mp_units.systems;

export import mp_units.core;

#define MP_UNITS_IN_MODULE_INTERFACE

#include <mp-units/systems/angular/angular.h>
#include <mp-units/systems/cgs/cgs.h>
#include <mp-units/systems/hep/hep.h>
#include <mp-units/systems/iau/iau.h>
#include <mp-units/systems/iec80000/iec80000.h>
#include <mp-units/systems/imperial/imperial.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/isq_angle/isq_angle.h>
#include <mp-units/systems/natural/natural.h>
#include <mp-units/systems/si/si.h>
#include <mp-units/systems/typographic/typographic.h>
#include <mp-units/systems/usc/usc.h>
