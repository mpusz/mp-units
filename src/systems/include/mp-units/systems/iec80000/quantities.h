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

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/dimension.h>
#include <mp-units/quantity_spec.h>
#endif

MP_UNITS_EXPORT
namespace mp_units::iec80000 {

// dimensions of base quantities
// clang-format off
inline constexpr struct dim_traffic_intensity : base_dimension<"A"> {} dim_traffic_intensity;
// clang-format on

// quantities
QUANTITY_SPEC(traffic_intensity, dim_traffic_intensity);
QUANTITY_SPEC(traffic_offered_intensity, traffic_intensity);
QUANTITY_SPEC(traffic_carried_intensity, traffic_intensity);
inline constexpr auto traffic_load = traffic_carried_intensity;
QUANTITY_SPEC(mean_queue_length, dimensionless);
QUANTITY_SPEC(loss_probability, dimensionless);
QUANTITY_SPEC(waiting_probability, dimensionless);
QUANTITY_SPEC(call_intensity, inverse(isq::duration));
inline constexpr auto calling_rate = call_intensity;
QUANTITY_SPEC(completed_call_intensity, call_intensity);
QUANTITY_SPEC(storage_capacity, dimensionless, is_kind);
inline constexpr auto storage_size = storage_capacity;
QUANTITY_SPEC(equivalent_binary_storage_capacity, storage_capacity);
QUANTITY_SPEC(transfer_rate, storage_capacity / isq::duration);
QUANTITY_SPEC(period_of_data_elements, isq::period, inverse(transfer_rate));
QUANTITY_SPEC(binary_digit_rate, transfer_rate);
inline constexpr auto bit_rate = binary_digit_rate;
QUANTITY_SPEC(period_of_binary_digits, isq::period, inverse(binary_digit_rate));
inline constexpr auto bit_period = period_of_binary_digits;
QUANTITY_SPEC(equivalent_binary_digit_rate, binary_digit_rate);
inline constexpr auto equivalent_bit_rate = bit_rate;
QUANTITY_SPEC(modulation_rate, inverse(isq::duration));
inline constexpr auto line_digit_rate = modulation_rate;
QUANTITY_SPEC(quantizing_distortion_power, isq::power);
QUANTITY_SPEC(carrier_power, isq::power);
QUANTITY_SPEC(signal_energy_per_binary_digit, carrier_power* period_of_binary_digits);
QUANTITY_SPEC(error_probability, dimensionless);
QUANTITY_SPEC(Hamming_distance, dimensionless);
QUANTITY_SPEC(clock_frequency, isq::frequency);
inline constexpr auto clock_rate = clock_frequency;
QUANTITY_SPEC(decision_content, dimensionless);

// TODO how to model information_content and the following quantities???
// QUANTITY_SPEC(information_content, ...);

}  // namespace mp_units::iec80000
