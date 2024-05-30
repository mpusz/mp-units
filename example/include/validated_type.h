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

#include <mp-units/bits/hacks.h>
#include <mp-units/compat_macros.h>
#include <compare>  // IWYU pragma: export
#include <ostream>
#include <utility>
#ifdef MP_UNITS_MODULES
import mp_units.core;
#else
#include <mp-units/bits/fmt.h>
#include <mp-units/framework/customization_points.h>
#endif

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
    MP_UNITS_EXPECTS(validate(value_));
  }

  constexpr explicit validated_type(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) :
      value_(std::move(value))
  {
    MP_UNITS_EXPECTS(validate(value_));
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


#if MP_UNITS_COMP_MSVC && MP_UNITS_COMP_MSVC < 1930

  constexpr explicit(false) operator T() const noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::copyable<T>
  {
    return value_;
  }

#else

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) operator T() const& noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires std::copyable<T>
  {
    return value_;
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr explicit(false) operator T() && noexcept(std::is_nothrow_move_constructible_v<T>)
  {
    return std::move(value_);
  }

#endif

  constexpr T& value() & noexcept = delete;
  [[nodiscard]] constexpr const T& value() const& noexcept { return value_; }
  [[nodiscard]] constexpr T&& value() && noexcept { return std::move(value_); }
  [[nodiscard]] constexpr const T&& value() const&& noexcept { return std::move(value_); }

  [[nodiscard]] bool operator==(const validated_type&) const
    requires std::equality_comparable<T>
  = default;
  [[nodiscard]] auto operator<=>(const validated_type&) const
    requires std::three_way_comparable<T>
  = default;
};

template<typename T, typename Validator>
inline constexpr bool mp_units::is_scalar<validated_type<T, Validator>> = mp_units::is_scalar<T>;

template<typename T, typename Validator>
inline constexpr bool mp_units::treat_as_floating_point<validated_type<T, Validator>> =
  mp_units::treat_as_floating_point<T>;


template<typename CharT, typename Traits, typename T, typename Validator>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              const validated_type<T, Validator>& v)
  requires requires { os << v.value(); }
{
  return os << v.value();
}


template<typename T, typename Validator, typename Char>
struct MP_UNITS_STD_FMT::formatter<validated_type<T, Validator>, Char> : formatter<T, Char> {
  template<typename FormatContext>
  auto format(const validated_type<T, Validator>& v, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return formatter<T, Char>::format(v.value(), ctx);
  }
};
