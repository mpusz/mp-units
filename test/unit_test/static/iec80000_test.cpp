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

#include <units/systems/iec80000/iec80000.h>
#include <units/systems/si/unit_symbols.h>

/* ************** DERIVED DIMENSIONS THAT INCLUDE UNITS WITH SPECIAL NAMES **************** */

namespace {

using namespace units;
using namespace units::iec80000;
using namespace units::iec80000::unit_symbols;
using namespace units::si::unit_symbols;
using enum units::quantity_character;

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
static_assert(verify(call_intensity, scalar, 1 / s));
static_assert(verify(calling_rate, scalar, 1 / s));
static_assert(verify(completed_call_intensity, scalar, 1 / s));
static_assert(verify(storage_capacity, scalar, one, bit, o, B));
static_assert(verify(storage_size, scalar, one, bit, o, B));
static_assert(verify(equivalent_binary_storage_capacity, scalar, one, bit));
static_assert(verify(transfer_rate, scalar, 1 / s, o / s, B / s));
static_assert(verify(period_of_data_elements, scalar, s));
static_assert(verify(binary_digit_rate, scalar, 1 / s, bit / s));
static_assert(verify(bit_rate, scalar, 1 / s, bit / s));
static_assert(verify(period_of_binary_digits, scalar, s));
static_assert(verify(bit_period, scalar, s));
static_assert(verify(equivalent_binary_digit_rate, scalar, 1 / s, bit / s));
static_assert(verify(equivalent_bit_rate, scalar, 1 / s, bit / s));
static_assert(verify(modulation_rate, scalar, 1 / s, Bd));
static_assert(verify(line_digit_rate, scalar, 1 / s, Bd));
static_assert(verify(quantizing_distortion_power, scalar, W));
static_assert(verify(carrier_power, scalar, W));
static_assert(verify(signal_energy_per_binary_digit, scalar, J));
static_assert(verify(error_probability, scalar, one));
static_assert(verify(Hamming_distance, scalar, one));
static_assert(verify(clock_frequency, scalar, Hz));
static_assert(verify(clock_rate, scalar, Hz));
static_assert(verify(decision_content, scalar, one));


// unit conversions
static_assert(storage_capacity[B](1) == storage_capacity[bit](8));
static_assert(storage_capacity[bit](1024) == storage_capacity[Kibit](1));
static_assert(storage_capacity[B](1024) == storage_capacity[KiB](1));
static_assert(storage_capacity[bit](8 * 1024) == storage_capacity[KiB](1));
static_assert(storage_capacity[Kibit](8) == storage_capacity[KiB](1));

static_assert(storage_capacity[kbit](1) == storage_capacity[bit](1000));
static_assert(storage_capacity[Mibit](2000) == storage_capacity[kbit](2097152));

static_assert(storage_capacity[Kibit](1) == storage_capacity[bit](1024));
static_assert(storage_capacity[Mibit](1) == storage_capacity[Kibit](1024));
static_assert(storage_capacity[Gibit](1) == storage_capacity[Mibit](1024));
static_assert(storage_capacity[Tibit](1) == storage_capacity[Gibit](1024));
static_assert(storage_capacity[Pibit](1) == storage_capacity[Tibit](1024));
static_assert(storage_capacity[Eibit](1) == storage_capacity[Pibit](1024));

// transfer rate
static_assert(storage_capacity[B](16) / isq::duration[s](2) == transfer_rate[B / s](8));
static_assert(storage_capacity[kB](120) / isq::duration[min](2) == transfer_rate[B / s](1000));

// modulation rate
static_assert(12 / isq::duration[s](2) == modulation_rate[Bd](6));
static_assert(6000 / isq::duration[s](3) == modulation_rate[kBd](2));

}  // namespace
