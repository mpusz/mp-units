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

#include <gsl/gsl-lite.hpp>
#include <units/bits/external/hacks.h>
#include <utility>

inline constexpr struct validated_tag {
} validated;

template<std::movable T, std::predicate<T> Validator>
class validated_type {
  T value_;
public:
  using value_type = T;

  static constexpr bool validate(const T& value) { return Validator()(value); }

  constexpr explicit validated_type(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::copyable<T>
      : value_(value)
  {
    gsl_Expects(validate(value_));
  }

  constexpr explicit validated_type(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) :
      value_(std::move(value))
  {
    gsl_Expects(validate(value_));
  }

  constexpr validated_type(const T& value, validated_tag) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::copyable<T>
      : value_(value)
  {
  }

  constexpr validated_type(T&& value, validated_tag) noexcept(std::is_nothrow_move_constructible_v<T>) :
      value_(std::move(value))
  {
  }

  constexpr explicit(false) operator T() const noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::copyable<T>
  {
    return value_;
  }

  constexpr T& value() & noexcept = delete;
  constexpr const T& value() const& noexcept { return value_; }
  constexpr T&& value() && noexcept { return std::move(value_); }
  constexpr const T&& value() const&& noexcept { return std::move(value_); }

  bool operator==(const validated_type&) const
    requires std::equality_comparable<T>
  = default;
  auto operator<=>(const validated_type&) const
    requires std::three_way_comparable<T>
  = default;
};
