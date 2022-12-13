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

#include <units/isq/si/acceleration.h>
#include <units/isq/si/area.h>
#include <units/isq/si/energy.h>
#include <units/isq/si/force.h>
#include <units/isq/si/fps/fps.h>
#include <units/isq/si/length.h>
#include <units/isq/si/mass.h>
#include <units/isq/si/power.h>
#include <units/isq/si/pressure.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>
#include <units/math.h>

namespace {

using namespace units::isq;

static_assert(si::fps::length<si::fps::foot>(1) == si::length<si::metre>(0.3048));
static_assert(si::fps::mass<si::fps::pound>(1) == si::mass<si::kilogram>(0.45359237));
static_assert(si::fps::time<si::fps::second>(1) == si::time<si::second>(1));
static_assert(si::fps::speed<si::fps::foot_per_second>(1) == si::speed<si::metre_per_second>(0.3048));
static_assert(si::fps::area<si::fps::square_foot>(1) == si::area<si::square_metre>(0.09290304));
static_assert(si::fps::acceleration<si::fps::foot_per_second_sq>(1) ==
              si::acceleration<si::metre_per_second_sq>(0.3048));
static_assert(si::fps::force<si::fps::poundal>(1) > si::force<si::newton>(0.138254) &&
              si::fps::force<si::fps::poundal>(1) < si::force<si::newton>(0.138256));
static_assert(si::fps::energy<si::fps::foot_poundal>(1) > si::energy<si::joule>(0.042140110093804) &&
              si::fps::energy<si::fps::foot_poundal>(1) < si::energy<si::joule>(0.042140110093806));
static_assert(si::fps::power<si::fps::foot_poundal_per_second>(1) > si::power<si::watt>(0.042140110093804) &&
              si::fps::power<si::fps::foot_poundal_per_second>(1) < si::power<si::watt>(0.042140110093806));
static_assert(si::fps::pressure<si::fps::poundal_per_foot_sq>(1) > si::pressure<si::pascal>(1.4881639435) &&
              si::fps::pressure<si::fps::poundal_per_foot_sq>(1) < si::pressure<si::pascal>(1.4881639437));

namespace si_literals {

using namespace units::isq::si::literals;

static_assert(si::fps::length<si::fps::foot>(1) == 0.3048_q_m);
static_assert(si::fps::mass<si::fps::pound>(1) == 0.45359237_q_kg);
static_assert(si::fps::time<si::fps::second>(1) == 1_q_s);
static_assert(si::fps::speed<si::fps::foot_per_second>(1) == 0.3048_q_m_per_s);
static_assert(si::fps::area<si::fps::square_foot>(1) == 0.09290304_q_m2);
static_assert(si::fps::acceleration<si::fps::foot_per_second_sq>(1) == 0.3048_q_m_per_s2);
static_assert(si::fps::force<si::fps::poundal>(1) > 0.138254_q_N && si::fps::force<si::fps::poundal>(1) < 0.138256_q_N);
static_assert(si::fps::energy<si::fps::foot_poundal>(1) > 0.042140110093804_q_J &&
              si::fps::energy<si::fps::foot_poundal>(1) < 0.042140110093806_q_J);
static_assert(si::fps::power<si::fps::foot_poundal_per_second>(1) > 0.042140110093804_q_W &&
              si::fps::power<si::fps::foot_poundal_per_second>(1) < 0.042140110093806_q_W);
static_assert(si::fps::pressure<si::fps::poundal_per_foot_sq>(1) > 1.4881639435_q_Pa &&
              si::fps::pressure<si::fps::poundal_per_foot_sq>(1) < 1.4881639437_q_Pa);
}  // namespace si_literals

namespace fps_literals {

using namespace units::isq::si::fps::literals;
using namespace units::isq::si::fps::references;

static_assert(1 * ft == si::length<si::metre>(0.3048));
static_assert(1_q_lb == si::mass<si::kilogram>(0.45359237));
static_assert(1_q_s == si::time<si::second>(1));
static_assert(1_q_ft_per_s == si::speed<si::metre_per_second>(0.3048));
static_assert(1_q_ft2 == si::area<si::square_metre>(0.09290304));
static_assert(1_q_ft_per_s2 == si::acceleration<si::metre_per_second_sq>(0.3048));
static_assert(1_q_pdl > si::force<si::newton>(0.138254) && 1_q_pdl < si::force<si::newton>(0.138256));
static_assert(1_q_ft_pdl > si::energy<si::joule>(0.042140110093804) &&
              1_q_ft_pdl < si::energy<si::joule>(0.042140110093806));
static_assert(1_q_ft_pdl_per_s > si::power<si::watt>(0.042140110093804) &&
              1_q_ft_pdl_per_s < si::power<si::watt>(0.042140110093806));
static_assert(1_q_pdl_per_ft2 > si::pressure<si::pascal>(1.4881639435) &&
              1_q_pdl_per_ft2 < si::pressure<si::pascal>(1.4881639437));
}  // namespace fps_literals

namespace fps_plus_si_literals {

using namespace units::isq::si::literals;
using namespace units::isq::si::fps::literals;
using namespace units::isq::si::fps::references;

// static_assert(100_q_cm == 1_q_m);   // ambiguous
// static_assert(1'000_q_g == 1_q_kg); // ambiguous
static_assert(1 * ft == 0.3048_q_m);
static_assert(1_q_lb == 0.45359237_q_kg);
static_assert(1_q_s == 1_q_s);
static_assert(1_q_ft_per_s == 0.3048_q_m_per_s);
static_assert(1_q_ft2 == 0.09290304_q_m2);
static_assert(1_q_ft_per_s2 == 0.3048_q_m_per_s2);
static_assert(1_q_pdl > 0.138254_q_N && 1_q_pdl < 0.138256_q_N);
static_assert(1_q_ft_pdl > 0.042140110093804_q_J && 1_q_ft_pdl < 0.042140110093806_q_J);
static_assert(1_q_ft_pdl_per_s > 0.042140110093804_q_W && 1_q_ft_pdl_per_s < 0.042140110093806_q_W);
static_assert(1_q_pdl_per_ft2 > 1.4881639435_q_Pa && 1_q_pdl_per_ft2 < 1.4881639437_q_Pa);

}  // namespace fps_plus_si_literals

namespace fps_test {
namespace {
constexpr bool is_near(auto a, auto b, auto tol)
{
  const auto diff = a - b;
  return (diff <= tol) && (-diff <= tol);
}
}  // namespace

using namespace units::isq::si::fps::literals;
using namespace units::isq::si::fps::references;

// addition

static_assert(si::length<si::metre>(1) + 1 * ft == si::length<si::metre>(1.3048));
static_assert(1 * ft + si::length<si::metre>(1) == si::length<si::metre>(1.3048));
static_assert(is_near(quantity_cast<si::length<si::metre>>(1. * ft / 0.3048) + si::length<si::metre>(1),
                      si::length<si::metre>(2),
                      si::length<si::femtometre>(1)));  // 1 m in ft + 1 m
static_assert(is_near(si::length<si::metre>(1) + quantity_cast<si::length<si::metre>>(1. * ft / 0.3048),
                      si::length<si::metre>(2),
                      si::length<si::femtometre>(1)));  // 1 m + 1 m in ft
static_assert(1 * ft + quantity_cast<si::fps::length<si::fps::foot>>(si::length<si::metre>(0.3048)) ==
              2 * ft);  // 1 ft + 1 ft in m
static_assert(quantity_cast<si::fps::length<si::fps::foot>>(si::length<si::metre>(0.3048)) + 1 * ft ==
              2 * ft);  // 1 ft in m + 1 ft

// substraction

static_assert(1 * ft - si::length<si::metre>(1) == -si::length<si::metre>(0.6952));
static_assert(si::length<si::metre>(1) - 1 * ft == si::length<si::metre>(0.6952));
static_assert(quantity_cast<si::length<si::metre>>(6. * ft) - si::length<si::metre>(1) >
                si::length<si::metre>(0.8287) &&
              quantity_cast<si::length<si::metre>>(6. * ft) - si::length<si::metre>(1) <
                si::length<si::metre>(0.8289));  // 6 ft in m - 1 m  = ... m
static_assert(si::length<si::metre>(5) - quantity_cast<si::length<si::metre>>(6 * ft) ==
              si::length<si::metre>(3.1712));  // 5 m - 6 ft in m  = ...
static_assert(6. * ft - quantity_cast<si::fps::length<si::fps::foot>>(si::length<si::metre>(0.3048)) ==
              5. * ft);  // 6 ft - 1 ft in m = 5 ft
static_assert(quantity_cast<si::fps::length<si::fps::foot>>(si::length<si::metre>(1.8288)) - 1. * ft ==
              5. * ft);  // 6 ft in m - 1 ft = 5 ft

// multiplication

// TODO Add support for comparing of an unknown_dimension
// static_assert(2 * ft * si::length<si::metre>(2) == si::area<si::square_metre>(1.2192));
static_assert(quantity_cast<si::length<si::metre>>(2. * ft) * si::length<si::metre>(2) ==
              si::area<si::square_metre>(1.2192));
static_assert(quantity_cast<si::length<si::metre>>(2. * ft) * si::length<si::metre>(0.6096) ==
              si::area<si::square_metre>(0.371612160));  // 2 ft * 2 ft == 4 sq ft
static_assert(2. * ft * quantity_cast<si::fps::length<si::fps::foot>>(si::length<si::metre>(0.6096)) == 4._q_ft2);

// division

// TODO Add support for comparing of an unknown_dimension
// static_assert(si::area<si::square_metre>(4) / 200_q_cm == si::length<si::metre>(2));
// static_assert(400._q_cm / si::length<si::metre>(2) == 2);

static_assert(si::area<si::square_metre>(1.48644864) / quantity_cast<si::length<si::metre>>(4 * ft) ==
              si::length<si::metre>(1.2192));  // 16 ft2 / 4 ft = 4 ft
static_assert(quantity_cast<si::fps::area<si::fps::square_foot>>(si::area<si::square_metre>(1.48644864)) / (4. * ft) ==
              4. * ft);  // 16 ft2 / 4 ft = 4 ft

}  // namespace fps_test

}  // namespace
