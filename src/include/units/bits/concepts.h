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

#include <units/bits/hacks.h>
#include <units/bits/numeric_concepts.h>
#include <units/bits/customization_points.h>
#include <units/ratio.h>

namespace units {

  namespace detail {

    template<typename T, typename U = T>
    concept basic_arithmetic = // exposition only
      std::magma<std::ranges::plus, T, U> &&
      std::magma<std::ranges::minus, T, U> &&
      std::magma<std::ranges::times, T, U> &&
      std::magma<std::ranges::divided_by, T, U>;

    template<typename From, typename To>
    concept safe_convertible = // exposition only
      std::convertible_to<From, To> &&
      (treat_as_floating_point<To> || (!treat_as_floating_point<From>));

    template<typename Rep, typename unit_from, typename unit_to>
    concept safe_divisible = // exposition only
      treat_as_floating_point<Rep> ||
      ratio_divide<typename unit_from::ratio, typename unit_to::ratio>::den == 1;

  }

}  // namespace units
