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
#include <units/isq/dimensions/energy.h>
#include <units/isq/dimensions/time.h>

namespace units::isq {

template<typename Child, Unit U, typename...>
struct dim_power;

template<typename Child, Unit U, DimensionOfT<dim_energy> E, DimensionOfT<dim_time> T>
struct dim_power<Child, U, E, T> : derived_dimension<Child, U, exponent<E, 1>, exponent<T, -1>> {};

template<typename Child, Unit U, DimensionOfT<dim_length> L, DimensionOfT<dim_force> F, DimensionOfT<dim_time> T>
struct dim_power<Child, U, L, F, T> : derived_dimension<Child, U, exponent<L, 1>, exponent<F, 1>, exponent<T, -1>> {};

template<typename T>
concept Power = QuantityOfT<T, dim_power>;

}  // namespace units::isq
