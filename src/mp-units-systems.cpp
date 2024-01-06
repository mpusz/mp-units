module;

#include <mp-units/bits/core-gmf.h>
#include <mp-units/quantity_spec_macro.h>

export module mp_units.systems;

export import mp_units.core;

export
{
// systems
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

// utility
#include <mp-units/chrono.h>
#include <mp-units/compare.h>
#include <mp-units/math.h>
#include <mp-units/random.h>
}
