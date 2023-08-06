// The MIT License (MIT)
//
// Copyright (c) 2023 Meskauskas Audrius
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

#include "almost_equals.h"
#include <catch2/catch_all.hpp>
#include <mp-units/systems/enzimology/enzimology.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/unit_symbols.h>
#include <mp-units/systems/si/units.h>
#include <limits>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::si;

// Check agreements between international unit (iu), unit and katal
static_assert(1. * enzimology::unit == 1. * enzimology::iu);
static_assert(1. * enzimology::unit < 16.6667 * enzimology::nanokatal);
static_assert(1. * enzimology::unit > 16.6666 * enzimology::nanokatal);

TEST_CASE("Enzimology calcuclations", "[enzimology]")
{
  // In the test tube with 2 ml of test solution, 10 micro-molles of the substrate have been processed in 5 minutes.
  SECTION("calculations")
  {
    const auto ml = milli<litre>;
    const auto mM = milli<mole>;
    const auto mkM = micro<mole>;

    const auto time = 5. * min;
    const auto processed_substrate = 10. * mkM;
    const auto volume = 2. * ml;

    // Same values as above, expressed in different units.
    const auto time_same = 300. * second;
    const auto processed_substrate_same = 0.01 * mM;

    quantity<enzimology::unit> units = processed_substrate / time;
    CHECK(MP_UNITS_STD_FMT::format("{}", units) == "2 U");

    quantity<enzimology::unit> units_same = processed_substrate_same / time_same;
    CHECK(MP_UNITS_STD_FMT::format("{}", units_same) == "2 U");

    CHECK(units == units_same);

    quantity<enzimology::activity> activity = units / volume;
    CHECK(MP_UNITS_STD_FMT::format("{}", activity) == "1 U/ml");

    auto protein_concentration = (20.0 * mg) / (1.0 * ml);

    quantity<enzimology::specific_activity> specific_activity = activity / protein_concentration;
    CHECK(MP_UNITS_STD_FMT::format("{}", specific_activity) == "0.05 U/mg");
  }

  SECTION("U vs IU")
  {
    // Some sources name it Unit, some International Unit.
    // Allow both, this likely to be highly opinion based.
    using namespace enzimology::unit_symbols;
    const auto one_unit = 1.0 * U;
    const auto one_international_unit = 1.0 * IU;

    CHECK(MP_UNITS_STD_FMT::format("{}", one_unit) == "1 U");
    CHECK(MP_UNITS_STD_FMT::format("{}", one_international_unit) == "1 IU");

    CHECK(one_unit == one_international_unit);
  }

  SECTION("katal")
  {
    using namespace enzimology::unit_symbols;
    const auto one_unit = 1.0 * U;
    const quantity<enzimology::nanokatal> same_in_nano_katals = one_unit;

    CHECK(MP_UNITS_STD_FMT::format("{}", one_unit) == "1 U");
    CHECK(MP_UNITS_STD_FMT::format("{}", same_in_nano_katals) == "16.6667 nkat");

    CHECK(one_unit == same_in_nano_katals);
  }
}
