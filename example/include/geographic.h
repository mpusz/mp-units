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
#include <mp-units/bits/fmt_hacks.h>
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/quantity.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/units.h>
#include <compare>
#include <limits>
#include <numbers>
#include <ostream>

namespace geographic {

inline constexpr struct mean_sea_level : mp_units::absolute_point_origin<mp_units::isq::altitude> {
  using mp_units::absolute_point_origin<mp_units::isq::altitude>::absolute_point_origin;
} mean_sea_level;

using msl_altitude = mp_units::quantity_point<mp_units::isq::altitude[mp_units::si::metre], mean_sea_level>;

// text output
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const msl_altitude& a)
{
  return os << a.absolute() << " AMSL";
}

}  // namespace geographic

template<>
struct STD_FMT::formatter<geographic::msl_altitude> : formatter<geographic::msl_altitude::quantity_type> {
  template<typename FormatContext>
  auto format(const geographic::msl_altitude& a, FormatContext& ctx)
  {
    formatter<geographic::msl_altitude::quantity_type>::format(a.absolute(), ctx);
    return STD_FMT::format_to(ctx.out(), " AMSL");
  }
};

namespace geographic {

template<typename T = double>
using latitude =
  mp_units::quantity<mp_units::isq::angular_measure[mp_units::si::degree], ranged_representation<T, -90, 90>>;

template<typename T = double>
using longitude =
  mp_units::quantity<mp_units::isq::angular_measure[mp_units::si::degree], ranged_representation<T, -180, 180>>;

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const latitude<T>& lat)
{
  if (lat.number() > 0)
    return os << "N" << lat.number();
  else
    return os << "S" << -lat.number();
}

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const longitude<T>& lon)
{
  if (lon.number() > 0)
    return os << "E" << lon.number();
  else
    return os << "W" << -lon.number();
}

inline namespace literals {

constexpr latitude<long double> operator"" _N(long double v) { return latitude<long double>{v * mp_units::si::degree}; }
constexpr latitude<long double> operator"" _S(long double v)
{
  return latitude<long double>{-v * mp_units::si::degree};
}
constexpr longitude<long double> operator"" _E(long double v)
{
  return longitude<long double>{v * mp_units::si::degree};
}
constexpr longitude<long double> operator"" _W(long double v)
{
  return longitude<long double>{-v * mp_units::si::degree};
}
constexpr latitude<std::int64_t> operator"" _N(unsigned long long v)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(v));
  return latitude<std::int64_t>{static_cast<std::int64_t>(v) * mp_units::si::degree};
}
constexpr latitude<std::int64_t> operator"" _S(unsigned long long v)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(v));
  return latitude<std::int64_t>{-static_cast<std::int64_t>(v) * mp_units::si::degree};
}
constexpr longitude<std::int64_t> operator"" _E(unsigned long long v)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(v));
  return longitude<std::int64_t>{static_cast<std::int64_t>(v) * mp_units::si::degree};
}
constexpr longitude<std::int64_t> operator"" _W(unsigned long long v)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(v));
  return longitude<std::int64_t>{-static_cast<std::int64_t>(v) * mp_units::si::degree};
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

template<typename T>
struct STD_FMT::formatter<geographic::latitude<T>> : formatter<T> {
  template<typename FormatContext>
  auto format(geographic::latitude<T> lat, FormatContext& ctx)
  {
    STD_FMT::format_to(ctx.out(), "{}", lat > geographic::latitude<T>::zero() ? 'N' : 'S');
    return formatter<T>::format(lat > geographic::latitude<T>::zero() ? lat.number() : -lat.number(), ctx);
  }
};

template<typename T>
struct STD_FMT::formatter<geographic::longitude<T>> : formatter<T> {
  template<typename FormatContext>
  auto format(geographic::longitude<T> lon, FormatContext& ctx)
  {
    STD_FMT::format_to(ctx.out(), "{}", lon > geographic::longitude<T>::zero() ? 'E' : 'W');
    return formatter<T>::format(lon > geographic::longitude<T>::zero() ? lon.number() : -lon.number(), ctx);
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
distance spherical_distance(position<T> from, position<T> to)
{
  using namespace mp_units;
  constexpr auto earth_radius = 6'371 * isq::radius[si::kilo<si::metre>];

  using isq::sin, isq::cos, isq::asin, isq::acos;

  // https://en.wikipedia.org/wiki/Great-circle_distance#Formulae
  if constexpr (sizeof(T) >= 8) {
    // spherical law of cosines
    const auto central_angle = acos(sin(from.lat) * sin(to.lat) + cos(from.lat) * cos(to.lat) * cos(to.lon - from.lon));
    // const auto central_angle = 2 * asin(sqrt(0.5 - cos(to.lat - from.lat) / 2 + cos(from.lat) * cos(to.lat) * (1
    // - cos(lon2_rad - from.lon)) / 2));

    return quantity_cast<isq::distance>(earth_radius * central_angle);
  } else {
    // the haversine formula
    const auto sin_lat = sin((to.lat - from.lat) / 2);
    const auto sin_lon = sin((to.lon - from.lon) / 2);
    const auto central_angle = 2 * asin(sqrt(sin_lat * sin_lat + cos(from.lat) * cos(to.lat) * sin_lon * sin_lon));

    return quantity_cast<isq::distance>(earth_radius * central_angle);
  }
}

}  // namespace geographic
