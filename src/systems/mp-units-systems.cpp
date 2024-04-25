module;

#include <mp-units/bits/core_gmf.h>
#include <chrono>

export module mp_units.systems;

export import mp_units.core;

#define MP_UNITS_IN_MODULE_INTERFACE

#include <mp-units/systems/angular.h>
#include <mp-units/systems/cgs.h>
#include <mp-units/systems/hep.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/iec80000.h>
#include <mp-units/systems/imperial.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/isq_angle.h>
#include <mp-units/systems/natural.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/typographic.h>
#include <mp-units/systems/usc.h>
