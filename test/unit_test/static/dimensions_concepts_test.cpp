// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <units/physical/si/acceleration.h>
#include <units/physical/si/area.h>
#include <units/physical/si/capacitance.h>
#include <units/physical/si/current.h>
#include <units/physical/si/electric_charge.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/force.h>
#include <units/physical/si/frequency.h>
#include <units/physical/si/length.h>
#include <units/physical/si/luminous_intensity.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/power.h>
#include <units/physical/si/pressure.h>
#include <units/physical/si/substance.h>
#include <units/physical/si/surface_tension.h>
#include <units/physical/si/temperature.h>
#include <units/physical/si/time.h>
#include <units/physical/si/velocity.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/volume.h>
#include <units/physical/si/magnetic_induction.h>

namespace {

using namespace units;

static_assert(Length<si::length<si::metre>>);
static_assert(!Length<si::time<si::second>>);

static_assert(Mass<si::mass<si::kilogram>>);
static_assert(!Mass<si::time<si::second>>);

static_assert(Time<si::time<si::second>>);
static_assert(!Time<si::length<si::metre>>);

static_assert(Current<si::current<si::ampere>>);
static_assert(!Current<si::time<si::second>>);

static_assert(Temperature<si::temperature<si::kelvin>>);
static_assert(!Temperature<si::time<si::second>>);

static_assert(Substance<si::substance<si::mole>>);
static_assert(!Substance<si::time<si::second>>);

static_assert(LuminousIntensity<si::luminous_intensity<si::candela>>);
static_assert(!LuminousIntensity<si::time<si::second>>);

static_assert(Frequency<si::frequency<si::hertz>>);
static_assert(!Frequency<si::time<si::second>>);

static_assert(Area<si::area<si::square_metre>>);
static_assert(!Area<si::time<si::second>>);

static_assert(Volume<si::volume<si::cubic_metre>>);
static_assert(!Volume<si::time<si::second>>);

static_assert(Velocity<si::velocity<si::metre_per_second>>);
static_assert(!Velocity<si::time<si::second>>);

static_assert(Acceleration<si::acceleration<si::metre_per_second_sq>>);
static_assert(!Acceleration<si::time<si::second>>);

static_assert(Force<si::force<si::newton>>);
static_assert(!Force<si::time<si::second>>);

static_assert(Energy<si::energy<si::joule>>);
static_assert(!Energy<si::time<si::second>>);

static_assert(Power<si::power<si::watt>>);
static_assert(!Power<si::time<si::second>>);

static_assert(Voltage<si::voltage<si::volt>>);
static_assert(!Voltage<si::time<si::second>>);

static_assert(ElectricCharge<si::electric_charge<si::coulomb>>);
static_assert(!ElectricCharge<si::time<si::second>>);

static_assert(Capacitance<si::capacitance<si::farad>>);
static_assert(!Capacitance<si::time<si::second>>);

static_assert(SurfaceTension<si::surface_tension<si::newton_per_metre>>);
static_assert(!SurfaceTension<si::time<si::second>>);

static_assert(Pressure<si::pressure<si::pascal>>);
static_assert(!Pressure<si::time<si::second>>);

static_assert(MagneticInduction<si::magnetic_induction<si::tesla>>);
static_assert(!MagneticInduction<si::time<si::second>>);

}
