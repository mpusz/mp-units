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
concept in_dimension_of = in_dimension<Dim> && is_derived_from_instantiation<Dim, DimTemplate>;

template<typename Q, template<typename...> typename DimTemplate>
concept in_quantity_of = in_quantity<Q> && is_derived_from_instantiation<typename Q::dimension, DimTemplate>;

// ------------------------ base dimensions -----------------------------

template<in_unit U>
struct dim_length : base_dimension<"L", U> {};

template<in_unit U>
struct dim_mass : base_dimension<"M", U> {};

template<in_unit U>
struct dim_time : base_dimension<"T", U> {};

template<in_unit U>
struct dim_electric_current : base_dimension<"I", U> {};

template<in_unit U>
struct dim_thermodynamic_temperature : base_dimension<"Θ", U> {};

template<in_unit U>
struct dim_substance : base_dimension<"N", U> {};

template<in_unit U>
struct dim_luminous_intensity : base_dimension<"J", U> {};

template<in_unit U>
struct dim_angle : base_dimension<"A", U> {};

// ------------------------ derived dimensions -----------------------------

template<typename Child, in_unit U, in_dimension_of<dim_time> T>
struct dim_frequency : derived_dimension<Child, U, exp<T, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_length> L>
struct dim_area : derived_dimension<Child, U, exp<L, 2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_length> L>
struct dim_volume : derived_dimension<Child, U, exp<L, 3>> {};

template<typename Child, in_unit U, in_dimension_of<dim_length> L, in_dimension_of<dim_time> T>
struct dim_speed : derived_dimension<Child, U, exp<L, 1>, exp<T, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_length> L, in_dimension_of<dim_time> T>
struct dim_acceleration : derived_dimension<Child, U, exp<L, 1>, exp<T, -2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_mass> M, in_dimension_of<dim_acceleration> A>
struct dim_force : derived_dimension<Child, U, exp<M, 1>, exp<A, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_mass> M, in_dimension_of<dim_speed> V>
struct dim_momentum : derived_dimension<Child, U, exp<M, 1>, exp<V, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_force> F, in_dimension_of<dim_length> L>
struct dim_energy : derived_dimension<Child, U, exp<F, 1>, exp<L, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_angle> A>
struct dim_torque : derived_dimension<Child, U, exp<E, 1>, exp<A, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_mass> M, in_dimension_of<dim_length> L>
struct dim_density : derived_dimension<Child, U, exp<M, 1>, exp<L, -3>> {};

template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_time> T>
struct dim_power : derived_dimension<Child, U, exp<E, 1>, exp<T, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_power> P, in_dimension_of<dim_electric_current> C>
struct dim_voltage : derived_dimension<Child, U, exp<P, 1>, exp<C, -1>> {};

template <typename Child, in_unit U, in_dimension_of<dim_voltage> V, in_dimension_of<dim_electric_current> C>
struct dim_resistance : derived_dimension<Child,U, exp<V, 1>, exp<C, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_time> T, in_dimension_of<dim_electric_current> C>
struct dim_electric_charge : derived_dimension<Child, U, exp<T, 1>, exp<C, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_electric_charge> C, in_dimension_of<dim_voltage> V>
struct dim_capacitance : derived_dimension<Child, U, exp<C, 1>, exp<V, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_force> F, in_dimension_of<dim_length> L>
struct dim_surface_tension : derived_dimension<Child, U, exp<F, 1>, exp<L, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_force> F, in_dimension_of<dim_area> A>
struct dim_pressure : derived_dimension<Child, U, exp<F, 1>, exp<A, -1>> {};

template <typename Child, in_unit U, in_dimension_of<dim_voltage> V, in_dimension_of<dim_time> T, in_dimension_of<dim_length> L>
struct dim_magnetic_induction : derived_dimension<Child, U, exp<V, 1>, exp<T, 1>, exp<L, -2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_magnetic_induction> B, in_dimension_of<dim_area> A>
struct dim_magnetic_flux : derived_dimension<Child, U, exp<B, 1>, exp<A, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_magnetic_flux> F, in_dimension_of<dim_electric_current> I>
struct dim_inductance : derived_dimension<Child, U, exp<F, 1>, exp<I, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_resistance> R>
struct dim_conductance : derived_dimension<Child, U, exp<R, -1>> {};

// TODO Add when downcasting issue is solved
// template<typename Child, in_unit U, in_dimension_of<dim_time> T>
// struct dim_radioactivity : derived_dimension<Child, U, exp<T, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_time> T, in_dimension_of<dim_substance> M>
struct dim_catalytic_activity : derived_dimension<Child, U, exp<T, -1>, exp<M, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_mass> M>
struct dim_absorbed_dose : derived_dimension<Child, U, exp<E, 1>, exp<M, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_electric_current> I, in_dimension_of<dim_length> L>
struct dim_current_density : derived_dimension<Child, U, exp<I, 1>, exp<L, -2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_substance> M, in_dimension_of<dim_length> L>
struct dim_concentration : derived_dimension<Child, U, exp<M, 1>, exp<L, -3>> {};

template<typename Child, in_unit U, in_dimension_of<dim_luminous_intensity> I, in_dimension_of<dim_length> L>
struct dim_luminance : derived_dimension<Child, U, exp<I, 1>, exp<L, -2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_pressure> P, in_dimension_of<dim_time> T>
struct dim_dynamic_viscosity : derived_dimension<Child, U, exp<P, 1>, exp<T, 1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_thermodynamic_temperature> T>
struct dim_heat_capacity : derived_dimension<Child, U, exp<E, 1>, exp<T, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_heat_capacity> C, in_dimension_of<dim_mass> M>
struct dim_specific_heat_capacity : derived_dimension<Child, U, exp<C, 1>, exp<M, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_heat_capacity> C, in_dimension_of<dim_substance> M>
struct dim_molar_heat_capacity : derived_dimension<Child, U, exp<C, 1>, exp<M, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_power> P, in_dimension_of<dim_length> L, in_dimension_of<dim_thermodynamic_temperature> T>
struct dim_thermal_conductivity : derived_dimension<Child, U, exp<P, 1>, exp<L, -1>, exp<T, -1>> {};

// TODO Add when downcasting issue is solved
// template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_length> L>
// struct dim_energy_density : derived_dimension<Child, U, exp<E, 1>, exp<L, -3>> {};

template<typename Child, in_unit U, in_dimension_of<dim_voltage> V, in_dimension_of<dim_length> L>
struct dim_electric_field_strength : derived_dimension<Child, U, exp<V, 1>, exp<L, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_electric_charge> Q, in_dimension_of<dim_length> L>
struct dim_charge_density : derived_dimension<Child, U, exp<Q, 1>, exp<L, -3>> {};

template<typename Child, in_unit U, in_dimension_of<dim_electric_charge> Q, in_dimension_of<dim_length> L>
struct dim_surface_charge_density : derived_dimension<Child, U, exp<Q, 1>, exp<L, -2>> {};

template<typename Child, in_unit U, in_dimension_of<dim_capacitance> C, in_dimension_of<dim_length> L>
struct dim_permittivity : derived_dimension<Child, U, exp<C, 1>, exp<L, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_inductance> H, in_dimension_of<dim_length> L>
struct dim_permeability : derived_dimension<Child, U, exp<H, 1>, exp<L, -1>> {};

template<typename Child, in_unit U, in_dimension_of<dim_energy> E, in_dimension_of<dim_substance> M>
struct dim_molar_energy : derived_dimension<Child, U, exp<E, 1>, exp<M, -1>> {};

template<typename T>
concept in_length = in_quantity_of<T, dim_length>;

template<typename T>
concept in_mass = in_quantity_of<T, dim_mass>;

template<typename T>
concept in_time = in_quantity_of<T, dim_time>;

template<typename T>
concept in_current = in_quantity_of<T, dim_electric_current>;

template<typename T>
concept in_temperature = in_quantity_of<T, dim_thermodynamic_temperature>;

template<typename T>
concept in_substance = in_quantity_of<T, dim_substance>;

template<typename T>
concept in_luminous_intensity = in_quantity_of<T, dim_luminous_intensity>;

template <typename T>
concept in_angle = in_quantity_of<T, dim_angle>;

template<typename T>
concept in_frequency = in_quantity_of<T, dim_frequency>;

template<typename T>
concept in_area = in_quantity_of<T, dim_area>;

template<typename T>
concept in_volume = in_quantity_of<T, dim_volume>;

template<typename T>
concept in_speed = in_quantity_of<T, dim_speed>;

template<typename T>
concept in_acceleration = in_quantity_of<T, dim_acceleration>;

template<typename T>
concept in_force = in_quantity_of<T, dim_force>;

template<typename T>
concept in_momentum = in_quantity_of<T, dim_momentum>;

template<typename T>
concept in_energy = in_quantity_of<T, dim_energy>;

template<typename T>
concept in_torque = in_quantity_of<T, dim_torque>;

template<typename T>
concept in_density = in_quantity_of<T, dim_density>;

template<typename T>
concept in_power = in_quantity_of<T, dim_power>;

template<typename T>
concept in_voltage = in_quantity_of<T, dim_voltage>;

template<typename T>
concept in_electric_charge = in_quantity_of<T, dim_electric_charge>;

template<typename T>
concept in_capacitance = in_quantity_of<T, dim_capacitance>;

template<typename T>
concept in_surface_tension = in_quantity_of<T, dim_surface_tension>;

template<typename T>
concept in_pressure = in_quantity_of<T, dim_pressure>;

template<typename T>
concept in_magnetic_induction = in_quantity_of<T, dim_magnetic_induction>;

template<typename T>
concept in_magnetic_flux = in_quantity_of<T, dim_magnetic_flux>;

template<typename T>
concept in_inductance = in_quantity_of<T, dim_inductance>;

template<typename T>
concept in_conductance = in_quantity_of<T, dim_conductance>;

// TODO Add when downcasting issue is solved
// template<typename T>
// concept Radioactivity = in_quantity_of<T, dim_radioactivity>;

template<typename T>
concept in_catalytic_activity = in_quantity_of<T, dim_catalytic_activity>;

template<typename T>
concept in_absorbed_dose = in_quantity_of<T, dim_absorbed_dose>;

template<typename T>
concept in_current_density = in_quantity_of<T, dim_current_density>;

template<typename T>
concept in_concentration = in_quantity_of<T, dim_concentration>;

template<typename T>
concept in_luminance = in_quantity_of<T, dim_luminance>;

template<typename T>
concept in_dynamic_viscosity = in_quantity_of<T, dim_dynamic_viscosity>;

template<typename T>
concept in_heat_capacity = in_quantity_of<T, dim_heat_capacity>;

template<typename T>
concept in_specific_heat_capacity = in_quantity_of<T, dim_specific_heat_capacity>;

template<typename T>
concept in_molar_heat_capacity = in_quantity_of<T, dim_molar_heat_capacity>;

template<typename T>
concept in_thermal_conductivity = in_quantity_of<T, dim_thermal_conductivity>;

// TODO Add when downcasting issue is solved
// template<typename T>
// concept EnergyDensity = in_quantity_of<T, dim_energy_density>;

template<typename T>
concept in_electric_field_strength = in_quantity_of<T, dim_electric_field_strength>;

template<typename T>
concept in_charge_density = in_quantity_of<T, dim_charge_density>;

template<typename T>
concept in_surface_charge_density = in_quantity_of<T, dim_surface_charge_density>;

template<typename T>
concept in_permittivity = in_quantity_of<T, dim_permittivity>;

template<typename T>
concept in_permeability = in_quantity_of<T, dim_permeability>;

template<typename T>
concept in_molar_energy = in_quantity_of<T, dim_molar_energy>;

}  // namespace units::physical
