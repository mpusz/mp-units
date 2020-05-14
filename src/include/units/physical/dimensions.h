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

#pragma once

#include <units/base_dimension.h>
#include <units/bits/external/type_traits.h>
#include <units/quantity.h>
#include <units/unit.h>

namespace units::physical {

template<typename Dim, template<typename...> typename DimTemplate>
concept DimensionOf = Dimension<Dim> && is_derived_from_instantiation<Dim, DimTemplate>;

template<typename Q, template<typename...> typename DimTemplate>
concept QuantityOf = Quantity<Q> && is_derived_from_instantiation<typename Q::dimension, DimTemplate>;

// ------------------------ base dimensions -----------------------------

template<Unit U>
struct dim_length : base_dimension<"L", U> {};

template<Unit U>
struct dim_mass : base_dimension<"M", U> {};

template<Unit U>
struct dim_time : base_dimension<"T", U> {};

template<Unit U>
struct dim_electric_current : base_dimension<"I", U> {};

template<Unit U>
struct dim_thermodynamic_temperature : base_dimension<"Θ", U> {};

template<Unit U>
struct dim_substance : base_dimension<"N", U> {};

template<Unit U>
struct dim_luminous_intensity : base_dimension<"J", U> {};

template<Unit U>
struct dim_angle : base_dimension<"A", U> {};

// ------------------------ derived dimensions -----------------------------

template<typename Child, Unit U, DimensionOf<dim_time> T>
struct dim_frequency : derived_dimension<Child, U, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L>
struct dim_area : derived_dimension<Child, U, exp<L, 2>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L>
struct dim_volume : derived_dimension<Child, U, exp<L, 3>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_speed : derived_dimension<Child, U, exp<L, 1>, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_acceleration : derived_dimension<Child, U, exp<L, 1>, exp<T, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_acceleration> A>
struct dim_force : derived_dimension<Child, U, exp<M, 1>, exp<A, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_speed> V>
struct dim_momentum : derived_dimension<Child, U, exp<M, 1>, exp<V, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_length> L>
struct dim_energy : derived_dimension<Child, U, exp<F, 1>, exp<L, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_angle> A>
struct dim_torque : derived_dimension<Child, U, exp<E, 1>, exp<A, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_length> L>
struct dim_density : derived_dimension<Child, U, exp<M, 1>, exp<L, -3>> {};

template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_time> T>
struct dim_power : derived_dimension<Child, U, exp<E, 1>, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_power> P, DimensionOf<dim_electric_current> C>
struct dim_voltage : derived_dimension<Child, U, exp<P, 1>, exp<C, -1>> {};

template <typename Child, Unit U, DimensionOf<dim_voltage> V, DimensionOf<dim_electric_current> C>
struct dim_resistance : derived_dimension<Child,U, exp<V, 1>, exp<C, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_time> T, DimensionOf<dim_electric_current> C>
struct dim_electric_charge : derived_dimension<Child, U, exp<T, 1>, exp<C, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_electric_charge> C, DimensionOf<dim_voltage> V>
struct dim_capacitance : derived_dimension<Child, U, exp<C, 1>, exp<V, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_length> L>
struct dim_surface_tension : derived_dimension<Child, U, exp<F, 1>, exp<L, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_area> A>
struct dim_pressure : derived_dimension<Child, U, exp<F, 1>, exp<A, -1>> {};

template <typename Child, Unit U, DimensionOf<dim_voltage> V, DimensionOf<dim_time> T, DimensionOf<dim_length> L>
struct dim_magnetic_induction : derived_dimension<Child, U, exp<V, 1>, exp<T, 1>, exp<L, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_magnetic_induction> B, DimensionOf<dim_area> A>
struct dim_magnetic_flux : derived_dimension<Child, U, exp<B, 1>, exp<A, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_magnetic_flux> F, DimensionOf<dim_electric_current> I>
struct dim_inductance : derived_dimension<Child, U, exp<F, 1>, exp<I, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_resistance> R>
struct dim_conductance : derived_dimension<Child, U, exp<R, -1>> {};

// TODO Add when downcasting issue is solved
// template<typename Child, Unit U, DimensionOf<dim_time> T>
// struct dim_radioactivity : derived_dimension<Child, U, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_time> T, DimensionOf<dim_substance> M>
struct dim_catalytic_activity : derived_dimension<Child, U, exp<T, -1>, exp<M, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_mass> M>
struct dim_absorbed_dose : derived_dimension<Child, U, exp<E, 1>, exp<M, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_electric_current> I, DimensionOf<dim_length> L>
struct dim_current_density : derived_dimension<Child, U, exp<I, 1>, exp<L, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_substance> M, DimensionOf<dim_length> L>
struct dim_concentration : derived_dimension<Child, U, exp<M, 1>, exp<L, -3>> {};

template<typename Child, Unit U, DimensionOf<dim_luminous_intensity> I, DimensionOf<dim_length> L>
struct dim_luminance : derived_dimension<Child, U, exp<I, 1>, exp<L, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_pressure> P, DimensionOf<dim_time> T>
struct dim_dynamic_viscosity : derived_dimension<Child, U, exp<P, 1>, exp<T, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_thermodynamic_temperature> T>
struct dim_heat_capacity : derived_dimension<Child, U, exp<E, 1>, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_heat_capacity> C, DimensionOf<dim_mass> M>
struct dim_specific_heat_capacity : derived_dimension<Child, U, exp<C, 1>, exp<M, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_heat_capacity> C, DimensionOf<dim_substance> M>
struct dim_molar_heat_capacity : derived_dimension<Child, U, exp<C, 1>, exp<M, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_power> P, DimensionOf<dim_length> L, DimensionOf<dim_thermodynamic_temperature> T>
struct dim_thermal_conductivity : derived_dimension<Child, U, exp<P, 1>, exp<L, -1>, exp<T, -1>> {};

// TODO Add when downcasting issue is solved
// template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_length> L>
// struct dim_energy_density : derived_dimension<Child, U, exp<E, 1>, exp<L, -3>> {};

template<typename Child, Unit U, DimensionOf<dim_voltage> V, DimensionOf<dim_length> L>
struct dim_electric_field_strength : derived_dimension<Child, U, exp<V, 1>, exp<L, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_electric_charge> Q, DimensionOf<dim_length> L>
struct dim_charge_density : derived_dimension<Child, U, exp<Q, 1>, exp<L, -3>> {};

template<typename Child, Unit U, DimensionOf<dim_electric_charge> Q, DimensionOf<dim_length> L>
struct dim_surface_charge_density : derived_dimension<Child, U, exp<Q, 1>, exp<L, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_capacitance> C, DimensionOf<dim_length> L>
struct dim_permittivity : derived_dimension<Child, U, exp<C, 1>, exp<L, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_inductance> H, DimensionOf<dim_length> L>
struct dim_permeability : derived_dimension<Child, U, exp<H, 1>, exp<L, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_substance> M>
struct dim_molar_energy : derived_dimension<Child, U, exp<E, 1>, exp<M, -1>> {};

template<typename T>
concept Length = QuantityOf<T, dim_length>;

template<typename T>
concept Mass = QuantityOf<T, dim_mass>;

template<typename T>
concept Time = QuantityOf<T, dim_time>;

template<typename T>
concept Current = QuantityOf<T, dim_electric_current>;

template<typename T>
concept Temperature = QuantityOf<T, dim_thermodynamic_temperature>;

template<typename T>
concept Substance = QuantityOf<T, dim_substance>;

template<typename T>
concept LuminousIntensity = QuantityOf<T, dim_luminous_intensity>;

template <typename T>
concept Angle = QuantityOf<T, dim_angle>;

template<typename T>
concept Frequency = QuantityOf<T, dim_frequency>;

template<typename T>
concept Area = QuantityOf<T, dim_area>;

template<typename T>
concept Volume = QuantityOf<T, dim_volume>;

template<typename T>
concept Speed = QuantityOf<T, dim_speed>;

template<typename T>
concept Acceleration = QuantityOf<T, dim_acceleration>;

template<typename T>
concept Force = QuantityOf<T, dim_force>;

template<typename T>
concept Momentum = QuantityOf<T, dim_momentum>;

template<typename T>
concept Energy = QuantityOf<T, dim_energy>;

template<typename T>
concept Torque = QuantityOf<T, dim_torque>;

template<typename T>
concept Density = QuantityOf<T, dim_density>;

template<typename T>
concept Power = QuantityOf<T, dim_power>;

template<typename T>
concept Voltage = QuantityOf<T, dim_voltage>;

template<typename T>
concept ElectricCharge = QuantityOf<T, dim_electric_charge>;

template<typename T>
concept Capacitance = QuantityOf<T, dim_capacitance>;

template<typename T>
concept SurfaceTension = QuantityOf<T, dim_surface_tension>;

template<typename T>
concept Pressure = QuantityOf<T, dim_pressure>;

template<typename T>
concept MagneticInduction = QuantityOf<T, dim_magnetic_induction>;

template<typename T>
concept MagneticFlux = QuantityOf<T, dim_magnetic_flux>;

template<typename T>
concept Inductance = QuantityOf<T, dim_inductance>;

template<typename T>
concept Conductance = QuantityOf<T, dim_conductance>;

// TODO Add when downcasting issue is solved
// template<typename T>
// concept Radioactivity = QuantityOf<T, dim_radioactivity>;

template<typename T>
concept CatalyticActivity = QuantityOf<T, dim_catalytic_activity>;

template<typename T>
concept AbsorbedDose = QuantityOf<T, dim_absorbed_dose>;

template<typename T>
concept CurrentDensity = QuantityOf<T, dim_current_density>;

template<typename T>
concept Concentration = QuantityOf<T, dim_concentration>;

template<typename T>
concept Luminance = QuantityOf<T, dim_luminance>;

template<typename T>
concept DynamicViscosity = QuantityOf<T, dim_dynamic_viscosity>;

template<typename T>
concept HeatCapacity = QuantityOf<T, dim_heat_capacity>;

template<typename T>
concept SpecificHeatCapacity = QuantityOf<T, dim_specific_heat_capacity>;

template<typename T>
concept MolarHeatCapacity = QuantityOf<T, dim_molar_heat_capacity>;

template<typename T>
concept ThermalConductivity = QuantityOf<T, dim_thermal_conductivity>;

// TODO Add when downcasting issue is solved
// template<typename T>
// concept EnergyDensity = QuantityOf<T, dim_energy_density>;

template<typename T>
concept ElectricFieldStrength = QuantityOf<T, dim_electric_field_strength>;

template<typename T>
concept ChargeDensity = QuantityOf<T, dim_charge_density>;

template<typename T>
concept SurfaceChargeDensity = QuantityOf<T, dim_surface_charge_density>;

template<typename T>
concept Permittivity = QuantityOf<T, dim_permittivity>;

template<typename T>
concept Permeability = QuantityOf<T, dim_permeability>;

template<typename T>
concept MolarEnergy = QuantityOf<T, dim_molar_energy>;

}  // namespace units::physical
