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

#include <mp-units/systems/iec.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

using namespace mp_units;
using namespace mp_units::iec::unit_symbols;
using namespace mp_units::si::unit_symbols;
using enum mp_units::quantity_character;

// unit conversions
static_assert(isq::storage_capacity(1 * B) == isq::storage_capacity(8 * bit));
static_assert(isq::storage_capacity(1024 * bit) == isq::storage_capacity(1 * Kibit));
static_assert(isq::storage_capacity(1024 * B) == isq::storage_capacity(1 * KiB));
static_assert(isq::storage_capacity(8 * 1024 * bit) == isq::storage_capacity(1 * KiB));
static_assert(isq::storage_capacity(8 * Kibit) == isq::storage_capacity(1 * KiB));

static_assert(isq::storage_capacity(1 * kbit) == isq::storage_capacity(1000 * bit));
static_assert(isq::storage_capacity(2000 * Mibit) == isq::storage_capacity(2097152 * kbit));

static_assert(isq::storage_capacity(1 * Kibit) == isq::storage_capacity(1024 * bit));
static_assert(isq::storage_capacity(1 * Mibit) == isq::storage_capacity(1024 * Kibit));
static_assert(isq::storage_capacity(1 * Gibit) == isq::storage_capacity(1024 * Mibit));
static_assert(isq::storage_capacity(1 * Tibit) == isq::storage_capacity(1024 * Gibit));
static_assert(isq::storage_capacity(1 * Pibit) == isq::storage_capacity(1024 * Tibit));
static_assert(isq::storage_capacity(1 * Eibit) == isq::storage_capacity(1024 * Pibit));

// transfer rate
static_assert(isq::storage_capacity(16 * B) / isq::duration(2 * s) == isq::transfer_rate(8 * B / s));
static_assert(isq::storage_capacity(120 * kB) / isq::duration(2 * min) == isq::transfer_rate(1000 * B / s));

// modulation rate
static_assert(12 / isq::duration(2 * s) == isq::modulation_rate(6 * Bd));
static_assert(6000 / isq::duration(3 * s) == isq::modulation_rate(2 * kBd));

}  // namespace
