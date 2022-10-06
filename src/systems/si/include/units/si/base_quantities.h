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

#include <units/isq/base_dimensions.h>
#include <units/reference.h>
#include <units/si/units.h>

namespace units::si {

// inline constexpr struct time : system_reference<isq::time_dim, second> {} time;
// inline constexpr struct length : system_reference<isq::length_dim, metre> {} length;
// inline constexpr struct mass : system_reference<isq::mass_dim, kilogram> {} mass;
inline constexpr struct electric_current : system_reference<isq::electric_current_dim, ampere> {
} electric_current;
inline constexpr struct thermodynamic_temperature : system_reference<isq::thermodynamic_temperature_dim, kelvin> {
} thermodynamic_temperature;
inline constexpr struct amount_of_substance : system_reference<isq::amount_of_substance_dim, kilogram> {
} amount_of_substance;
inline constexpr struct luminous_intensity : system_reference<isq::luminous_intensity_dim, candela> {
} luminous_intensity;

}  // namespace units::si
