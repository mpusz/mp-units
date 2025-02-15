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

#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#if MP_UNITS_HOSTED
#include <cmath>
#endif  // MP_UNITS_HOSTED
#endif

namespace {

// ********************* QUANTITY *********************

using q_double = mp_units::quantity<mp_units::si::metre>;
using q_int = mp_units::quantity<mp_units::si::metre, int>;

// cv qualifiers
static_assert(std::numeric_limits<const q_double>::max() == q_double::max());
static_assert(std::numeric_limits<volatile q_double>::max() == q_double::max());
static_assert(std::numeric_limits<const volatile q_double>::max() == q_double::max());

// is_specialized
static_assert(std::numeric_limits<q_double>::is_specialized == true);
static_assert(std::numeric_limits<q_int>::is_specialized == true);

// is_integer
static_assert(std::numeric_limits<q_double>::is_integer == false);
static_assert(std::numeric_limits<q_int>::is_integer == true);

// has_infinity
static_assert(std::numeric_limits<q_double>::has_infinity == true);
static_assert(std::numeric_limits<q_int>::has_infinity == false);

// min
static_assert(std::numeric_limits<q_double>::min() == q_double::min());
static_assert(std::numeric_limits<q_int>::min() == q_int::min());

static_assert(std::numeric_limits<q_double>::min().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::lowest());
static_assert(std::numeric_limits<q_int>::min().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::lowest());

// max
static_assert(std::numeric_limits<q_double>::max() == q_double::max());
static_assert(std::numeric_limits<q_int>::max() == q_int::max());

static_assert(std::numeric_limits<q_double>::max().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::max());
static_assert(std::numeric_limits<q_int>::max().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::max());

// lowest
static_assert(std::numeric_limits<q_double>::lowest().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::lowest());
static_assert(std::numeric_limits<q_int>::lowest().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::lowest());

// epsilon
static_assert(std::numeric_limits<q_double>::epsilon().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::epsilon());
static_assert(std::numeric_limits<q_int>::epsilon().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::epsilon());

// round_error
static_assert(std::numeric_limits<q_double>::round_error().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::round_error());
static_assert(std::numeric_limits<q_int>::round_error().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::round_error());

// infinity
static_assert(std::numeric_limits<q_double>::infinity().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::infinity());
static_assert(std::numeric_limits<q_int>::infinity().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::infinity());

#if MP_UNITS_HOSTED && (__cpp_lib_constexpr_cmath || MP_UNITS_COMP_GCC)

// quiet_NaN
static_assert(std::isnan(std::numeric_limits<q_double>::quiet_NaN().numerical_value_in(mp_units::si::metre)));

// signaling_NaN
static_assert(std::isnan(std::numeric_limits<q_double>::signaling_NaN().numerical_value_in(mp_units::si::metre)));

#endif

// denorm_min
static_assert(std::numeric_limits<q_double>::denorm_min().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::denorm_min());
static_assert(std::numeric_limits<q_int>::denorm_min().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::denorm_min());


// ********************* QUANTITY POINT *********************

using qp_double = mp_units::quantity_point<mp_units::si::metre>;
using qp_int = mp_units::quantity_point<mp_units::si::metre, mp_units::default_point_origin(mp_units::si::metre), int>;

// cv qualifiers
static_assert(std::numeric_limits<const qp_double>::max() == qp_double::max());
static_assert(std::numeric_limits<volatile qp_double>::max() == qp_double::max());
static_assert(std::numeric_limits<const volatile qp_double>::max() == qp_double::max());

// is_specialized
static_assert(std::numeric_limits<qp_double>::is_specialized == true);
static_assert(std::numeric_limits<qp_int>::is_specialized == true);

// is_integer
static_assert(std::numeric_limits<qp_double>::is_integer == false);
static_assert(std::numeric_limits<qp_int>::is_integer == true);

// has_infinity
static_assert(std::numeric_limits<qp_double>::has_infinity == true);
static_assert(std::numeric_limits<qp_int>::has_infinity == false);

// min
static_assert(std::numeric_limits<qp_double>::min() == qp_double::min());
static_assert(std::numeric_limits<qp_int>::min() == qp_int::min());

static_assert(std::numeric_limits<qp_double>::min().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::lowest());
static_assert(std::numeric_limits<qp_int>::min().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::lowest());

// max
static_assert(std::numeric_limits<qp_double>::max() == qp_double::max());
static_assert(std::numeric_limits<qp_int>::max() == qp_int::max());

static_assert(std::numeric_limits<qp_double>::max().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::max());
static_assert(std::numeric_limits<qp_int>::max().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::max());

// lowest
static_assert(std::numeric_limits<qp_double>::lowest().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::lowest());
static_assert(std::numeric_limits<qp_int>::lowest().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::lowest());

// epsilon
static_assert(std::numeric_limits<qp_double>::epsilon().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::epsilon());
static_assert(std::numeric_limits<qp_int>::epsilon().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::epsilon());

// round_error
static_assert(std::numeric_limits<qp_double>::round_error().quantity_from_zero().numerical_value_in(
                mp_units::si::metre) == std::numeric_limits<double>::round_error());
static_assert(std::numeric_limits<qp_int>::round_error().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::round_error());

// infinity
static_assert(std::numeric_limits<qp_double>::infinity().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<double>::infinity());
static_assert(std::numeric_limits<qp_int>::infinity().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::infinity());

#if MP_UNITS_HOSTED && (__cpp_lib_constexpr_cmath || MP_UNITS_COMP_GCC)

// quiet_NaN
static_assert(
  std::isnan(std::numeric_limits<qp_double>::quiet_NaN().quantity_from_zero().numerical_value_in(mp_units::si::metre)));

// signaling_NaN
static_assert(std::isnan(
  std::numeric_limits<qp_double>::signaling_NaN().quantity_from_zero().numerical_value_in(mp_units::si::metre)));

#endif

// denorm_min
static_assert(std::numeric_limits<qp_double>::denorm_min().quantity_from_zero().numerical_value_in(
                mp_units::si::metre) == std::numeric_limits<double>::denorm_min());
static_assert(std::numeric_limits<qp_int>::denorm_min().quantity_from_zero().numerical_value_in(mp_units::si::metre) ==
              std::numeric_limits<int>::denorm_min());

}  // namespace
