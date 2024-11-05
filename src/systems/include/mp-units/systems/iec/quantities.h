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

#include <mp-units/systems/isq/information_science_and_technology.h>

MP_UNITS_EXPORT
namespace mp_units::iec {

[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto dim_traffic_intensity =
  isq::dim_traffic_intensity;

[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto traffic_intensity =
  isq::traffic_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto traffic_offered_intensity =
  isq::traffic_offered_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto traffic_carried_intensity =
  isq::traffic_carried_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto traffic_load =
  isq::traffic_carried_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto mean_queue_length =
  isq::mean_queue_length;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto loss_probability =
  isq::loss_probability;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto waiting_probability =
  isq::waiting_probability;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto call_intensity = isq::call_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto calling_rate = isq::call_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto completed_call_intensity =
  isq::completed_call_intensity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto storage_capacity =
  isq::storage_capacity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto storage_size = isq::storage_capacity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto equivalent_binary_storage_capacity =
  isq::equivalent_binary_storage_capacity;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto transfer_rate = isq::transfer_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto period_of_data_elements =
  isq::period_of_data_elements;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto binary_digit_rate =
  isq::binary_digit_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto bit_rate = isq::binary_digit_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto period_of_binary_digits =
  isq::period_of_binary_digits;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto bit_period =
  isq::period_of_binary_digits;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto equivalent_binary_digit_rate =
  isq::equivalent_binary_digit_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto equivalent_bit_rate = isq::bit_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto modulation_rate =
  isq::modulation_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto line_digit_rate =
  isq::modulation_rate;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto quantizing_distortion_power =
  isq::quantizing_distortion_power;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto carrier_power = isq::carrier_power;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto signal_energy_per_binary_digit =
  isq::signal_energy_per_binary_digit;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto error_probability =
  isq::error_probability;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto Hamming_distance =
  isq::Hamming_distance;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto clock_frequency =
  isq::clock_frequency;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto clock_rate = isq::clock_frequency;
[[deprecated("Quantity type moved to the `isq` namespace")]] inline constexpr auto decision_content =
  isq::decision_content;

}  // namespace mp_units::iec
