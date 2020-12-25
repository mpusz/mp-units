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

#include <units/bits/basic_concepts.h>
#include <units/customization_points.h>
#include <type_traits>

namespace units::detail {

struct one_rep {
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator*(const Rep& lhs, one_rep)
  {
    return lhs;
  }
  template<QuantityValue Rep>
  [[nodiscard]] friend constexpr Rep operator/(const Rep& lhs, one_rep)
  {
    return lhs;
  }

  template<QuantityValue Rep>
  [[nodiscard]] constexpr operator Rep() const noexcept
  {
    return quantity_values<Rep>::one();
  }

  [[nodiscard]] bool operator==(const one_rep&) const = default;
  [[nodiscard]] auto operator<=>(const one_rep&) const = default;
};

}  // namespace units::detail

namespace std {
template<>
struct common_type<units::detail::one_rep, units::detail::one_rep> {
  using type = units::detail::one_rep;
};
template<units::QuantityValue Rep>
struct common_type<units::detail::one_rep, Rep> {
  using type = Rep;
};
template<units::QuantityValue Rep>
struct common_type<Rep, units::detail::one_rep> {
  using type = Rep;
};

}  // namespace std
