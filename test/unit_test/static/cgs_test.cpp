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

#include <units/physical/si/cgs/cgs.h>

namespace {

using namespace units;
using namespace units::physical::si::cgs;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(centimetre::symbol == "cm");

// mass

// time

/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// speed

static_assert((10_q_cm / 5_q_s).count() == 2);
static_assert((2_q_cm_per_s).count() == 2);
static_assert(10_q_cm / 5_q_s == 2_q_cm_per_s);
static_assert(10_q_cm / 2_q_cm_per_s == 5_q_s);
static_assert(10_q_cm == 2_q_cm_per_s * 5_q_s);

static_assert(detail::unit_text<dim_speed, centimetre_per_second>() == "cm/s");

// area
static_assert(centimetre::ratio / dimension_unit<dim_length>::ratio == ratio(1));

static_assert((1_q_cm * 1_q_cm).count() == 1);
static_assert((1_q_cm2).count() == 1);
static_assert(1_q_cm * 1_q_cm == 1_q_cm2);
static_assert(100_q_cm * 100_q_cm == area<physical::si::square_metre>(1));
static_assert(100_q_cm * 100_q_cm == length<physical::si::metre>(1) * length<physical::si::metre>(1));
static_assert(100_q_cm2 / 10_q_cm == 10_q_cm);

static_assert(detail::unit_text<dim_area, square_centimetre>() == basic_symbol_text("cm²", "cm^2"));

/* ************** DERIVED DIMENSIONS WITH NAMED UNITS **************** */

// acceleration

static_assert(10_q_cm_per_s / 10_q_s == 1_q_Gal);
static_assert(10_q_cm_per_s / 1_q_Gal == 10_q_s);
static_assert(1_q_Gal * 10_q_s == 10_q_cm_per_s);

// force

static_assert(10_q_g * 10_q_Gal == 100_q_dyn);
static_assert(100_q_dyn / 10_q_g == 10_q_Gal);
static_assert(100_q_dyn / 10_q_Gal == 10_q_g);

// pressure

static_assert(10_q_dyn / 10_q_cm2 == 1_q_Ba);
static_assert(10_q_dyn / 1_q_Ba == 10_q_cm2);
static_assert(1_q_Ba * 10_q_cm2 == 10_q_dyn);

// energy

static_assert(10_q_dyn * 10_q_cm == 100_q_erg);
static_assert(100_q_erg / 10_q_cm == 10_q_dyn);
static_assert(100_q_erg / 10_q_dyn == 10_q_cm);

/* ************** DERIVED DIMENSIONS IN TERMS OF OTHER UNITS **************** */

// power

static_assert(10_q_erg / 10_q_s == 1_q_erg_per_s);
static_assert(1_q_erg_per_s * 10_q_s == 10_q_erg);
static_assert(10_q_erg / 1_q_erg_per_s == 10_q_s);

static_assert(detail::unit_text<dim_power, erg_per_second>() == "erg/s");

}
