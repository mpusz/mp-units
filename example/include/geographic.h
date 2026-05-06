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

#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <compare>
#include <limits>
#include <numbers>
#include <ostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework.h>
#include <mp-units/math.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

namespace geographic {

inline constexpr struct mean_sea_level final : mp_units::absolute_point_origin<mp_units::isq::altitude> {
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

// quantity specifications for geographic coordinates and orientation angles
//
// Geographic coordinates use different wrapping behaviors:
// - latitude: symmetric/reflects at ±90° (can't go past poles)
// - longitude: mirrored wrapping [-180°, 180°) — half-open interval, 180° wraps to -180°
// - elevation: symmetric/reflects at ±90° (like latitude)
//
// Orientation angles have different zero references and rotation directions.
// All use mirrored wrapping [-180°, 180°) — half-open interval where max is exclusive:
// - geometric_azimuth: 0° = East, increases counter-clockwise
// - bearing: 0° = North, increases clockwise
//   Conversion: bearing = 90° - geometric_azimuth
// - heading_azimuth: 0° = North, increases counter-clockwise
//   Conversion: heading = geometric_azimuth - 90°
//
// All geographic quantity specs are marked with is_kind to prevent accidental mixing
// (e.g., latitude + longitude, bearing + heading) and to require explicit conversions
// between different angle reference frames.
//
// For trigonometric functions (sin/cos/etc.), explicit conversion to angular_measure is needed:
//   const quantity<angular_measure> angle = isq::angular_measure(lat.quantity_from(equator));
//   sin(angle);  // Now works with plain angular_measure

// Basic geographic coordinates
QUANTITY_SPEC(geo_latitude, mp_units::isq::angular_measure, mp_units::is_kind);
QUANTITY_SPEC(geo_longitude, mp_units::isq::angular_measure, mp_units::is_kind);
QUANTITY_SPEC(geo_elevation, mp_units::isq::angular_measure, mp_units::is_kind);

// Orientation angles (different zero references and rotation directions)
QUANTITY_SPEC(geometric_azimuth, mp_units::isq::angular_measure, mp_units::is_kind);
QUANTITY_SPEC(geo_bearing, mp_units::isq::angular_measure, mp_units::is_kind);
QUANTITY_SPEC(heading_azimuth, mp_units::isq::angular_measure, mp_units::is_kind);

// Note: equator carries no `reflect_in_range` bounds. Latitude reflection at the
// poles is coupled with a 180° longitude shift, which a single-axis policy
// cannot express. The coupled normalization lives in `position`'s constructor.
inline constexpr struct equator final : mp_units::absolute_point_origin<geo_latitude> {
} equator;

inline constexpr struct prime_meridian final :
    mp_units::absolute_point_origin<geo_longitude,
                                    mp_units::wrap_to_range{-180 * mp_units::si::degree, 180 * mp_units::si::degree}> {
} prime_meridian;

inline constexpr struct horizon final :
    mp_units::absolute_point_origin<geo_elevation,
                                    mp_units::reflect_in_range{-90 * mp_units::si::degree, 90 * mp_units::si::degree}> {
} horizon;

// Geometric azimuth: 0° = East, counter-clockwise positive, mirrored wrapping [-180°, 180°)
inline constexpr struct east final :
    mp_units::absolute_point_origin<geometric_azimuth,
                                    mp_units::wrap_to_range{-180 * mp_units::si::degree, 180 * mp_units::si::degree}> {
} east;

// Bearing: 0° = North, clockwise positive
// Note: bearing = 90° - geometric_azimuth (involves sign flip - cannot use relative_point_origin)
inline constexpr struct north_cw final :
    mp_units::absolute_point_origin<geo_bearing,
                                    mp_units::wrap_to_range{-180 * mp_units::si::degree, 180 * mp_units::si::degree}> {
} north_cw;

// Heading azimuth: 0° = North, counter-clockwise positive (heading = geometric_azimuth - 90°)
// Implemented as a relative origin: offset -90° from east
inline constexpr struct north_ccw final :
    mp_units::relative_point_origin<east - 90.0 * mp_units::si::degree,
                                    mp_units::wrap_to_range{-180 * mp_units::si::degree, 180 * mp_units::si::degree}> {
} north_ccw;

template<typename T = double>
using latitude = mp_units::quantity_point<geo_latitude[mp_units::si::degree], equator, T>;

template<typename T = double>
using longitude = mp_units::quantity_point<geo_longitude[mp_units::si::degree], prime_meridian, T>;

template<typename T = double>
using elevation = mp_units::quantity_point<geo_elevation[mp_units::si::degree], horizon, T>;

template<typename T = double>
using azimuth = mp_units::quantity_point<geometric_azimuth[mp_units::si::degree], east, T>;

template<typename T = double>
using bearing = mp_units::quantity_point<geo_bearing[mp_units::si::degree], north_cw, T>;

template<typename T = double>
using heading = mp_units::quantity_point<heading_azimuth[mp_units::si::degree], north_ccw, T>;

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

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const elevation<T>& elev)
{
  return os << elev.quantity_ref_from(geographic::horizon);
}

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const azimuth<T>& az)
{
  return os << "Az " << az.quantity_ref_from(geographic::east);
}

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const bearing<T>& brg)
{
  return os << "BRG " << brg.quantity_ref_from(geographic::north_cw);
}

template<class CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const heading<T>& hdg)
{
  return os << "HDG " << hdg.quantity_ref_from(geographic::north_ccw);
}

inline namespace literals {

constexpr latitude<double> operator""_N(long double v)
{
  return equator + static_cast<double>(v) * geo_latitude[mp_units::si::degree];
}

constexpr latitude<double> operator""_S(long double v)
{
  return equator - static_cast<double>(v) * geo_latitude[mp_units::si::degree];
}

constexpr longitude<double> operator""_E(long double v)
{
  return prime_meridian + static_cast<double>(v) * geo_longitude[mp_units::si::degree];
}

constexpr longitude<double> operator""_W(long double v)
{
  return prime_meridian - static_cast<double>(v) * geo_longitude[mp_units::si::degree];
}

}  // namespace literals

}  // namespace geographic

// Note: No std::numeric_limits specializations needed!
// The generic specialization in quantity_point.h automatically handles all bounded quantity_points
// by querying the bounds from the origin's NTTP parameter.

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::latitude<T>, Char> :
    formatter<typename geographic::latitude<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::latitude<T> lat, FormatContext& ctx) const -> decltype(ctx.out())
  {
    const auto& q = lat.quantity_ref_from(geographic::equator);
    ctx.advance_to(formatter<typename geographic::latitude<T>::quantity_type, Char>::format(q >= 0 ? q : -q, ctx));
    return MP_UNITS_STD_FMT::format_to(ctx.out(), "{}", q >= 0 ? " N" : " S");
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::longitude<T>, Char> :
    formatter<typename geographic::longitude<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::longitude<T> lon, FormatContext& ctx) const -> decltype(ctx.out())
  {
    const auto& q = lon.quantity_ref_from(geographic::prime_meridian);
    ctx.advance_to(formatter<typename geographic::longitude<T>::quantity_type, Char>::format(q >= 0 ? q : -q, ctx));
    return MP_UNITS_STD_FMT::format_to(ctx.out(), "{}", q >= 0 ? " E" : " W");
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::elevation<T>, Char> :
    formatter<typename geographic::elevation<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::elevation<T> elev, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return formatter<typename geographic::elevation<T>::quantity_type, Char>::format(
      elev.quantity_ref_from(geographic::horizon), ctx);
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::azimuth<T>, Char> :
    formatter<typename geographic::azimuth<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::azimuth<T> az, FormatContext& ctx) const -> decltype(ctx.out())
  {
    ctx.advance_to(MP_UNITS_STD_FMT::format_to(ctx.out(), "Az "));
    return formatter<typename geographic::azimuth<T>::quantity_type, Char>::format(
      az.quantity_ref_from(geographic::east), ctx);
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::bearing<T>, Char> :
    formatter<typename geographic::bearing<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::bearing<T> brg, FormatContext& ctx) const -> decltype(ctx.out())
  {
    ctx.advance_to(MP_UNITS_STD_FMT::format_to(ctx.out(), "BRG "));
    return formatter<typename geographic::bearing<T>::quantity_type, Char>::format(
      brg.quantity_ref_from(geographic::north_cw), ctx);
  }
};

template<typename T, typename Char>
struct MP_UNITS_STD_FMT::formatter<geographic::heading<T>, Char> :
    formatter<typename geographic::heading<T>::quantity_type, Char> {
  template<typename FormatContext>
  auto format(geographic::heading<T> hdg, FormatContext& ctx) const -> decltype(ctx.out())
  {
    ctx.advance_to(MP_UNITS_STD_FMT::format_to(ctx.out(), "HDG "));
    return formatter<typename geographic::heading<T>::quantity_type, Char>::format(
      hdg.quantity_ref_from(geographic::north_ccw), ctx);
  }
};

namespace geographic {

using distance = mp_units::quantity<mp_units::isq::distance[mp_units::si::kilo<mp_units::si::metre>]>;

// A geographic position couples latitude and longitude: reflecting latitude at a
// pole requires shifting longitude by 180°. Because this constraint spans both
// axes, it cannot be expressed by a single-axis policy on `equator`; instead the
// constructor performs the coupled normalization, leaving `prime_meridian`'s
// `wrap_to_range` to handle the resulting longitude wrap.
template<typename T>
class position {
public:
  latitude<T> lat;
  longitude<T> lon;

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr position(latitude<T> lat_in, longitude<T> lon_in) noexcept : lon(lon_in)
  {
    using mp_units::quantity;
    using mp_units::si::degree;

    constexpr quantity half_turn = T{180} * degree;
    constexpr quantity full_turn = T{360} * degree;
    constexpr quantity quarter_turn = T{90} * degree;

    // Latitude reflection (`half_turn - lat_q`) is not defined on points, so the
    // normalization is done in displacement space relative to the equator.
    quantity lat_q = lat_in.quantity_from(equator);

    // Fold latitude into [-180°, 180°] first.
    lat_q = fmod(lat_q + half_turn, full_turn);
    if (lat_q < lat_q.zero()) lat_q += full_turn;
    lat_q -= half_turn;

    // Reflect at the poles, shifting longitude by 180°; wrap_to_range on
    // prime_meridian normalizes the longitude back into (-180°, 180°].
    if (lat_q > quarter_turn) {
      lat_q = half_turn - lat_q;
      lon += half_turn;
    } else if (lat_q < -quarter_turn) {
      lat_q = -half_turn - lat_q;
      lon += half_turn;
    }

    lat = equator + lat_q;
  }

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  friend distance spherical_distance(position from, position to)
  {
    using namespace mp_units;
    constexpr quantity earth_radius = 6'371 * isq::radius[si::kilo<si::metre>];

    using si::sin, si::cos, si::asin, si::acos;

    const quantity from_lat = isq::angular_measure(from.lat.quantity_ref_from(equator));
    const quantity from_lon = isq::angular_measure(from.lon.quantity_ref_from(prime_meridian));
    const quantity to_lat = isq::angular_measure(to.lat.quantity_ref_from(equator));
    const quantity to_lon = isq::angular_measure(to.lon.quantity_ref_from(prime_meridian));

    // https://en.wikipedia.org/wiki/Great-circle_distance#Formulae
    if constexpr (sizeof(T) >= 8) {
      // spherical law of cosines
      const quantity central_angle =
        acos(sin(from_lat) * sin(to_lat) + cos(from_lat) * cos(to_lat) * cos(to_lon - from_lon));
      // const auto central_angle = 2 * asin(sqrt(0.5 - cos(to_lat - from_lat) / 2 + cos(from_lat) * cos(to_lat) * (1
      // - cos(lon2_rad - from_lon)) / 2));

      return quantity_cast<isq::distance>((earth_radius * central_angle).in(earth_radius.unit));
    } else {
      // the haversine formula
      const quantity sin_lat = sin((to_lat - from_lat) / 2);
      const quantity sin_lon = sin((to_lon - from_lon) / 2);
      const quantity central_angle =
        2 * asin(sqrt(sin_lat * sin_lat + cos(from_lat) * cos(to_lat) * sin_lon * sin_lon));

      return quantity_cast<isq::distance>((earth_radius * central_angle).in(earth_radius.unit));
    }
  }
};

}  // namespace geographic
