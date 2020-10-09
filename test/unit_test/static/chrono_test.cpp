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

#include "test_tools.h"
#include <units/chrono.h>
#include <units/physical/si/derived/speed.h>

namespace {

using namespace units;
using namespace units::physical;
using namespace units::physical::si::literals;
using namespace std::chrono_literals;


// construction
static_assert(std::constructible_from<si::time<si::second, std::int64_t>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::second, std::int64_t>>);
static_assert(std::constructible_from<si::time<si::hour, std::int64_t>, std::chrono::hours>);
static_assert(!std::convertible_to<std::chrono::hours, si::time<si::hour, std::int64_t>>);
static_assert(!std::constructible_from<si::time<si::second, std::int64_t>, std::chrono::hours>);
static_assert(!std::convertible_to<std::chrono::hours, si::time<si::second, std::int64_t>>);
static_assert(!std::constructible_from<si::time<si::hour, std::int64_t>, std::chrono::seconds>);
static_assert(!std::convertible_to<std::chrono::seconds, si::time<si::hour, std::int64_t>>);

// CTAD
static_assert(std::is_same_v<decltype(quantity{1s}), si::time<si::second, std::chrono::seconds::rep>>);
static_assert(std::is_same_v<decltype(quantity{1h}), si::time<si::hour, std::chrono::hours::rep>>);

// operators
static_assert(quantity{1s} + 1_q_s == 2_q_s);
static_assert(quantity{1s} + 1_q_min == 61_q_s);
static_assert(10_q_m / quantity{2s} == 5_q_m_per_s);

}  // namespace
