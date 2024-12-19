// The MIT License (MIT)
//
// Copyright (c) 2024 Roth Michaels
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

#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/framework/quantity_spec.h>
#include <mp-units/framework/unit.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

namespace audio {

QUANTITY_SPEC(sample_count, mp_units::dimensionless, mp_units::is_kind);
QUANTITY_SPEC(sample_duration, mp_units::isq::time);
QUANTITY_SPEC(sample_rate, mp_units::isq::frequency, sample_count / mp_units::isq::time);

inline constexpr struct sample final : mp_units::named_unit<"Smpl", mp_units::one, mp_units::kind_of<sample_count>> {
} sample;

QUANTITY_SPEC(unit_sample_amount, mp_units::dimensionless, mp_units::is_kind);

inline constexpr struct sample_value final :
    mp_units::named_unit<"PCM", mp_units::one, mp_units::kind_of<unit_sample_amount>> {
} sample_value;

QUANTITY_SPEC(beat_count, mp_units::dimensionless, mp_units::is_kind);
QUANTITY_SPEC(beat_duration, mp_units::isq::time);
QUANTITY_SPEC(tempo, mp_units::isq::frequency, beat_count / mp_units::isq::time);

inline constexpr struct quarter_note final : mp_units::named_unit<"q", mp_units::one, mp_units::kind_of<beat_count>> {
} quarter_note;

inline constexpr struct whole_note final : mp_units::named_unit<"w", mp_units::mag<4> * quarter_note> {
} whole_note;
inline constexpr struct half_note final : mp_units::named_unit<"h", mp_units::mag<2> * quarter_note> {
} half_note;
inline constexpr struct dotted_half_note final : mp_units::named_unit<"h.", mp_units::mag<3> * quarter_note> {
} dotted_half_note;
inline constexpr struct eigth_note final : mp_units::named_unit<"8th", mp_units::mag_ratio<1, 2> * quarter_note> {
} eigth_note;
inline constexpr struct dotted_quarter_note final : mp_units::named_unit<"q.", mp_units::mag<3> * eigth_note> {
} dotted_quarter_note;
inline constexpr struct quarter_note_triplet final : mp_units::named_unit<"qt", mp_units::mag_ratio<1, 3> * half_note> {
} quarter_note_triplet;
inline constexpr struct sixteenth_note final : mp_units::named_unit<"16th", mp_units::mag_ratio<1, 2> * eigth_note> {
} sixteenth_note;
inline constexpr struct dotted_eigth_note final : mp_units::named_unit<"8th.", mp_units::mag<3> * sixteenth_note> {
} dotted_eigth_note;

inline constexpr struct beats_per_minute final : mp_units::named_unit<"bpm", quarter_note / mp_units::non_si::minute> {
} beats_per_minute;

namespace unit_symbols {
inline constexpr auto smpl = sample;

inline constexpr auto pcm = sample_value;

// inline constexpr auto n_wd = 3 * half_note;
inline constexpr auto n_w = whole_note;
// inline constexpr auto n_hd = dotted_half_note;
// inline constexpr auto n_h = half_note;
// inline constexpr auto n_qd = dotted_quarter_note;
// inline constexpr auto n_q = quarter_note;
// inline constexpr auto n_qt = quarter_note_triplet;
// inline constexpr auto n_8thd = dotted_eigth_note;
// inline constexpr auto n_8th = eigth_note;
// inline constexpr auto n_16th = sixteenth_note;

// inline constexpr auto bpm = beats_per_minute;
}  // namespace unit_symbols
}  // namespace audio
