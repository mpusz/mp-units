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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Determines the mass of the Earth from two benchtop measurements:
//
//     g = 4π²L / T²        (simple pendulum)
//     M = g R² / G         (surface gravity of a sphere)

#include <mp-units/bits/hacks.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework.h>
#include <mp-units/math.h>
#include <mp-units/systems/codata/adopted_values.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/uncertain.h>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using mp_units::utility::measurement_of;
using mp_units::utility::uncertain;

// A pendulum measures the magnitude of the free-fall acceleration, which is a scalar, while
// `isq::acceleration` is a vector quantity with no scalar counterpart yet (V3 will model the
// magnitude of a vector).
QUANTITY_SPEC(gravity_magnitude, isq::speed / isq::duration);

[[nodiscard]] QuantityOf<gravity_magnitude> auto local_gravity(const QuantityOf<isq::distance> auto& pendulum_length,
                                                               const QuantityOf<isq::duration> auto& total_time,
                                                               int swings)
{
  const QuantityOf<isq::duration> auto period = total_time / swings;
  return gravity_magnitude(1 * (mag<4> * pow<2>(π)) * pendulum_length / pow<2>(period));
}

[[nodiscard]] QuantityOf<isq::mass> auto sphere_mass_from_surface_gravity(
  const QuantityOf<gravity_magnitude> auto& surface_gravity, const QuantityOf<isq::radius> auto& radius)
{
  const quantity G = measurement_of(iau::newtonian_constant_of_gravitation);
  return surface_gravity * pow<2>(radius) / G;
}

}  // namespace


int main()
{
  // A 1 m pendulum timed over 50 swings with a stopwatch, and the Earth's mean radius as
  // published. The uncertainties are what the instruments justify: half a millimetre on the
  // metre stick, six hundredths of a second on the total time, a hundred metres on the radius.
  const quantity pendulum_length = isq::distance(uncertain{1.0000, 0.0005} * m);
  const quantity total_time = uncertain{100.30, 0.06} * s;
  constexpr int swings = 50;
  const quantity earth_radius = isq::radius(uncertain{6371.0, 0.1} * km);

  const quantity gravity = local_gravity(pendulum_length, total_time, swings);
  const quantity earth_mass = sphere_mass_from_surface_gravity(gravity, earth_radius);

  std::cout << MP_UNITS_STD_FMT::format("pendulum length     = {}\n", pendulum_length);
  std::cout << MP_UNITS_STD_FMT::format("time for {} swings  = {}\n", swings, total_time);
  std::cout << MP_UNITS_STD_FMT::format("local gravity       = {::N[.4f]}\n", gravity.in(m / s2));
  // a conventional value fixed by the CGPM, not a measurement, so it carries no uncertainty
  std::cout << MP_UNITS_STD_FMT::format("  standard gravity  = {::N[.4f]}\n",
                                        (1. * codata::standard_gravity).in(m / s2));
  std::cout << MP_UNITS_STD_FMT::format("mass of the Earth   = {::N[.4e]}\n", earth_mass.in(kg));
  // the published mass is itself defined as (GM)⊕ᴺ/G, so requesting an uncertainty-capable
  // representation for the conversion reports how well `G` is known
  std::cout << MP_UNITS_STD_FMT::format("  accepted value    = {::N[.4e]}\n",
                                        (1. * iau::terrestrial_mass).in<uncertain<double>>(kg));

  // each contribution is scaled by the exponent its input enters the formula with
  const auto relative_uncertainty_of = [](const auto& quantity) {
    return quantity.numerical_value_in(quantity.unit).relative_uncertainty();
  };
  const auto from_length = relative_uncertainty_of(pendulum_length);
  const auto from_period = 2 * relative_uncertainty_of(total_time);
  const auto from_radius = 2 * relative_uncertainty_of(earth_radius);
  const auto from_G = get_value<double>(get_relative_standard_uncertainty(iau::newtonian_constant_of_gravitation));
  // independent contributions, so they combine in quadrature rather than by addition
  const auto combined = std::hypot(std::hypot(from_length, from_period), std::hypot(from_radius, from_G));

  std::cout << MP_UNITS_STD_FMT::format("\nrelative uncertainty contributions\n");
  std::cout << MP_UNITS_STD_FMT::format("  pendulum length        = {:.1e}\n", from_length);
  std::cout << MP_UNITS_STD_FMT::format("  period (squared)       = {:.1e}\n", from_period);
  std::cout << MP_UNITS_STD_FMT::format("  Earth radius (sq.)     = {:.1e}\n", from_radius);
  std::cout << MP_UNITS_STD_FMT::format("  G (CODATA 2018)        = {:.1e}\n", from_G);
  std::cout << MP_UNITS_STD_FMT::format("  combined in quadrature = {:.3e}\n", combined);
  std::cout << MP_UNITS_STD_FMT::format("  reported by the result = {:.3e}\n", relative_uncertainty_of(earth_mass));
}
