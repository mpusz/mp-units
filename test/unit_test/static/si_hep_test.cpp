// The MIT License (MIT)
//
// Copyright (c) 2021 Mateusz Pusz
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

#include <units/isq/si/area.h>
#include <units/isq/si/hep/area.h>
#include <units/isq/si/hep/energy.h>
#include <units/isq/si/hep/mass.h>
#include <units/isq/si/hep/momentum.h>
#include <units/isq/si/length.h>

namespace {

using namespace units::isq;

static_assert(units::detail::quantity_ratio<si::length<si::metre>> == units::ratio(1));

// mass
static_assert(si::hep::mass<si::hep::eV_per_c2>(1'000) == si::hep::mass<si::hep::keV_per_c2>(1));
static_assert(si::hep::mass<si::hep::eV_per_c2>(1'000) == si::mass<si::hep::keV_per_c2>(1));
static_assert(si::mass<si::hep::eV_per_c2>(1'000) == si::hep::mass<si::hep::keV_per_c2>(1));

// momentum
static_assert(si::hep::momentum<si::hep::eV_per_c>(1'000'000) == si::hep::momentum<si::hep::MeV_per_c>(1));
static_assert(si::hep::momentum<si::hep::eV_per_c>(1'000'000) == si::momentum<si::hep::MeV_per_c>(1));
static_assert(si::momentum<si::hep::eV_per_c>(1'000'000) == si::hep::momentum<si::hep::MeV_per_c>(1));

// area
static_assert(si::area<si::hep::barn>(1e28) == si::area<si::square_metre>(1));
// static_assert(si::area<si::hep::barn>(1) == si::area<si::square_metre>(1e-28)); // numeric rounding issues on some
// platforms


namespace hep_literal_test {

using namespace units::isq::si::hep::literals;

static_assert(si::hep::energy<si::hep::eV>(1'000) == 1_q_keV);
static_assert(si::hep::energy<si::hep::eV>(1'000'000) == 1_q_MeV);

static_assert(si::hep::mass<si::hep::eV_per_c2>(1'000) == 1_q_keV_per_c2);
static_assert(si::hep::mass<si::hep::eV_per_c2>(1'000'000) == 1_q_MeV_per_c2);

static_assert(si::hep::momentum<si::hep::eV_per_c>(1'000) == 1_q_keV_per_c);
static_assert(si::hep::momentum<si::hep::eV_per_c>(1'000'000) == 1_q_MeV_per_c);

// static_assert(si::area<si::square_metre, long double>(1e-28L) == 1_q_b); // numeric rounding issues on some platforms
// static_assert(si::hep::area<si::square_yoctometre, long double>(1e-4L) == 1_q_b); // numeric rounding issues on some
// platforms
static_assert(si::area<si::square_metre>(1e-43) == 1_q_fb);

}  // namespace hep_literal_test

}  // namespace
