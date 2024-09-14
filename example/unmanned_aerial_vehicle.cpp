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

#include "geographic.h"
#include "ranged_representation.h"
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#include <cassert>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstdint>
#include <iostream>
#include <string>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/bits/fmt.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace geographic;

// **** HAE ****

enum class earth_gravity_model : std::int8_t { egm84_15, egm95_5, egm2008_1 };

template<earth_gravity_model M>
struct height_above_ellipsoid_t final : absolute_point_origin<isq::altitude> {
  static constexpr earth_gravity_model egm = M;
};
template<earth_gravity_model M>
constexpr height_above_ellipsoid_t<M> height_above_ellipsoid;  // NOLINT(google-readability-casting)

template<earth_gravity_model M>
using hae_altitude = quantity_point<isq::altitude[si::metre], height_above_ellipsoid<M>>;

constexpr const char* to_text(earth_gravity_model m)
{
  switch (m) {
    using enum earth_gravity_model;
    case egm84_15:
      return "EGM84-15";
    case egm95_5:
      return "EGM95-5";
    case egm2008_1:
      return "EGM2008-1";
  }
  assert(false && "unsupported enum value");
}

template<earth_gravity_model M>
[[nodiscard]] consteval bool is_hae(height_above_ellipsoid_t<M>)
{
  return true;
}
[[nodiscard]] consteval bool is_hae(...) { return false; }

template<class CharT, class Traits, QuantityPoint QP>
  requires(is_hae(QP::absolute_point_origin))
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const QP& a)
{
  return os << a - a.absolute_point_origin << " HAE(" << to_text(a.absolute_point_origin.egm) << ")";
}

template<QuantityPoint QP, typename Char>
  requires(is_hae(QP::absolute_point_origin))
struct MP_UNITS_STD_FMT::formatter<QP, Char> : formatter<typename QP::quantity_type, Char> {
  template<typename FormatContext>
  auto format(const QP& a, FormatContext& ctx) const -> decltype(ctx.out())
  {
    formatter<typename QP::quantity_type, Char>::format(a - a.absolute_point_origin, ctx);
    return MP_UNITS_STD_FMT::format_to(ctx.out(), " HAE({})", to_text(QP::absolute_point_origin.egm));
  }
};

double GeographicLibWhatsMyOffset(long double /* lat */, long double /* lon */)
{
  // for example use GeographicLib for that:
  // - https://geographiclib.sourceforge.io/C++/doc/geoid.html
  // - https://conan.io/center/geographiclib
  return 29.49;
}

template<earth_gravity_model M>
hae_altitude<M> to_hae(msl_altitude msl, position<long double> pos)
{
  const auto geoid_undulation =
    isq::height(GeographicLibWhatsMyOffset(pos.lat.quantity_from_zero().numerical_value_in(si::degree),
                                           pos.lon.quantity_from_zero().numerical_value_in(si::degree)) *
                si::metre);
  return height_above_ellipsoid<M> + (msl - mean_sea_level - geoid_undulation);
}


// **** HAL ****

// clang-format off
inline constexpr struct height_above_launch final : absolute_point_origin<isq::altitude> {} height_above_launch;
// clang-format on

using hal_altitude = quantity_point<isq::altitude[si::metre], height_above_launch>;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const hal_altitude& a)
{
  return os << a.quantity_from(height_above_launch) << " HAL";
}

template<typename Char>
struct MP_UNITS_STD_FMT::formatter<hal_altitude, Char> : formatter<hal_altitude::quantity_type, Char> {
  template<typename FormatContext>
  auto format(const hal_altitude& a, FormatContext& ctx) const -> decltype(ctx.out())
  {
    formatter<hal_altitude::quantity_type, Char>::format(a.quantity_from(height_above_launch), ctx);
    return MP_UNITS_STD_FMT::format_to(ctx.out(), " HAL");
  }
};


// **** UAV ****

class unmanned_aerial_vehicle {
  msl_altitude current_ = mean_sea_level + 0 * si::metre;
  msl_altitude launch_ = current_;
public:
  void take_off(msl_altitude alt) { launch_ = alt; }
  [[nodiscard]] msl_altitude take_off() const { return launch_; }

  void current(msl_altitude alt) { current_ = alt; }
  [[nodiscard]] msl_altitude current() const { return current_; }

  [[nodiscard]] hal_altitude hal() const { return height_above_launch + current_.quantity_from(launch_); }
};


int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::international::unit_symbols;

  unmanned_aerial_vehicle uav;
  uav.take_off(mean_sea_level + 6'000 * ft);
  uav.current(mean_sea_level + 10'000 * ft);
  std::cout << MP_UNITS_STD_FMT::format("hal = {::N[.2f]}\n", uav.hal());

  const msl_altitude ground_level = mean_sea_level + 123 * m;
  std::cout << MP_UNITS_STD_FMT::format("agl = {::N[.2f]}\n", uav.current().quantity_from(ground_level));

  struct waypoint {
    std::string name;
    geographic::position<long double> pos;
    msl_altitude msl_alt;
  };

  const waypoint wpt = {"EPPR", {54.24772_N, 18.6745_E}, mean_sea_level + 16. * ft};
  std::cout << MP_UNITS_STD_FMT::format("{}: {} {}, {::N[.2f]}, {::N[.2f]}\n", wpt.name, wpt.pos.lat, wpt.pos.lon,
                                        wpt.msl_alt, to_hae<earth_gravity_model::egm2008_1>(wpt.msl_alt, wpt.pos));
}
