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

#include <units/isq/iec80000/iec80000.h>
#include <units/isq/si/time.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

using namespace units::isq::si::references;
using namespace units::isq::iec80000::references;

// information

static_assert(1 * B == 8 * bit);
static_assert(1024 * bit == 1 * Kibit);
static_assert(1024 * B == 1 * KiB);
static_assert(8 * 1024 * bit == 1 * KiB);
static_assert(8 * 1 * Kibit == 1 * KiB);

static_assert(1 * kbit == 1000 * bit);
static_assert(2000 * Mibit == 2097152 * kbit);

static_assert(1 * Kibit == 1024 * bit);
static_assert(1 * Mibit == 1024 * Kibit);
static_assert(1 * Gibit == 1024 * Mibit);
static_assert(1 * Tibit == 1024 * Gibit);
static_assert(1 * Pibit == 1024 * Tibit);
static_assert(1 * Eibit == 1024 * Pibit);

// transfer rate

static_assert(16 * B / (2 * s) == 8 * (B / s));
static_assert(120 * kB / (2 * min) == 1000 * (B / s));

// modulation rate

static_assert(12 / (2 * s) == 6 * Bd);
static_assert(6000 / (3 * s) == 2 * kBd);

}  // namespace
