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

#include <mp-units/framework.h>
#include <mp-units/systems/isq/isq.h>
#include <string_view>

namespace {

using namespace mp_units;

using enum text_encoding;

static_assert(dimension_symbol(dimension_one) == "1");

// base dimensions
static_assert(dimension_symbol(isq::dim_length) == "L");
static_assert(dimension_symbol(isq::dim_thermodynamic_temperature) == "Θ");
static_assert(dimension_symbol<dimension_symbol_formatting{.encoding = ascii}>(isq::dim_thermodynamic_temperature) ==
              "O");

// derived dimensions
static_assert(dimension_symbol(isq::speed.dimension) == "LT⁻¹");
static_assert(dimension_symbol<dimension_symbol_formatting{.encoding = ascii}>(isq::speed.dimension) == "LT^-1");
static_assert(dimension_symbol(isq::power.dimension) == "L²MT⁻³");
static_assert(dimension_symbol<dimension_symbol_formatting{.encoding = ascii}>(isq::power.dimension) == "L^2MT^-3");

static_assert(dimension_symbol(pow<123>(isq::dim_length)) == "L¹²³");
static_assert(dimension_symbol(pow<1, 2>(isq::dim_length)) == "L^(1/2)");
static_assert(dimension_symbol(pow<3, 5>(isq::dim_length)) == "L^(3/5)");
static_assert(dimension_symbol(pow<123>(isq::speed.dimension)) == "L¹²³T⁻¹²³");
static_assert(dimension_symbol(pow<1, 2>(isq::speed.dimension)) == "L^(1/2)T^-(1/2)");
static_assert(dimension_symbol(pow<3, 5>(isq::speed.dimension)) == "L^(3/5)T^-(3/5)");

}  // namespace
