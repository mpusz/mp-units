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

#include <units/physical/si/area.h>
#include <units/physical/si/length.h>
#include <units/physical/international/length.h>
#include <units/physical/si/velocity.h>
#include <units/physical/si/volume.h>
#include <units/physical/international/area.h>
#include <units/physical/us/length.h>
#include <units/physical/international/velocity.h>
#include <units/physical/international/volume.h>
#include <utility>

namespace {

using namespace units;
using namespace units::si;
using namespace units::us;
using namespace units::international;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(1q_yd == 0.9144q_m);
static_assert(1q_yd == 3q_ft);
static_assert(1q_ft == 12q_in);
static_assert(1q_mi == 1760q_yd);

static_assert(5q_in + 8q_cm == 207q_mm);


/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// velocity

static_assert(10.0q_mi / 2q_h == 5q_mph);

static_assert(mile_per_hour::symbol == "mi/h");

}  // namespace
