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


#include <units/isq/si/si.h>

namespace {

using namespace units;
using namespace units::isq;

static_assert(Length<si::length<si::metre>>);
static_assert(!Length<si::time<si::second>>);

static_assert(Mass<si::mass<si::kilogram>>);
static_assert(!Mass<si::time<si::second>>);

static_assert(Time<si::time<si::second>>);
static_assert(!Time<si::length<si::metre>>);

static_assert(ElectricCurrent<si::electric_current<si::ampere>>);
static_assert(!ElectricCurrent<si::time<si::second>>);

static_assert(ThermodynamicTemperature<si::thermodynamic_temperature<si::kelvin>>);
static_assert(!ThermodynamicTemperature<si::time<si::second>>);

static_assert(AmountOfSubstance<si::amount_of_substance<si::mole>>);
static_assert(!AmountOfSubstance<si::time<si::second>>);

static_assert(LuminousIntensity<si::luminous_intensity<si::candela>>);
static_assert(!LuminousIntensity<si::time<si::second>>);

static_assert(Frequency<si::frequency<si::hertz>>);
static_assert(!Frequency<si::time<si::second>>);

static_assert(Area<si::area<si::square_metre>>);
static_assert(!Area<si::time<si::second>>);

static_assert(Volume<si::volume<si::cubic_metre>>);
static_assert(!Volume<si::area<si::square_metre>>);
#if UNITS_DOWNCAST_MODE == 0
static_assert(
  Volume<quantity<unknown_dimension<exponent<si::dim_length, 3>>, unknown_coherent_unit<exponent<si::dim_length, 3>>>>);
#endif

static_assert(Speed<si::speed<si::metre_per_second>>);
static_assert(!Speed<si::time<si::second>>);

static_assert(Acceleration<si::acceleration<si::metre_per_second_sq>>);
static_assert(!Acceleration<si::time<si::second>>);
#if UNITS_DOWNCAST_MODE == 0
static_assert(Acceleration<quantity<unknown_dimension<exponent<si::dim_length, 1>, exponent<si::dim_time, -2>>,
                                    unknown_coherent_unit<exponent<si::dim_length, 1>, exponent<si::dim_time, -2>>>>);
#endif

static_assert(Force<si::force<si::newton>>);
static_assert(!Force<si::time<si::second>>);
#if UNITS_DOWNCAST_MODE == 0
// static_assert(Force<quantity<unknown_dimension<exponent<si::dim_length, 1>, exponent<si::dim_time, -2>,
// exponent<si::dim_mass, 1>>, unknown_coherent_unit<exponent<si::dim_length, 1>,
// exponent<si::dim_time, -2>>>);
#endif

static_assert(Energy<si::energy<si::joule>>);
static_assert(!Energy<si::time<si::second>>);
#if UNITS_DOWNCAST_MODE == 0
// static_assert(Energy<quantity<unknown_dimension<exponent<si::dim_mass, 1>, exponent<si::dim_length, 2>,
// exponent<si::dim_time, -3>>, unknown_coherent_unit<exponent<si::dim_mass, 1>,
// exponent<si::dim_length, 2>>>);
#endif

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

static_assert(MagneticFlux<si::magnetic_flux<si::weber>>);
static_assert(!MagneticFlux<si::time<si::second>>);

static_assert(Inductance<si::inductance<si::henry>>);
static_assert(!Inductance<si::time<si::second>>);

static_assert(Conductance<si::conductance<si::siemens>>);
static_assert(!Conductance<si::time<si::second>>);

// TODO Add when downcasting issue is solved
// static_assert(Radioactivity<si::radioactivity<si::becquerel>>);
// static_assert(!Radioactivity<si::time<si::second>>);

static_assert(CatalyticActivity<si::catalytic_activity<si::katal>>);
static_assert(!CatalyticActivity<si::time<si::second>>);

static_assert(AbsorbedDose<si::absorbed_dose<si::gray>>);
static_assert(!AbsorbedDose<si::time<si::second>>);

static_assert(CurrentDensity<si::current_density<si::ampere_per_metre_sq>>);
static_assert(!CurrentDensity<si::time<si::second>>);

static_assert(Concentration<si::concentration<si::mol_per_metre_cub>>);
static_assert(!Concentration<si::time<si::second>>);

static_assert(Luminance<si::luminance<si::candela_per_metre_sq>>);
static_assert(!Luminance<si::time<si::second>>);

static_assert(DynamicViscosity<si::dynamic_viscosity<si::pascal_second>>);
static_assert(!DynamicViscosity<si::time<si::second>>);

static_assert(HeatCapacity<si::heat_capacity<si::joule_per_kelvin>>);
static_assert(!HeatCapacity<si::time<si::second>>);

static_assert(SpecificHeatCapacity<si::specific_heat_capacity<si::joule_per_kilogram_kelvin>>);
static_assert(!SpecificHeatCapacity<si::time<si::second>>);

static_assert(MolarHeatCapacity<si::molar_heat_capacity<si::joule_per_mole_kelvin>>);
static_assert(!MolarHeatCapacity<si::time<si::second>>);

static_assert(ThermalConductivity<si::thermal_conductivity<si::watt_per_metre_kelvin>>);
static_assert(!ThermalConductivity<si::time<si::second>>);

// TODO Add when downcasting issue is solved
// static_assert(EnergyDensity<si::energy_density<si::joule_per_metre_cub>>);
// static_assert(!EnergyDensity<si::time<si::second>>);

static_assert(ElectricFieldStrength<si::electric_field_strength<si::volt_per_metre>>);
static_assert(!ElectricFieldStrength<si::time<si::second>>);

static_assert(ChargeDensity<si::charge_density<si::coulomb_per_metre_cub>>);
static_assert(!ChargeDensity<si::time<si::second>>);

static_assert(SurfaceChargeDensity<si::surface_charge_density<si::coulomb_per_metre_sq>>);
static_assert(!SurfaceChargeDensity<si::time<si::second>>);

static_assert(Permittivity<si::permittivity<si::farad_per_metre>>);
static_assert(!Permittivity<si::time<si::second>>);

static_assert(Permeability<si::permeability<si::henry_per_metre>>);
static_assert(!Permeability<si::time<si::second>>);

static_assert(MolarEnergy<si::molar_energy<si::joule_per_mole>>);
static_assert(!MolarEnergy<si::time<si::second>>);

}  // namespace
