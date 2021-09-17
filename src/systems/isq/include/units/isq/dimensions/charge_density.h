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
#include <units/isq/dimensions/electric_charge.h>
#include <units/isq/dimensions/length.h>

namespace units::isq {

template<typename Child, Unit U, typename...>
struct dim_charge_density;

template<typename Child, Unit U, DimensionOfT<dim_electric_charge> Q, DimensionOfT<dim_length> L>
struct dim_charge_density<Child, U, Q, L> : derived_dimension<Child, U, exponent<Q, 1>, exponent<L, -3>> {};

template<typename Child, Unit U, typename...>
struct dim_surface_charge_density;

template<typename Child, Unit U, DimensionOfT<dim_electric_charge> Q, DimensionOfT<dim_length> L>
struct dim_surface_charge_density<Child, U, Q, L> : derived_dimension<Child, U, exponent<Q, 1>, exponent<L, -2>> {};

template<typename T>
concept ChargeDensity = QuantityOfT<T, dim_charge_density>;

template<typename T>
concept SurfaceChargeDensity = QuantityOfT<T, dim_surface_charge_density>;

}  // namespace units::isq
