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

#include <units/physical/dimensions.h>
#include <units/physical/natural/units.h>
#include <units/quantity.h>

namespace units::physical::natural {

struct dim_length : physical::dim_length<inverted_gigaelectronvolt> {};
template<Unit U, ScalableNumber Rep = double>
using length = quantity<dim_length, U, Rep>;

struct dim_time : physical::dim_time<inverted_gigaelectronvolt> {};
template<Unit U, ScalableNumber Rep = double>
using time = quantity<dim_time, U, Rep>;

struct dim_mass : physical::dim_mass<gigaelectronvolt> {};
template<Unit U, ScalableNumber Rep = double>
using mass = quantity<dim_mass, U, Rep>;

struct dim_speed : physical::dim_speed<dim_speed, one, dim_length, dim_time> {};
template<Unit U, ScalableNumber Rep = double>
using speed = quantity<dim_speed, U, Rep>;

struct dim_acceleration : physical::dim_acceleration<dim_acceleration, gigaelectronvolt, dim_length, dim_time> {};
template<Unit U, ScalableNumber Rep = double>
using acceleration = quantity<dim_acceleration, U, Rep>;

struct dim_force : physical::dim_force<dim_force, square_gigaelectronvolt, dim_mass, dim_acceleration> {};
template<Unit U, ScalableNumber Rep = double>
using force = quantity<dim_force, U, Rep>;

struct dim_momentum : physical::dim_momentum<dim_momentum, gigaelectronvolt, dim_mass, dim_speed> {};
template<Unit U, ScalableNumber Rep = double>
using momentum = quantity<dim_momentum, U, Rep>;

struct dim_energy : physical::dim_energy<dim_energy, gigaelectronvolt, dim_force, dim_length> {};
template<Unit U, ScalableNumber Rep = double>
using energy = quantity<dim_force, U, Rep>;

// Typical UDLs will not work here as the same units are reused by many quantities.
// Should we define some strange ones (i.e. q_mass_GeV)?

}  // namespace units::physical::natural
