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
#include <units/bits/type_traits.h>
#include <units/quantity.h>
#include <units/unit.h>

namespace units {

inline namespace physical {

template<typename Dim, template<typename...> typename DimTemplate>
concept DimensionOf = (Dimension<Dim> || BaseDimension<Dim>) && is_derived_from_instantiation<Dim, DimTemplate>;

template<typename Q, template<typename...> typename DimTemplate>
concept QuantityOf = Quantity<Q> && is_derived_from_instantiation<Q::dimension, DimTemplate>;

// ------------------------ base dimensions -----------------------------

// length
template<Unit U>
struct dim_length : base_dimension<"length", U> {};

template<typename T>
concept Length = QuantityOf<T, dim_length>;

// mass
template<Unit U>
struct dim_mass : base_dimension<"mass", U> {};

template<typename T>
concept Mass = QuantityOf<T, dim_mass>;

// time
template<Unit U>
struct dim_time : base_dimension<"time", U> {};

template<typename T>
concept Time = QuantityOf<T, dim_time>;

// current
template<Unit U>
struct dim_current : base_dimension<"current", U> {};

template<typename T>
concept Current = QuantityOf<T, dim_current>;

// temperature
template<Unit U>
struct dim_temperature : base_dimension<"temperature", U> {};

template<typename T>
concept Temperature = QuantityOf<T, dim_temperature>;

// substance
template<Unit U>
struct dim_substance : base_dimension<"substance", U> {};

template<typename T>
concept Substance = QuantityOf<T, dim_substance>;

// luminous intensity
template<Unit U>
struct dim_luminous_intensity : base_dimension<"luminous intensity", U> {};

template<typename T>
concept LuminousIntensity = QuantityOf<T, dim_luminous_intensity>;

// ------------------------ derived dimensions -----------------------------

// frequency
template<typename Child, Unit U, DimensionOf<dim_time> T>
struct dim_frequency : derived_dimension<Child, U, exp<T, -1>> {};

template<typename T>
concept Frequency = QuantityOf<T, dim_frequency>;

// area
template<typename Child, Unit U, DimensionOf<dim_length> L>
struct dim_area : derived_dimension<Child, U, exp<L, 2>> {};

template<typename T>
concept Area = QuantityOf<T, dim_area>;

// velocity
template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_velocity : derived_dimension<Child, U, exp<L, 1>, exp<T, -1>> {};

template<typename T>
concept Velocity = QuantityOf<T, dim_velocity>;

// acceleration
template<typename Child, Unit U, DimensionOf<dim_length> L, DimensionOf<dim_time> T>
struct dim_acceleration : derived_dimension<Child, U, exp<L, 1>, exp<T, -2>> {};

template<typename T>
concept Acceleration = QuantityOf<T, dim_acceleration>;

// force
template<typename Child, Unit U, DimensionOf<dim_mass> M, DimensionOf<dim_acceleration> A>
struct dim_force : derived_dimension<Child, U, exp<M, 1>, exp<A, 1>> {};

template<typename T>
concept Force = QuantityOf<T, dim_force>;

// energy
template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_length> L>
struct dim_energy : derived_dimension<Child, U, exp<F, 1>, exp<L, 1>> {};

template<typename T>
concept Energy = QuantityOf<T, dim_energy>;

// power
template<typename Child, Unit U, DimensionOf<dim_energy> E, DimensionOf<dim_time> T>
struct dim_power : derived_dimension<Child, U, exp<E, 1>, exp<T, -1>> {};

template<typename T>
concept Power = QuantityOf<T, dim_power>;

// pressure
template<typename Child, Unit U, DimensionOf<dim_force> F, DimensionOf<dim_area> A>
struct dim_pressure : derived_dimension<Child, U, exp<F, 1>, exp<A, -1>> {};

template<typename T>
concept Pressure = QuantityOf<T, dim_pressure>;

}  // namespace physical

}  // namespace units
