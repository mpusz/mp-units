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
#include <mp-units/systems/angular/units.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/bits/value_cast.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>
#include <mp-units/unit.h>
#include <cmath>
#endif


MP_UNITS_EXPORT
namespace mp_units::angular {

template<ReferenceOf<angle> auto R, typename Rep>
  requires requires(Rep v) { sin(v); } || requires(Rep v) { std::sin(v); }
[[nodiscard]] inline QuantityOf<dimensionless> auto sin(const quantity<R, Rep>& q) noexcept
{
  using std::sin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(sin(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{sin(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{sin(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<angle> auto R, typename Rep>
  requires requires(Rep v) { cos(v); } || requires(Rep v) { std::cos(v); }
[[nodiscard]] inline QuantityOf<dimensionless> auto cos(const quantity<R, Rep>& q) noexcept
{
  using std::cos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(cos(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{cos(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{cos(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<angle> auto R, typename Rep>
  requires requires(Rep v) { tan(v); } || requires(Rep v) { std::tan(v); }
[[nodiscard]] inline QuantityOf<dimensionless> auto tan(const quantity<R, Rep>& q) noexcept
{
  using std::tan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(tan(q.force_numerical_value_in(radian)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{tan(value_cast<rep>(q).numerical_value_in(radian)), one};
  } else
    return quantity{tan(q.numerical_value_in(radian)), one};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires requires(Rep v) { asin(v); } || requires(Rep v) { std::asin(v); }
[[nodiscard]] inline QuantityOf<angle> auto asin(const quantity<R, Rep>& q) noexcept
{
  using std::asin;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(asin(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{asin(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{asin(q.numerical_value_in(one)), radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires requires(Rep v) { acos(v); } || requires(Rep v) { std::acos(v); }
[[nodiscard]] inline QuantityOf<angle> auto acos(const quantity<R, Rep>& q) noexcept
{
  using std::acos;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(acos(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{acos(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{acos(q.numerical_value_in(one)), radian};
}

template<ReferenceOf<dimensionless> auto R, typename Rep>
  requires requires(Rep v) { atan(v); } || requires(Rep v) { std::atan(v); }
[[nodiscard]] inline QuantityOf<angle> auto atan(const quantity<R, Rep>& q) noexcept
{
  using std::atan;
  if constexpr (!treat_as_floating_point<Rep>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan(q.force_numerical_value_in(one)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{atan(value_cast<rep>(q).numerical_value_in(one)), radian};
  } else
    return quantity{atan(q.numerical_value_in(one)), radian};
}

template<auto R1, typename Rep1, auto R2, typename Rep2>
  requires requires(Rep1 v1, Rep2 v2) {
    common_reference(R1, R2);
    requires requires { atan2(v1, v2); } || requires { std::atan2(v1, v2); };
  }
[[nodiscard]] inline QuantityOf<angle> auto atan2(const quantity<R1, Rep1>& y, const quantity<R2, Rep2>& x) noexcept
{
  constexpr auto ref = common_reference(R1, R2);
  constexpr auto unit = get_unit(ref);
  using std::atan2;
  if constexpr (!treat_as_floating_point<Rep1> || !treat_as_floating_point<Rep2>) {
    // check what is the return type when called with the integral value
    using rep = decltype(atan2(y.force_numerical_value_in(unit), x.force_numerical_value_in(unit)));
    // use this type ahead of calling the function to prevent narrowing if a unit conversion is needed
    return quantity{atan2(value_cast<rep>(y).numerical_value_in(unit), value_cast<rep>(x).numerical_value_in(unit)),
                    radian};
  } else
    return quantity{atan2(y.numerical_value_in(unit), x.numerical_value_in(unit)), radian};
}

}  // namespace mp_units::angular
