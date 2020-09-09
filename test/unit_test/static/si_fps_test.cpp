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


#include <units/physical/fps/acceleration.h>
#include <units/physical/fps/area.h>
#include <units/physical/fps/energy.h>
#include <units/physical/fps/force.h>
#include <units/physical/fps/length.h>
#include <units/physical/fps/mass.h>
#include <units/physical/fps/power.h>
#include <units/physical/fps/pressure.h>
#include <units/physical/fps/time.h>
#include <units/physical/fps/speed.h>
#include <units/physical/si/acceleration.h>
#include <units/physical/si/area.h>
#include <units/physical/si/energy.h>
#include <units/physical/si/force.h>
#include <units/physical/si/length.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/power.h>
#include <units/physical/si/pressure.h>
#include <units/physical/si/time.h>
#include <units/physical/si/speed.h>

namespace {

using namespace units::physical;

static_assert(fps::length<fps::foot>(1) == si::length<si::metre>(0.3048));
static_assert(fps::mass<fps::pound>(1) == si::mass<si::kilogram>(0.45359237));
static_assert(fps::time<fps::second>(1) == si::time<si::second>(1));
static_assert(fps::speed<fps::foot_per_second>(1) == si::speed<si::metre_per_second>(0.3048));
static_assert(fps::area<fps::square_foot>(1) == si::area<si::square_metre>(0.09290304));
static_assert(fps::acceleration<fps::foot_per_second_sq>(1) == si::acceleration<si::metre_per_second_sq>(0.3048));
static_assert(fps::force<fps::poundal>(1) > si::force<si::newton>(0.138254) && 
              fps::force<fps::poundal>(1) < si::force<si::newton>(0.138256));
static_assert(fps::energy<fps::foot_poundal>(1) > si::energy<si::joule>(0.042140110093804) && 
              fps::energy<fps::foot_poundal>(1) < si::energy<si::joule>(0.042140110093806));
static_assert(fps::power<fps::foot_poundal_per_second>(1) > si::power<si::watt>(0.042140110093804) && 
              fps::power<fps::foot_poundal_per_second>(1) < si::power<si::watt>(0.042140110093806));
static_assert(fps::pressure<fps::poundal_per_foot_sq>(1) > si::pressure<si::pascal>(1.4881639435) &&
              fps::pressure<fps::poundal_per_foot_sq>(1) < si::pressure<si::pascal>(1.4881639437));

namespace si_literals {

using namespace units::physical::si::literals;

static_assert(fps::length<fps::foot>(1) == 0.3048_q_m);
static_assert(fps::mass<fps::pound>(1) == 0.45359237_q_kg);
static_assert(fps::time<fps::second>(1) == 1_q_s);
static_assert(fps::speed<fps::foot_per_second>(1) == 0.3048_q_m_per_s);
static_assert(fps::area<fps::square_foot>(1) == 0.09290304_q_m2);
static_assert(fps::acceleration<fps::foot_per_second_sq>(1) == 0.3048_q_m_per_s2);
static_assert(fps::force<fps::poundal>(1) > 0.138254_q_N && 
              fps::force<fps::poundal>(1) < 0.138256_q_N);
static_assert(fps::energy<fps::foot_poundal>(1) > 0.042140110093804_q_J && 
              fps::energy<fps::foot_poundal>(1) < 0.042140110093806_q_J);
static_assert(fps::power<fps::foot_poundal_per_second>(1) > 0.042140110093804_q_W && 
              fps::power<fps::foot_poundal_per_second>(1) < 0.042140110093806_q_W);
static_assert(fps::pressure<fps::poundal_per_foot_sq>(1) >  1.4881639435_q_Pa &&
              fps::pressure<fps::poundal_per_foot_sq>(1) <  1.4881639437_q_Pa);
}

namespace fps_literals {

using namespace units::physical::fps::literals;

static_assert(1_q_ft == si::length<si::metre>(0.3048));
static_assert(1_q_lb == si::mass<si::kilogram>(0.45359237));
static_assert(1_q_s == si::time<si::second>(1));
static_assert(1_q_ft_per_s == si::speed<si::metre_per_second>(0.3048));
static_assert(1_q_ft2 == si::area<si::square_metre>(0.09290304));
static_assert(1_q_ft_per_s2 == si::acceleration<si::metre_per_second_sq>(0.3048));
static_assert(1_q_pdl > si::force<si::newton>(0.138254) && 
              1_q_pdl < si::force<si::newton>(0.138256));
static_assert(1_q_ft_pdl > si::energy<si::joule>(0.042140110093804) && 
              1_q_ft_pdl < si::energy<si::joule>(0.042140110093806));
static_assert(1_q_ft_pdl_per_s > si::power<si::watt>(0.042140110093804) && 
              1_q_ft_pdl_per_s < si::power<si::watt>(0.042140110093806));
static_assert(1_q_pdl_per_ft2> si::pressure<si::pascal>(1.4881639435) &&
              1_q_pdl_per_ft2 < si::pressure<si::pascal>(1.4881639437));
}

namespace fps_plus_si_literals {

using namespace units::physical::si::literals;
using namespace units::physical::fps::literals;

// static_assert(100_q_cm == 1_q_m);   // ambiguous
// static_assert(1'000_q_g == 1_q_kg); // ambiguous
static_assert(1_q_ft == 0.3048_q_m);
static_assert(1_q_lb == 0.45359237_q_kg);
static_assert(1_q_s  == 1_q_s);
static_assert(1_q_ft_per_s == 0.3048_q_m_per_s);
static_assert(1_q_ft2 == 0.09290304_q_m2);
static_assert(1_q_ft_per_s2 == 0.3048_q_m_per_s2);
static_assert(1_q_pdl > 0.138254_q_N && 
              1_q_pdl < 0.138256_q_N);
static_assert(1_q_ft_pdl > 0.042140110093804_q_J && 
              1_q_ft_pdl < 0.042140110093806_q_J);
static_assert(1_q_ft_pdl_per_s > 0.042140110093804_q_W && 
              1_q_ft_pdl_per_s < 0.042140110093806_q_W);
static_assert(1_q_pdl_per_ft2> 1.4881639435_q_Pa &&
              1_q_pdl_per_ft2 <1.4881639437_q_Pa);

}

namespace cgs_test {

using namespace units::physical::fps::literals;

// addition

// static_assert(si::length<si::metre>(1) + 1_q_ft == si::length<si::metre>(1.3048)); // should not compile (different dimensions)
// static_assert(1_q_ft / 0.3048 + si::length<si::metre>(1) == si::length<si::metre>(1.3048)); // should not compile (different dimensions)
static_assert(quantity_cast<si::length<si::metre>>(1._q_ft / 0.3048) + si::length<si::metre>(1) == si::length<si::metre>(2));   // 1 m in ft + 1 m
static_assert(si::length<si::metre>(1) + quantity_cast<si::length<si::metre>>(1._q_ft / 0.3048) == si::length<si::metre>(2));   // 1 m + 1 m in ft
static_assert(1_q_ft + quantity_cast<fps::length<fps::foot>>(si::length<si::metre>(0.3048)) == 2_q_ft);                         // 1 ft + 1 ft in m
static_assert(quantity_cast<fps::length<fps::foot>>(si::length<si::metre>(0.3048)) + 1_q_ft == 2_q_ft);                         // 1 ft in m + 1 ft

// substraction

// static_assert(1_q_ft - si::length<si::metre>(1) == -si::length<si::metre>(0.6952)); // should not compile (different dimensions)
// static_assert(si::length<si::metre>(1) - 1_q_ft == si::length<si::metre>(0.6952)); // should not compile (different dimensions)
static_assert(quantity_cast<si::length<si::metre>>(6._q_ft) - si::length<si::metre>(1) > si::length<si::metre>(0.8287) &&
              quantity_cast<si::length<si::metre>>(6._q_ft) - si::length<si::metre>(1) < si::length<si::metre>(0.8289));      // 6 ft in m - 1 m  = ... m
static_assert(si::length<si::metre>(5) - quantity_cast<si::length<si::metre>>(6_q_ft) == si::length<si::metre>(3.1712));       // 5 m - 6 ft in m  = ...
static_assert(6._q_ft - quantity_cast<fps::length<fps::foot>>(si::length<si::metre>(0.3048)) == 5._q_ft);                       // 6 ft - 1 ft in m = 5 ft
static_assert(quantity_cast<fps::length<fps::foot>>(si::length<si::metre>(1.8288)) - 1._q_ft == 5._q_ft);                       // 6 ft in m - 1 ft = 5 ft

// multiplication

// static_assert(2_q_ft * si::length<si::metre>(2) == si::area<si::square_metre>(1.2192)); // should not compile (unknown dimension)
// static_assert(quantity_cast<si::length<si::metre>>(2._q_ft) * si::length<si::metre>(2) == si::area<si::square_metre>(1.2192)); 
static_assert(quantity_cast<si::length<si::metre>>(2._q_ft) * si::length<si::metre>(0.6096) == si::area<si::square_metre>(0.371612160));  // 2 ft * 2 ft == 4 sq ft
static_assert(2._q_ft * quantity_cast<fps::length<fps::foot>>(si::length<si::metre>(0.6096)) == 4._q_ft2);

// division

// static_assert(si::area<si::square_metre>(4) / 200_q_cm == si::length<si::metre>(2)); // should not compile (unknown dimension)
static_assert(si::area<si::square_metre>(1.48644864) / quantity_cast<si::length<si::metre>>(4_q_ft) == si::length<si::metre>(1.2192)); // 16 ft2 / 4 ft = 4 ft
static_assert(quantity_cast<fps::area<fps::square_foot>>(si::area<si::square_metre>(1.48644864)) / 4._q_ft == 4._q_ft);                 // 16 ft2 / 4 ft = 4 ft

}

}
