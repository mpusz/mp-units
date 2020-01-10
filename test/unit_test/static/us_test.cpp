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
#include <units/physical/us/area.h>
#include <units/physical/us/length.h>
#include <units/physical/us/velocity.h>
#include <units/physical/us/volume.h>
#include <utility>

namespace {

using namespace units;
using namespace units::si;
using namespace units::us;
using namespace units::international;

/* ************** BASE DIMENSIONS **************** */

// length

static_assert(1yd == 0.9144m);
static_assert(1yd == 3ft);
static_assert(1ft == 12in);
static_assert(1mi == 1760yd);

static_assert(5in + 8cm == 207mm);


/* ************** DERIVED DIMENSIONS IN TERMS OF BASE UNITS **************** */

// velocity

static_assert(10.0mi / 2h == 5mph);

static_assert(mile_per_hour::symbol == "mi/h");

}  // namespace
