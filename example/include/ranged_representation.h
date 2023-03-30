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

#include "validated_type.h"
#include <units/bits/external/hacks.h>
#include <algorithm>
#include <concepts>
#include <type_traits>

template<std::movable T, UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Min,
         UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Max>
inline constexpr auto is_in_range = [](const auto& v) { return std::clamp(v, T{Min}, T{Max}) == v; };

template<std::movable T, UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Min,
         UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Max>
using is_in_range_t = decltype(is_in_range<T, Min, Max>);

template<std::movable T, UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Min,
         UNITS_CONSTRAINED_NTTP_WORKAROUND(std::convertible_to<T>) auto Max>
class ranged_representation : public validated_type<T, is_in_range_t<T, Min, Max>> {
public:
  using validated_type<T, is_in_range_t<T, Min, Max>>::validated_type;
  constexpr ranged_representation() : validated_type<T, is_in_range_t<T, Min, Max>>(T{}) {}

  [[nodiscard]] constexpr ranged_representation operator-() const
    requires requires(T t) { -t; }
  {
    return ranged_representation(-this->value());
  }
};

template<typename T, auto Min, auto Max>
struct std::common_type<std::intmax_t, ranged_representation<T, Min, Max>> :
    std::type_identity<ranged_representation<std::common_type_t<std::intmax_t, T>, Min, Max>> {};

template<typename T, auto Min, auto Max>
struct std::common_type<ranged_representation<T, Min, Max>, std::intmax_t> :
    std::type_identity<ranged_representation<std::common_type_t<T, std::intmax_t>, Min, Max>> {};
