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

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::isq {

// clang-format off
// dimensions of base quantities
inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass final : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_current final : base_dimension<"I"> {} dim_electric_current;
inline constexpr struct dim_thermodynamic_temperature final : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_thermodynamic_temperature;
inline constexpr struct dim_amount_of_substance final : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity final : base_dimension<"J"> {} dim_luminous_intensity;
// clang-format on

// base quantities
QUANTITY_SPEC(length, dim_length);
QUANTITY_SPEC(mass, dim_mass);
QUANTITY_SPEC(time, dim_time);
inline constexpr auto duration = time;
QUANTITY_SPEC(electric_current, dim_electric_current);
QUANTITY_SPEC(thermodynamic_temperature, dim_thermodynamic_temperature);
QUANTITY_SPEC(amount_of_substance, dim_amount_of_substance);
QUANTITY_SPEC(luminous_intensity, dim_luminous_intensity);

}  // namespace mp_units::isq
