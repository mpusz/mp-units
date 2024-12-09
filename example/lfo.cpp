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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <exception>
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ostream.h>
// #include <mp-units/systems/cgs.h>
// #include <mp-units/systems/international.h>
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

namespace third_party {

//! State of a playback engine for music host application.
struct musical_context {
  float sample_rate;  //!< samples per second
  float tempo;        //!< beats per minute (quarter note == one beat)
};

//! API provided by music host application to provide global info
//! about the playback engine.
musical_context get_musical_context()
{
  // Example data, this would be variable in a real-world context
  return musical_context{.sample_rate = 8000.f, .tempo = 130.f};
}
}  // namespace third_party

namespace {
using namespace mp_units;

QUANTITY_SPEC(sample_count, dimensionless, is_kind);
QUANTITY_SPEC(sample_duration, isq::time);
QUANTITY_SPEC(sample_rate, isq::frequency, sample_count / isq::time);

inline constexpr struct sample final : named_unit<"Smpl", one, kind_of<sample_count>> {
} sample;

QUANTITY_SPEC(unit_sample_amount, dimensionless, is_kind);

inline constexpr struct sample_value final : named_unit<"PCM", one, kind_of<unit_sample_amount>> {
} sample_value;

QUANTITY_SPEC(beat_count, dimensionless, is_kind);
QUANTITY_SPEC(beat_duration, isq::time);
QUANTITY_SPEC(tempo, isq::frequency, beat_count / isq::time);

inline constexpr struct quarter_note final : named_unit<"q", one, kind_of<beat_count>> {
} quarter_note;
static_assert(detail::SymbolicArg<struct quarter_note>);
static_assert(Unit<struct quarter_note>);

inline constexpr struct whole_note final : named_unit<"w", mag<4> * quarter_note> {
} whole_note;
inline constexpr struct half_note final : named_unit<"h", mag<2> * quarter_note> {
} half_note;
inline constexpr struct dotted_half_note final : named_unit<"h.", mag<3> * quarter_note> {
} dotted_half_note;
inline constexpr struct eigth_note final : named_unit<"8th", mag_ratio<1, 2> * quarter_note> {
} eigth_note;
inline constexpr struct dotted_quarter_note final : named_unit<"q.", mag<3> * eigth_note> {
} dotted_quarter_note;
inline constexpr struct quarter_note_triplet final : named_unit<"qt", mag_ratio<1, 3> * half_note> {
} quarter_note_triplet;
inline constexpr struct sixteenth_note final : named_unit<"16th", mag_ratio<1, 2> * eigth_note> {
} sixteenth_note;
inline constexpr struct dotted_eigth_note final : named_unit<"8th.", mag<3> * sixteenth_note> {
} dotted_eigth_note;

inline constexpr struct beats_per_minute final : named_unit<"bpm", quarter_note / non_si::minute> {
} beats_per_minute;

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

//! Typesafe version of music application playback engine state
struct musical_context {
  quantity<sample_rate[si::hertz], float> sample_rate;
  quantity<beats_per_minute, float> tempo;
};

//! Typesafe wrapper around API for host application musical context
musical_context get_musical_context()
{
  auto context = third_party::get_musical_context();
  return musical_context{.sample_rate = context.sample_rate * si::hertz, .tempo = context.tempo * beats_per_minute};
}

class sine_wave_osc {
public:
  sine_wave_osc(const musical_context& context, QuantityOf<isq::frequency> auto freq) :
      m_context{context}, m_frequency{freq}
  {
    std::cout << std::format("Created LFO with starting frequency {} ({}) for sample rate {} at tempo {}\n", freq,
                             m_frequency, context.sample_rate, context.tempo);
  }

  quantity<si::hertz, float> get_frequency() const { return m_frequency; }

  void set_frequency(QuantityOf<isq::frequency> auto freq)
  {
    m_frequency = freq;
    std::cout << std::format("Setting frequency to {} ({})\n", freq, m_frequency);
  }

  void set_period(QuantityOf<isq::time> auto period)
  {
    m_frequency = 1.f / period;
    std::cout << std::format("Setting period to {} (i.e. frequency to {})\n", period, m_frequency);
  }

  quantity<sample_value, float> operator()()
  {
    auto out = sin(m_phase);
    m_phase += step;
    return out;
  }

private:
  void update_step() { m_step = (angular::revolution * m_frequency) / m_context.sample_rate; }

  quantity<sample_value> sin(quantity_point<angular::radian, float>)
  {
    return std::sin(m_phase.numerical_value_in(angular::radian)) * pcm;
  }

  musical_context m_context;
  quantity<si::hertz, float> m_frequency;
  quantity_point<angular::radian, float> m_phase{0.f};
  quantity<angular::radian, float> m_step;
};
}  // namespace

int main()
{
  using namespace mp_units::si::unit_symbols;

  const auto context = get_musical_context();

  auto sin_gen = sine_wave_osc{context, 2 * Hz};

  // TODO set_frequency and set_period calls for demonstrating use of different units


  auto buffer = std::vector<quantity<sample_value, float>>(2 * whole_note)
}
