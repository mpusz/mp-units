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

#include <units/physical/si/fps/fps.h>
#include <units/physical/si/derived/acceleration.h>
#include <units/physical/si/derived/area.h>
#include <units/physical/si/derived/energy.h>
#include <units/physical/si/derived/force.h>
#include <units/physical/si/base/length.h>
#include <units/physical/si/base/mass.h>
#include <units/physical/si/derived/power.h>
#include <units/physical/si/derived/pressure.h>
#include <units/physical/si/derived/speed.h>
#include <units/physical/si/base/time.h>

namespace {

using namespace units;
using namespace units::physical::si::fps;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(foot::symbol == "ft");

// mass

// time

/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// speed

static_assert(10_q_ft / 5_q_s == 2_q_ft_per_s);
static_assert(10_q_ft / 2_q_ft_per_s == 5_q_s);
static_assert(10_q_ft == 2_q_ft_per_s * 5_q_s);

static_assert(detail::unit_text<dim_speed, foot_per_second>() == "ft/s");

// area
static_assert(foot::ratio / dimension_unit<dim_length>::ratio == ratio(1));

static_assert(1_q_ft * 1_q_ft == 1_q_ft2);
static_assert(100_q_ft2 / 10_q_ft == 10_q_ft);

static_assert(detail::unit_text<dim_area, square_foot>() == basic_symbol_text("ft²", "ft^2"));

/* ************** DERIVED DIMENSIONS WITH NAMED UNITS **************** */

// acceleration

static_assert(10_q_ft_per_s / 10_q_s == 1_q_ft_per_s2);
static_assert(10_q_ft_per_s / 1_q_ft_per_s2 == 10_q_s);
static_assert(1_q_ft_per_s2 * 10_q_s == 10_q_ft_per_s);

// force

static_assert(10_q_lb * 10_q_ft_per_s2 == 100_q_pdl);
static_assert(100_q_pdl / 10_q_lb == 10_q_ft_per_s2);
static_assert(100_q_pdl / 10_q_ft_per_s2 == 10_q_lb);

// pressure
static_assert(10_q_pdl / 10_q_ft2 == 1_q_pdl_per_ft2);
static_assert(10_q_pdl / 1_q_pdl_per_ft2 == 10_q_ft2);
static_assert(1_q_pdl_per_ft2 * 10_q_ft2 == 10_q_pdl);

// energy

static_assert(10_q_pdl * 10_q_ft == 100_q_ft_pdl);
static_assert(100_q_ft_pdl / 10_q_ft == 10_q_pdl);
static_assert(100_q_ft_pdl / 10_q_pdl == 10_q_ft);

/* ************** DERIVED DIMENSIONS IN TERMS OF OTHER UNITS **************** */

// power

static_assert(10_q_ft_pdl / 10_q_s == 1_q_ft_pdl_per_s);
static_assert(1_q_ft_pdl_per_s * 10_q_s == 10_q_ft_pdl);
static_assert(10_q_ft_pdl / 1_q_ft_pdl_per_s == 10_q_s);

// static_assert(detail::unit_text<dim_power, foot_pound_force_per_second>() == "ft_lbf/s");

}
