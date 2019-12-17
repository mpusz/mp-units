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
#include <units/physical/cgs/energy.h>
#include <units/physical/cgs/force.h>
#include <units/physical/cgs/length.h>
#include <units/physical/cgs/mass.h>
#include <units/physical/cgs/power.h>
#include <units/physical/cgs/pressure.h>
#include <units/physical/cgs/time.h>
#include <units/physical/cgs/velocity.h>

namespace {

using namespace units;
using namespace units::cgs;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(centimetre::symbol == "cm");

// mass

// time

/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// velocity

static_assert(10cm / 5s == 2cmps);
static_assert(10cm / 2cmps == 5s);
static_assert(10cm == 2cmps * 5s);

static_assert(detail::unit_text<dim_velocity, centimetre_per_second>() == "cm/s");

// area
static_assert(std::is_same_v<ratio_divide<centimetre::ratio, dimension_unit<dim_length>::ratio>, ratio<1>>);

static_assert(1cm * 1cm == 1sq_cm);
static_assert(100sq_cm / 10cm == 10cm);

static_assert(detail::unit_text<dim_area, square_centimetre>() == "cm²");

/* ************** DERIVED DIMENSIONS WITH NAMED UNITS **************** */

// acceleration

static_assert(10cmps / 10s == 1Gal);
static_assert(10cmps / 1Gal == 10s);
static_assert(1Gal * 10s == 10cmps);

// force

static_assert(10g * 10Gal == 100dyn);
static_assert(100dyn / 10g == 10Gal);
static_assert(100dyn / 10Gal == 10g);

// pressure

static_assert(10dyn / 10sq_cm == 1Ba);
static_assert(10dyn / 1Ba == 10sq_cm);
static_assert(1Ba * 10sq_cm == 10dyn);

// energy

static_assert(10dyn * 10cm == 100_erg);
static_assert(100_erg / 10cm == 10dyn);
static_assert(100_erg / 10dyn == 10cm);

/* ************** DERIVED DIMENSIONS IN TERMS OF OTHER UNITS **************** */

// power

static_assert(10_erg / 10s == 1_ergps);
static_assert(1_ergps * 10s == 10_erg);
static_assert(10_erg / 1_ergps == 10s);

static_assert(detail::unit_text<dim_power, erg_per_second>() == "erg/s");

}
