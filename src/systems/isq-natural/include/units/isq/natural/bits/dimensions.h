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

#include <units/isq/dimensions.h>
#include <units/isq/natural/bits/units.h>
#include <units/quantity.h>

namespace units::isq::natural {

struct dim_length : isq::dim_length<inverted_gigaelectronvolt> {};
template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

struct dim_time : isq::dim_time<inverted_gigaelectronvolt> {};
template<UnitOf<dim_time> U, Representation Rep = double>
using time = quantity<dim_time, U, Rep>;

struct dim_mass : isq::dim_mass<gigaelectronvolt> {};
template<UnitOf<dim_mass> U, Representation Rep = double>
using mass = quantity<dim_mass, U, Rep>;

struct dim_speed : isq::dim_speed<dim_speed, one, dim_length, dim_time> {};
template<UnitOf<dim_speed> U, Representation Rep = double>
using speed = quantity<dim_speed, U, Rep>;

struct dim_acceleration : isq::dim_acceleration<dim_acceleration, gigaelectronvolt, dim_length, dim_time> {};
template<UnitOf<dim_acceleration> U, Representation Rep = double>
using acceleration = quantity<dim_acceleration, U, Rep>;

struct dim_force : isq::dim_force<dim_force, square_gigaelectronvolt, dim_mass, dim_acceleration> {};
template<UnitOf<dim_force> U, Representation Rep = double>
using force = quantity<dim_force, U, Rep>;

struct dim_momentum : isq::dim_momentum<dim_momentum, gigaelectronvolt, dim_mass, dim_speed> {};
template<UnitOf<dim_momentum> U, Representation Rep = double>
using momentum = quantity<dim_momentum, U, Rep>;

struct dim_energy : isq::dim_energy<dim_energy, gigaelectronvolt, dim_force, dim_length> {};
template<UnitOf<dim_force> U, Representation Rep = double>
using energy = quantity<dim_force, U, Rep>;

// Typical UDLs will not work here as the same units are reused by many quantities.
// Should we define some strange ones (i.e. _q_mass_GeV)?

}  // namespace units::isq::natural
