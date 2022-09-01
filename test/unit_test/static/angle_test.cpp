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

#include <units/generic/angle.h>
#include <numbers>

namespace {

using namespace units::references;

static_assert(360 * deg == 1 * rot);
static_assert(400 * grad == 1 * rot);
static_assert(std::numbers::pi * 2 * rad == 1. * rot);

using namespace units::literals;

static_assert(360_q_deg == 1_q_rot);
static_assert(400_q_grad == 1_q_rot);
static_assert(std::numbers::pi * quantity_cast<double>(2._q_rad) == quantity_cast<double>(1._q_rot));

static_assert(units::aliases::deg<>(360.) == units::aliases::rot<>(1.));
static_assert(units::aliases::deg<int>(360) == units::aliases::rot<int>(1));
static_assert(units::aliases::grad<int>(400) == units::aliases::rot<int>(1));
static_assert(std::numbers::pi * units::aliases::rad<>(2.) == units::aliases::rot<>(1.));

}  // namespace
