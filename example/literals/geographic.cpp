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
#include <cmath>
#include <numbers>
#include <type_traits>

namespace {

using namespace units::isq::si::literals;
inline constexpr auto earth_radius = 6371._q_km;

}  // namespace

namespace geographic {

distance spherical_distance(position from, position to)
{
  using rep = std::common_type_t<latitude::value_type, longitude::value_type>;
  constexpr auto p = std::numbers::pi_v<rep> / 180;
  const auto lat1 = from.lat.value() * p;
  const auto lon1 = from.lon.value() * p;
  const auto lat2 = to.lat.value() * p;
  const auto lon2 = to.lon.value() * p;

  using std::sin, std::cos, std::asin, std::sqrt;

  // https://en.wikipedia.org/wiki/Great-circle_distance#Formulae
  if constexpr (sizeof(rep) >= 8) {
    // spherical law of cosines
    const auto central_angle = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1));
    // const auto central_angle = 2 * asin(sqrt(0.5 - cos(lat2 - lat1) / 2 + cos(lat1) * cos(lat2) * (1 - cos(lon2 - lon1)) / 2));
    return distance(earth_radius * central_angle);
  } else {
    // the haversine formula
    const auto sin_lat = sin(lat2 - lat1) / 2;
    const auto sin_lon = sin(lon2 - lon1) / 2;
    const auto central_angle = 2 * asin(sqrt(sin_lat * sin_lat + cos(lat1) * cos(lat2) * sin_lon * sin_lon));
    return distance(earth_radius * central_angle);
  }
}

}  // namespace geographic
