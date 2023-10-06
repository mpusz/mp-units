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

#include <mp-units/quantity.h>
#include <mp-units/reference.h>
#include <mp-units/systems/iec80000/iec80000.h>
#include <mp-units/systems/si/unit_symbols.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

using namespace mp_units;
using namespace mp_units::iec80000;
using namespace mp_units::iec80000::unit_symbols;
using namespace mp_units::si::unit_symbols;
using enum mp_units::quantity_character;

[[nodiscard]] consteval bool verify(QuantitySpec auto q, quantity_character ch, Unit auto... units)
{
  return q.character == ch && (... && requires { q[units]; });
}

// quantities
static_assert(verify(traffic_intensity, scalar, E));
static_assert(verify(traffic_offered_intensity, scalar, E));
static_assert(verify(traffic_carried_intensity, scalar, E));
static_assert(verify(traffic_load, scalar, E));
static_assert(verify(mean_queue_length, scalar, one));
static_assert(verify(loss_probability, scalar, one));
static_assert(verify(waiting_probability, scalar, one));
static_assert(verify(call_intensity, scalar, one / s));
static_assert(verify(calling_rate, scalar, one / s));
static_assert(verify(completed_call_intensity, scalar, one / s));
static_assert(verify(storage_capacity, scalar, one, bit, o, B));
static_assert(verify(storage_size, scalar, one, bit, o, B));
static_assert(verify(equivalent_binary_storage_capacity, scalar, one, bit));
static_assert(verify(transfer_rate, scalar, one / s, o / s, B / s));
static_assert(verify(period_of_data_elements, scalar, s));
static_assert(verify(binary_digit_rate, scalar, one / s, bit / s));
static_assert(verify(bit_rate, scalar, one / s, bit / s));
static_assert(verify(period_of_binary_digits, scalar, s));
static_assert(verify(bit_period, scalar, s));
static_assert(verify(equivalent_binary_digit_rate, scalar, one / s, bit / s));
static_assert(verify(equivalent_bit_rate, scalar, one / s, bit / s));
static_assert(verify(modulation_rate, scalar, one / s, Bd));
static_assert(verify(line_digit_rate, scalar, one / s, Bd));
static_assert(verify(quantizing_distortion_power, scalar, W));
static_assert(verify(carrier_power, scalar, W));
static_assert(verify(signal_energy_per_binary_digit, scalar, J));
static_assert(verify(error_probability, scalar, one));
static_assert(verify(Hamming_distance, scalar, one));
static_assert(verify(clock_frequency, scalar, Hz));
static_assert(verify(clock_rate, scalar, Hz));
static_assert(verify(decision_content, scalar, one));


// unit conversions
static_assert(storage_capacity(1 * B) == storage_capacity(8 * bit));
static_assert(storage_capacity(1024 * bit) == storage_capacity(1 * Kibit));
static_assert(storage_capacity(1024 * B) == storage_capacity(1 * KiB));
static_assert(storage_capacity(8 * 1024 * bit) == storage_capacity(1 * KiB));
static_assert(storage_capacity(8 * Kibit) == storage_capacity(1 * KiB));

static_assert(storage_capacity(1 * kbit) == storage_capacity(1000 * bit));
static_assert(storage_capacity(2000 * Mibit) == storage_capacity(2097152 * kbit));

static_assert(storage_capacity(1 * Kibit) == storage_capacity(1024 * bit));
static_assert(storage_capacity(1 * Mibit) == storage_capacity(1024 * Kibit));
static_assert(storage_capacity(1 * Gibit) == storage_capacity(1024 * Mibit));
static_assert(storage_capacity(1 * Tibit) == storage_capacity(1024 * Gibit));
static_assert(storage_capacity(1 * Pibit) == storage_capacity(1024 * Tibit));
static_assert(storage_capacity(1 * Eibit) == storage_capacity(1024 * Pibit));

// transfer rate
static_assert(storage_capacity(16 * B) / isq::duration(2 * s) == transfer_rate(8 * B / s));
static_assert(storage_capacity(120 * kB) / isq::duration(2 * min) == transfer_rate(1000 * B / s));

// modulation rate
static_assert(12 / isq::duration(2 * s) == modulation_rate(6 * Bd));
static_assert(6000 / isq::duration(3 * s) == modulation_rate(2 * kBd));

}  // namespace
