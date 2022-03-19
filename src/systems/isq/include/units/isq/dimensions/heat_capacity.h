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

#include <units/concepts.h>
#include <units/isq/dimensions/amount_of_substance.h>
#include <units/isq/dimensions/energy.h>
#include <units/isq/dimensions/mass.h>
#include <units/isq/dimensions/thermodynamic_temperature.h>

namespace units::isq {

template<typename Child, Unit U, typename...>
struct dim_heat_capacity;

template<typename Child, Unit U, DimensionOfT<dim_energy> E, DimensionOfT<dim_thermodynamic_temperature> T>
struct dim_heat_capacity<Child, U, E, T> : derived_dimension<Child, U, exponent<E, 1>, exponent<T, -1>> {};

template<typename Child, Unit U, typename...>
struct dim_specific_heat_capacity;

template<typename Child, Unit U, DimensionOfT<dim_heat_capacity> C, DimensionOfT<dim_mass> M>
struct dim_specific_heat_capacity<Child, U, C, M> : derived_dimension<Child, U, exponent<C, 1>, exponent<M, -1>> {};

template<typename Child, Unit U, typename...>
struct dim_molar_heat_capacity;

template<typename Child, Unit U, DimensionOfT<dim_heat_capacity> C, DimensionOfT<dim_amount_of_substance> M>
struct dim_molar_heat_capacity<Child, U, C, M> : derived_dimension<Child, U, exponent<C, 1>, exponent<M, -1>> {};

template<typename T>
concept HeatCapacity = QuantityOfT<T, dim_heat_capacity>;

template<typename T>
concept SpecificHeatCapacity = QuantityOfT<T, dim_specific_heat_capacity>;

template<typename T>
concept MolarHeatCapacity = QuantityOfT<T, dim_molar_heat_capacity>;

}  // namespace units::isq
