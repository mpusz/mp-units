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

#include <units/bits/fmt_hacks.h>
#include <units/isq/si/length.h>
#include <units/quantity_kind.h>

#include <limits>
#include <ostream>

// IWYU pragma: begin_exports
#include <compare>
// IWYU pragma: end_exports

namespace geographic {

template<typename Derived, typename Rep>
struct coordinate {
  using value_type = Rep;
  constexpr explicit coordinate(value_type v) : value_(v) {}
  constexpr value_type value() const { return value_; }
  auto operator<=>(const coordinate&) const = default;
private:
  value_type value_;
};

struct latitude : coordinate<struct latitude_, double> {
  using coordinate::coordinate;
};

struct longitude : coordinate<struct longitude_, double> {
  using coordinate::coordinate;
};

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const latitude& lat)
{
  if (lat.value() > 0)
    return os << "N" << lat.value();
  else
    return os << "S" << -lat.value();
}

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const longitude& lon)
{
  if (lon.value() > 0)
    return os << "E" << lon.value();
  else
    return os << "W" << -lon.value();
}

inline namespace literals {

constexpr auto operator"" _N(unsigned long long v) { return latitude(static_cast<latitude::value_type>(v)); }
constexpr auto operator"" _N(long double v) { return latitude(static_cast<latitude::value_type>(v)); }
constexpr auto operator"" _S(unsigned long long v) { return latitude(-static_cast<latitude::value_type>(v)); }
constexpr auto operator"" _S(long double v) { return latitude(-static_cast<latitude::value_type>(v)); }
constexpr auto operator"" _E(unsigned long long v) { return longitude(static_cast<longitude::value_type>(v)); }
constexpr auto operator"" _E(long double v) { return longitude(static_cast<longitude::value_type>(v)); }
constexpr auto operator"" _W(unsigned long long v) { return longitude(-static_cast<longitude::value_type>(v)); }
constexpr auto operator"" _W(long double v) { return longitude(-static_cast<longitude::value_type>(v)); }

}  // namespace literals

}  // namespace geographic

template<>
class std::numeric_limits<geographic::latitude> : public numeric_limits<geographic::latitude::value_type> {
  static constexpr auto min() noexcept { return geographic::latitude(-90); }
  static constexpr auto lowest() noexcept { return geographic::latitude(-90); }
  static constexpr auto max() noexcept { return geographic::latitude(90); }
};

template<>
class std::numeric_limits<geographic::longitude> : public numeric_limits<geographic::longitude::value_type> {
  static constexpr auto min() noexcept { return geographic::longitude(-180); }
  static constexpr auto lowest() noexcept { return geographic::longitude(-180); }
  static constexpr auto max() noexcept { return geographic::longitude(180); }
};

template<>
struct STD_FMT::formatter<geographic::latitude> : formatter<geographic::latitude::value_type> {
  template<typename FormatContext>
  auto format(geographic::latitude lat, FormatContext& ctx)
  {
    STD_FMT::format_to(ctx.out(), FMT_RUNTIME(lat.value() > 0 ? "N" : "S"));
    return formatter<geographic::latitude::value_type>::format(lat.value() > 0 ? lat.value() : -lat.value(), ctx);
  }
};

template<>
struct STD_FMT::formatter<geographic::longitude> : formatter<geographic::longitude::value_type> {
  template<typename FormatContext>
  auto format(geographic::longitude lon, FormatContext& ctx)
  {
    STD_FMT::format_to(ctx.out(), FMT_RUNTIME(lon.value() > 0 ? "E" : "W"));
    return formatter<geographic::longitude::value_type>::format(lon.value() > 0 ? lon.value() : -lon.value(), ctx);
  }
};

namespace geographic {

struct horizontal_kind : units::kind<horizontal_kind, units::isq::si::dim_length> {};
using distance = units::quantity_kind<horizontal_kind, units::isq::si::kilometre>;

struct position {
  latitude lat;
  longitude lon;
};

distance spherical_distance(position from, position to);

}  // namespace geographic
