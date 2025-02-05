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
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

template<auto R, typename Rep>
  requires requires { typename std::numeric_limits<Rep>; }
class std::numeric_limits<mp_units::quantity<R, Rep>> : public std::numeric_limits<Rep> {
public:
  static constexpr mp_units::quantity<R, Rep> min() noexcept
    requires requires { mp_units::quantity<R, Rep>::min(); }
  {
    return mp_units::quantity<R, Rep>::min();
  }

  static constexpr mp_units::quantity<R, Rep> max() noexcept
    requires requires { mp_units::quantity<R, Rep>::max(); }
  {
    return mp_units::quantity<R, Rep>::max();
  }

  static constexpr mp_units::quantity<R, Rep> lowest() noexcept
    requires requires { std::numeric_limits<Rep>::lowest(); }
  {
    return {std::numeric_limits<Rep>::lowest(), R};
  }

  static constexpr mp_units::quantity<R, Rep> epsilon() noexcept
    requires requires { std::numeric_limits<Rep>::epsilon(); }
  {
    return {std::numeric_limits<Rep>::epsilon(), R};
  }

  static constexpr mp_units::quantity<R, Rep> round_error() noexcept
    requires requires { std::numeric_limits<Rep>::round_error(); }
  {
    return {std::numeric_limits<Rep>::round_error(), R};
  }

  static constexpr mp_units::quantity<R, Rep> infinity() noexcept
    requires requires { std::numeric_limits<Rep>::infinity(); }
  {
    return {std::numeric_limits<Rep>::infinity(), R};
  }

  static constexpr mp_units::quantity<R, Rep> quiet_NaN() noexcept
    requires requires { std::numeric_limits<Rep>::quiet_NaN(); }
  {
    return {std::numeric_limits<Rep>::quiet_NaN(), R};
  }

  static constexpr mp_units::quantity<R, Rep> signaling_NaN() noexcept
    requires requires { std::numeric_limits<Rep>::signaling_NaN(); }
  {
    return {std::numeric_limits<Rep>::signaling_NaN(), R};
  }

  static constexpr mp_units::quantity<R, Rep> denorm_min() noexcept
    requires requires { std::numeric_limits<Rep>::denorm_min(); }
  {
    return {std::numeric_limits<Rep>::denorm_min(), R};
  }
};


template<auto R, auto PO, typename Rep>
  requires requires { typename std::numeric_limits<Rep>; }
class std::numeric_limits<mp_units::quantity_point<R, PO, Rep>> : public std::numeric_limits<Rep> {
public:
  static constexpr mp_units::quantity_point<R, PO, Rep> min() noexcept
    requires requires { mp_units::quantity_point<R, PO, Rep>::min(); }
  {
    return mp_units::quantity_point<R, PO, Rep>::min();
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> max() noexcept
    requires requires { mp_units::quantity_point<R, PO, Rep>::max(); }
  {
    return mp_units::quantity_point<R, PO, Rep>::max();
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> lowest() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::lowest(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::lowest(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> epsilon() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::epsilon(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::epsilon(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> round_error() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::round_error(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::round_error(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> infinity() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::infinity(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::infinity(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> quiet_NaN() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::quiet_NaN(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::quiet_NaN(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> signaling_NaN() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::signaling_NaN(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::signaling_NaN(), PO};
  }

  static constexpr mp_units::quantity_point<R, PO, Rep> denorm_min() noexcept
    requires requires { std::numeric_limits<mp_units::quantity<R, Rep>>::denorm_min(); }
  {
    return {std::numeric_limits<mp_units::quantity<R, Rep>>::denorm_min(), PO};
  }
};
