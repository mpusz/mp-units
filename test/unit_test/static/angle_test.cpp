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

#include <mp_units/generic/angle.h>
#include <mp_units/generic/solid_angle.h>
#include <numbers>

namespace {

// plane angle

using namespace mp_units::references;
using namespace mp_units::literals;

static_assert(360 * deg == 1 * rev);
static_assert(400 * grad == 1 * rev);
static_assert(std::numbers::pi * 2 * rad == 1. * rev);

static_assert(360_q_deg == 1_q_rev);
static_assert(400_q_grad == 1_q_rev);
static_assert(std::numbers::pi * value_cast<double>(2._q_rad) == value_cast<double>(1._q_rev));

static_assert(mp_units::aliases::deg<>(360.) == mp_units::aliases::rev<>(1.));
static_assert(mp_units::aliases::deg<int>(360) == mp_units::aliases::rev<int>(1));
static_assert(mp_units::aliases::grad<int>(400) == mp_units::aliases::rev<int>(1));
static_assert(std::numbers::pi * mp_units::aliases::rad<>(2.) == mp_units::aliases::rev<>(1.));

// solid angle

static_assert(1_q_rad * 1_q_rad == 1_q_sr);
static_assert(1_q_deg * 1_q_deg == 1_q_deg2);

}  // namespace
