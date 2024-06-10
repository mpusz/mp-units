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

#include "ranged_representation.h"
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#include <compare>
#include <limits>
#include <numbers>
#include <ostream>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

namespace geographic {

inline constexpr struct mean_sea_level : mp_units::absolute_point_origin<mean_sea_level, mp_units::isq::altitude> {
} mean_sea_level;

using msl_altitude = mp_units::quantity_point<mp_units::isq::altitude[mp_units::si::metre], mean_sea_level>;

// text output
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const msl_altitude& a)
{
  return os << a - mean_sea_level << " AMSL";
}

}  // namespace geographic

template<typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::msl_altitude, Char> :
    formatter<geographic::msl_altitude::quantity_type, Char> {
  template<typename FormatContext>
  auto format(const geographic::msl_altitude& a, FormatContext& ctx) const -> decltype(ctx.out())
  {
    ctx.advance_to(
      formatter<geographic::msl_altitude::quantity_type, Char>::format(a - geographic::mean_sea_level, ctx));
    return MP_UNITS_STD_FMT::format_to(ctx.out(), " AMSL");
  }
};

namespace geographic {

inline constexpr struct equator : mp_units::absolute_point_origin<equator, mp_units::isq::angular_measure> {
} equator;
inline constexpr struct prime_meridian :
    mp_units::absolute_point_origin<prime_meridian, mp_units::isq::angular_measure> {
} prime_meridian;


template<typename T = double>
using latitude = mp_units::quantity_point<mp_units::si::degree, equator, ranged_representation<T, -90, 90>>;

template<typename T = double>
using longitude = mp_units::quantity_point<mp_units::si::degree, prime_meridian, ranged_representation<T, -180, 180>>;

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const latitude<T>& lat)
{
  const auto& q = lat.quantity_ref_from(geographic::equator);
  return (is_gteq_zero(q)) ? (os << q << " N") : (os << -q << " S");
}

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const longitude<T>& lon)
{
  const auto& q = lon.quantity_ref_from(geographic::prime_meridian);
  return (is_gteq_zero(q)) ? (os << q << " E") : (os << -q << " W");
}

inline namespace literals {

constexpr latitude<long double> operator"" _N(long double v)
{
  return equator + ranged_representation<long double, -90, 90>{v} * mp_units::si::degree;
}
constexpr latitude<long double> operator"" _S(long double v)
{
  return equator - ranged_representation<long double, -90, 90>{v} * mp_units::si::degree;
}
constexpr longitude<long double> operator"" _E(long double v)
{
  return prime_meridian + ranged_representation<long double, -180, 180>{v} * mp_units::si::degree;
}
constexpr longitude<long double> operator"" _W(long double v)
{
  return prime_meridian - ranged_representation<long double, -180, 180>{v} * mp_units::si::degree;
}

}  // namespace literals

}  // namespace geographic

template<typename T>
class std::numeric_limits<geographic::latitude<T>> : public numeric_limits<T> {
  static constexpr auto min() noexcept { return geographic::latitude<T>(-90); }
  static constexpr auto lowest() noexcept { return geographic::latitude<T>(-90); }
  static constexpr auto max() noexcept { return geographic::latitude<T>(90); }
};

template<typename T>
class std::numeric_limits<geographic::longitude<T>> : public numeric_limits<T> {
  static constexpr auto min() noexcept { return geographic::longitude<T>(-180); }
  static constexpr auto lowest() noexcept { return geographic::longitude<T>(-180); }
  static constexpr auto max() noexcept { return geographic::longitude<T>(180); }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::latitude<T>, Char> :
    formatter<typename geographic::latitude<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::latitude<T> lat, FormatContext& ctx) const -> decltype(ctx.out())
  {
    const auto& q = lat.quantity_ref_from(geographic::equator);
    ctx.advance_to(
      formatter<typename geographic::latitude<T>::quantity_type, Char>::format(is_gteq_zero(q) ? q : -q, ctx));
    return MP_UNITS_STD_FMT::format_to(ctx.out(), "{}", is_gteq_zero(q) ? " N" : "S");
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::longitude<T>, Char> :
    formatter<typename geographic::longitude<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::longitude<T> lon, FormatContext& ctx) const -> decltype(ctx.out())
  {
    const auto& q = lon.quantity_ref_from(geographic::prime_meridian);
    ctx.advance_to(
      formatter<typename geographic::longitude<T>::quantity_type, Char>::format(is_gteq_zero(q) ? q : -q, ctx));
    return MP_UNITS_STD_FMT::format_to(ctx.out(), "{}", is_gteq_zero(q) ? " E" : " W");
  }
};

namespace geographic {

using distance = mp_units::quantity<mp_units::isq::distance[mp_units::si::kilo<mp_units::si::metre>]>;

template<typename T>
struct position {
  latitude<T> lat;
  longitude<T> lon;
};

template<typename T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
distance spherical_distance(position<T> from, position<T> to)
{
  using namespace mp_units;
  constexpr quantity earth_radius = 6'371 * isq::radius[si::kilo<si::metre>];

  using si::sin, si::cos, si::asin, si::acos;

  const quantity from_lat = from.lat.quantity_from_zero();
  const quantity from_lon = from.lon.quantity_from_zero();
  const quantity to_lat = to.lat.quantity_from_zero();
  const quantity to_lon = to.lon.quantity_from_zero();

  // https://en.wikipedia.org/wiki/Great-circle_distance#Formulae
  if constexpr (sizeof(T) >= 8) {
    // spherical law of cosines
    const quantity central_angle =
      acos(sin(from_lat) * sin(to_lat) + cos(from_lat) * cos(to_lat) * cos(to_lon - from_lon));
    // const auto central_angle = 2 * asin(sqrt(0.5 - cos(to_lat - from_lat) / 2 + cos(from_lat) * cos(to_lat) * (1
    // - cos(lon2_rad - from_lon)) / 2));

    return quantity_cast<isq::distance>(earth_radius * central_angle);
  } else {
    // the haversine formula
    const quantity sin_lat = sin((to_lat - from_lat) / 2);
    const quantity sin_lon = sin((to_lon - from_lon) / 2);
    const quantity central_angle = 2 * asin(sqrt(sin_lat * sin_lat + cos(from_lat) * cos(to_lat) * sin_lon * sin_lon));

    return quantity_cast<isq::distance>(earth_radius * central_angle);
  }
}

}  // namespace geographic
