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

#include "units/si/length.h"
#include "units/si/time.h"
#include "units/si/frequency.h"
#include "units/si/velocity.h"
#include <gtest/gtest.h>
#include <utility>

using namespace units;
using namespace units::literals;


// frequency

static_assert(2 / 1_s == 2_Hz);
static_assert(1000 / 1_s == 1_kHz);
static_assert(3.2_GHz == 3'200'000'000_Hz);


// time

static_assert(1_h == 3600_s);


// length

static_assert(1_km == 1000_m);
static_assert(10_km / 5_km == 2);
static_assert(10_km / 2 == 5_km);


// velocity

static_assert(std::is_same_v<decltype(1_km / 1_s), velocity<long long int, std::ratio<1000, 1>>>);

static_assert(10_m / 5_s == 2_mps);
static_assert(1_km / 1_s == 1000_mps);
//static_assert(1_km / 1_h == 1_kmph);  // should not compile
static_assert(1.0_km / 1_h == 1_kmph);
static_assert(1000.0_m / 3600.0_s == 1_kmph);

static_assert(2_kmph * 2_h == 4_km);
//static_assert(2_kmph * 15_min == 500_m); // should not compile
static_assert(2_kmph * 15.0_min == 500_m);
static_assert(2.0_kmph * 15_min == 500_m);

static_assert(2_km / 2_kmph == 1_h);
// static_assert(2000_m / 2_kmph == 1_h); // should not compile
static_assert(quantity_cast<kilometers<int>>(2000_m) / 2_kmph == 1_h);
