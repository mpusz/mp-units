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

#include <mp-units/ext/format.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

#include "audio.h"
#include "wrapped_third_party_audio_api.h"

namespace {
using namespace mp_units;

//! A DSP generator class that generates sample values for a sine wave oscillator.
class sine_wave_osc {
public:
  sine_wave_osc(const audio::musical_context& context, QuantityOf<isq::frequency> auto freq) :
      m_context{context}, m_frequency{freq}
  {
    std::cout << MP_UNITS_STD_FMT::format(
      "Created oscillator with starting frequency {} ({}) for sample rate {} at tempo {}\n", freq, m_frequency,
      context.current_sample_rate, context.current_tempo);
  }

  quantity<si::hertz, float> get_frequency() const { return m_frequency; }

  void set_frequency(QuantityOf<isq::frequency> auto freq)
  {
    m_frequency = freq;
    std::cout << MP_UNITS_STD_FMT::format("Setting frequency to {} ({})\n", freq, m_frequency);
  }

  void set_period(QuantityOf<isq::time> auto period)
  {
    m_frequency = inverse<si::hertz>(period);
    std::cout << MP_UNITS_STD_FMT::format("Setting period to {} (i.e. frequency to {})\n", period, m_frequency);
  }

  void set_period(QuantityOf<audio::beat_count> auto period)
  {
    std::cout << MP_UNITS_STD_FMT::format("Setting period to {} -- ", period);
    set_period(value_cast<float>(period) / m_context.current_tempo);
  }

  quantity<audio::sample_value, float> operator()()
  {
    auto out = angular::sin(m_phase.quantity_from_zero());
    m_phase += m_step;
    return out;
  }

  void reset() { m_phase = phase_t{0.f * angular::radian}; }

private:
  using phase_t = quantity_point<angular::radian, mp_units::default_point_origin(angular::radian), float>;

  void update_step() { m_step = (m_frequency / m_context.current_sample_rate) * angular::revolution; }

  audio::musical_context m_context;
  quantity<si::hertz, float> m_frequency;
  phase_t m_phase{0.f * angular::radian};
  quantity<angular::radian, float> m_step;
};
}  // namespace

int main()
{
  using namespace mp_units::si::unit_symbols;

  const auto context = audio::get_musical_context();

  // Sine oscillators are sometimes used as a "low-frequency oscillator"
  // (LFO) that runs at a frequency below the range of human hearing and
  // is used a source of modulation for other paramters in an audio
  // algorithm.
  auto sin_gen = sine_wave_osc{context, 1 * Hz};

  // Depending on the use-case sometimes an LFO will be set with a frequency in Hz
  sin_gen.set_frequency(13 * Hz);

  // for some use-cases it is more convient for a user to set the period
  sin_gen.set_period(42 * s);

  // and in some other use-cases setting the period in musical note duration is more intuitive
  sin_gen.set_period(1 * audio::half_note);

  // Our oscillator can be used to generate sample values for a buffer
  // of audio samples.  In this example we will create a buffer with
  // duration equal to 2 measures of 4/4 music (i.e. 2 whole notes at
  // the current tempo):
  const quantity beats = 2 * audio::whole_note;
  const quantity buffer_duration = value_cast<float>(beats) / context.current_tempo;
  const quantity buffer_size =
    quantity_cast<audio::sample_count>((buffer_duration * context.current_sample_rate).in(one));

  std::cout << MP_UNITS_STD_FMT::format("\nCreating buffer with size:\n\t{}\n\t{}\n\t{}\n\n", beats, buffer_duration,
                                        buffer_size);

  using buffer_t = std::vector<quantity<audio::sample_value, float>>;

  auto buffer_1 = buffer_t(std::size_t(buffer_size.numerical_value_in(audio::sample)));

  std::cout << MP_UNITS_STD_FMT::format("Filling buffer with values from LFO @ {}", sin_gen.get_frequency());
  std::generate(begin(buffer_1), end(buffer_1), sin_gen);

  std::cout << MP_UNITS_STD_FMT::format("\nLFO Values:\n[{}", buffer_1[0u]);
  for (const auto sampleValue : std::ranges::subrange(begin(buffer_1) + 1, end(buffer_1))) {
    std::cout << MP_UNITS_STD_FMT::format(", {}", sampleValue);
  }
  std::cout << "]\n\n";

  // generated values should be the same after resetting oscillator
  sin_gen.reset();
  auto buffer_2 = buffer_t(buffer_1.size());
  std::generate(begin(buffer_2), end(buffer_2), sin_gen);

  return buffer_1 == buffer_2;
}
