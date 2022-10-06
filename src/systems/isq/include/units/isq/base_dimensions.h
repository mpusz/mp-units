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

#include <units/dimension.h>

namespace units::isq {

// clang-format off
inline constexpr struct length_dim : base_dimension<"L"> {} length_dim;
inline constexpr struct mass_dim : base_dimension<"M"> {} mass_dim;
inline constexpr struct time_dim : base_dimension<"T"> {} time_dim;
inline constexpr struct electric_current_dim : base_dimension<"I"> {} electric_current_dim;
// TODO Should the below use basic_symbol_text? How to name it for ASCII?
inline constexpr struct thermodynamic_temperature_dim : base_dimension<"Θ"> {} thermodynamic_temperature_dim;
inline constexpr struct amount_of_substance_dim : base_dimension<"N"> {} amount_of_substance_dim;
inline constexpr struct luminous_intensity_dim : base_dimension<"J"> {} luminous_intensity_dim;
// clang-format on

}  // namespace units::isq
