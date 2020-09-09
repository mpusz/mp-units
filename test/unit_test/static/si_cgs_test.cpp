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


#include <units/physical/cgs/acceleration.h>
#include <units/physical/cgs/area.h>
#include <units/physical/cgs/energy.h>
#include <units/physical/cgs/force.h>
#include <units/physical/cgs/length.h>
#include <units/physical/cgs/mass.h>
#include <units/physical/cgs/power.h>
#include <units/physical/cgs/pressure.h>
#include <units/physical/cgs/time.h>
#include <units/physical/cgs/speed.h>
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

static_assert(cgs::length<cgs::centimetre>(100) == si::length<si::metre>(1));
static_assert(cgs::mass<cgs::gram>(1'000) == si::mass<si::kilogram>(1));
static_assert(cgs::time<cgs::second>(1) == si::time<si::second>(1));
static_assert(cgs::speed<cgs::centimetre_per_second>(100) == si::speed<si::metre_per_second>(1));
static_assert(cgs::area<cgs::square_centimetre>(10000) == si::area<si::square_metre>(1));
static_assert(cgs::acceleration<cgs::gal>(100) == si::acceleration<si::metre_per_second_sq>(1));
static_assert(cgs::force<cgs::dyne>(100'000) == si::force<si::newton>(1));
static_assert(cgs::energy<cgs::erg>(10'000'000) == si::energy<si::joule>(1));
static_assert(cgs::power<cgs::erg_per_second>(10'000'000) == si::power<si::watt>(1));
static_assert(cgs::pressure<cgs::barye>(10) == si::pressure<si::pascal>(1));

namespace si_test {

using namespace units::physical::si::literals;

static_assert(cgs::length<cgs::centimetre>(100) == 1_q_m);
static_assert(cgs::mass<cgs::gram>(1'000) == 1_q_kg);
static_assert(cgs::time<cgs::second>(1) == 1_q_s);
static_assert(cgs::speed<cgs::centimetre_per_second>(100) == 1_q_m_per_s);
static_assert(cgs::acceleration<cgs::gal>(100) == 1_q_m_per_s2);
static_assert(cgs::force<cgs::dyne>(100'000) == 1_q_N);
static_assert(cgs::energy<cgs::erg>(10'000'000) == 1_q_J);
static_assert(cgs::power<cgs::erg_per_second>(10'000'000) == 1_q_W);
static_assert(cgs::pressure<cgs::barye>(10) == 1_q_Pa);

}

namespace cgs_test {

using namespace units::physical::cgs::literals;

static_assert(100_q_cm == si::length<si::metre>(1));
static_assert(1'000_q_g == si::mass<si::kilogram>(1));
static_assert(1_q_s == si::time<si::second>(1));
static_assert(100_q_cm_per_s == si::speed<si::metre_per_second>(1));
static_assert(100_q_Gal == si::acceleration<si::metre_per_second_sq>(1));
static_assert(100'000_q_dyn == si::force<si::newton>(1));
static_assert(10'000'000_q_erg == si::energy<si::joule>(1));
static_assert(10'000'000_q_erg_per_s == si::power<si::watt>(1));
static_assert(10_q_Ba == si::pressure<si::pascal>(1));

}

namespace both_test {

using namespace units::physical::si::literals;
using namespace units::physical::cgs::literals;

// static_assert(100_q_cm == 1_q_m);   // ambiguous
// static_assert(1'000_q_g == 1_q_kg); // ambiguous
static_assert(1_q_s == 1_q_s);
static_assert(100_q_cm_per_s == 1_q_m_per_s);
static_assert(100_q_Gal == 1_q_m_per_s2);
static_assert(100'000_q_dyn == 1_q_N);
static_assert(10'000'000_q_erg == 1_q_J);
static_assert(10'000'000_q_erg_per_s == 1_q_W);
static_assert(10_q_Ba == quantity_cast<double>(1_q_Pa));

}

namespace cgs_test {

// addition

// static_assert(100_q_cm + si::length<si::metre>(1) == si::length<si::metre>(2)); // should not compile (different dimensions)
// static_assert(si::length<si::metre>(1) + 100_q_cm == si::length<si::metre>(2)); // should not compile (different dimensions)
static_assert(quantity_cast<si::length<si::metre>>(100_q_cm) + si::length<si::metre>(1) == si::length<si::metre>(2));
static_assert(si::length<si::metre>(1) + quantity_cast<si::length<si::metre>>(100_q_cm) == si::length<si::metre>(2));
static_assert(100_q_cm + quantity_cast<cgs::length<cgs::centimetre>>(si::length<si::metre>(1)) == 200_q_cm);
static_assert(quantity_cast<cgs::length<cgs::centimetre>>(si::length<si::metre>(1)) + 100_q_cm == 200_q_cm);

// substraction

// static_assert(500_q_cm - si::length<si::metre>(1) == si::length<si::metre>(4)); // should not compile (different dimensions)
// static_assert(si::length<si::metre>(5) - 100_q_cm == si::length<si::metre>(4)); // should not compile (different dimensions)
static_assert(quantity_cast<si::length<si::metre>>(500_q_cm) - si::length<si::metre>(1) == si::length<si::metre>(4));
static_assert(si::length<si::metre>(5) - quantity_cast<si::length<si::metre>>(100_q_cm) == si::length<si::metre>(4));
static_assert(500_q_cm - quantity_cast<cgs::length<cgs::centimetre>>(si::length<si::metre>(1)) == 400_q_cm);
static_assert(quantity_cast<cgs::length<cgs::centimetre>>(si::length<si::metre>(5)) - 100_q_cm == 400_q_cm);

// multiplication

// static_assert(200_q_cm * si::length<si::metre>(2) == si::area<si::square_metre>(4)); // should not compile (unknown dimension)

static_assert(quantity_cast<si::dim_length>(200._q_cm) * si::length<si::metre>(2) == si::area<si::square_metre>(4));
static_assert(200._q_cm * quantity_cast<cgs::dim_length>(si::length<si::metre>(2)) == 40'000_q_cm2);

// TODO Add support for quantity_cast on an unknown_dimension?
// static_assert(quantity_cast<si::area<si::square_metre>>(200_q_cm * si::length<si::metre>(2)) == si::area<si::square_metre>(4));
// static_assert(quantity_cast<si::dim_area>(200_q_cm * si::length<si::metre>(2)) == si::area<si::square_metre>(4));
// static_assert(quantity_cast<cgs::area<cgs::square_centimeters>>(200_q_cm * si::length<si::metre>(2)) == 40'000_q_sq_cm);
// static_assert(quantity_cast<cgs::dim_area>(200_q_cm * si::length<si::metre>(2)) == 40'000_q_sq_cm);

// division

// static_assert(si::area<si::square_metre>(4) / 200_q_cm == si::length<si::metre>(2)); // should not compile (unknown dimension)

static_assert(si::area<si::square_metre>(4) / quantity_cast<si::length<si::metre>>(200_q_cm) == si::length<si::metre>(2));
static_assert(quantity_cast<cgs::area<cgs::square_centimetre>>(si::area<si::square_metre>(4)) / 200._q_cm == 200_q_cm);

}

}
