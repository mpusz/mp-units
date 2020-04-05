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

namespace units {

namespace physical {

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

// ------------------------ derived dimensions -----------------------------

template<typename Child, Unit U, DimensionOf<dim_time> T>
struct dim_frequency : derived_dimension<Child, U, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L>
struct dim_area : derived_dimension<Child, U, exp<L, 2>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L>
struct dim_volume : derived_dimension<Child, U, exp<L, 3>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_velocity : derived_dimension<Child, U, exp<L, 1>, exp<T, -1>> {};

template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_acceleration : derived_dimension<Child, U, exp<L, 1>, exp<T, -2>> {};

template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_acceleration> A>
struct dim_force : derived_dimension<Child, U, exp<M, 1>, exp<A, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_velocity> V>
struct dim_momentum : derived_dimension<Child, U, exp<M, 1>, exp<V, 1>> {};

template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_length> L>
struct dim_energy : derived_dimension<Child, U, exp<F, 1>, exp<L, 1>> {};

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

}  // namespace physical

template<typename T>
concept Length = physical::QuantityOf<T, physical::dim_length>;

template<typename T>
concept Mass = physical::QuantityOf<T, physical::dim_mass>;

template<typename T>
concept Time = physical::QuantityOf<T, physical::dim_time>;

template<typename T>
concept Current = physical::QuantityOf<T, physical::dim_electric_current>;

template<typename T>
concept Temperature = physical::QuantityOf<T, physical::dim_thermodynamic_temperature>;

template<typename T>
concept Substance = physical::QuantityOf<T, physical::dim_substance>;

template<typename T>
concept LuminousIntensity = physical::QuantityOf<T, physical::dim_luminous_intensity>;

template<typename T>
concept Frequency = physical::QuantityOf<T, physical::dim_frequency>;

template<typename T>
concept Area = physical::QuantityOf<T, physical::dim_area>;

template<typename T>
concept Volume = physical::QuantityOf<T, physical::dim_volume>;

template<typename T>
concept Velocity = physical::QuantityOf<T, physical::dim_velocity>;

template<typename T>
concept Acceleration = physical::QuantityOf<T, physical::dim_acceleration>;

template<typename T>
concept Force = physical::QuantityOf<T, physical::dim_force>;

template<typename T>
concept Momentum = physical::QuantityOf<T, physical::dim_momentum>;

template<typename T>
concept Energy = physical::QuantityOf<T, physical::dim_energy>;

template<typename T>
concept Power = physical::QuantityOf<T, physical::dim_power>;

template<typename T>
concept Voltage = physical::QuantityOf<T, physical::dim_voltage>;

template<typename T>
concept ElectricCharge = physical::QuantityOf<T, physical::dim_electric_charge>;

template<typename T>
concept Capacitance = physical::QuantityOf<T, physical::dim_capacitance>;

template<typename T>
concept SurfaceTension = physical::QuantityOf<T, physical::dim_surface_tension>;

template<typename T>
concept Pressure = physical::QuantityOf<T, physical::dim_pressure>;

template<typename T>
concept MagneticInduction = physical::QuantityOf<T, physical::dim_magnetic_induction>;

template <typename T>
concept MagneticFlux = physical::QuantityOf<T, physical::dim_magnetic_flux>;

template <typename T>
concept Inductance = physical::QuantityOf<T, physical::dim_inductance>;

template <typename T>
concept Conductance = physical::QuantityOf<T, physical::dim_inductance>;

}  // namespace units
