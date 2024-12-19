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
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

#include "audio.h"
#include "wrapped_third_party_audio_api.h"

namespace {
using namespace mp_units;

class sine_wave_osc {
public:
  sine_wave_osc(const audio::musical_context& context, QuantityOf<isq::frequency> auto freq) :
      m_context{context}, m_frequency{freq}
  {
    std::cout << MP_UNITS_STD_FMT::format(
      "Created oscillator with starting frequency {} ({}) for sample rate {} at tempo {}\n", freq, m_frequency,
      context.sample_rate, context.tempo);
  }

  quantity<si::hertz, float> get_frequency() const { return m_frequency; }

  void set_frequency(QuantityOf<isq::frequency> auto freq)
  {
    m_frequency = freq;
    std::cout << MP_UNITS_STD_FMT::format("Setting frequency to {} ({})\n", freq, m_frequency);
  }

  void set_period(QuantityOf<isq::time> auto period)
  {
    m_frequency = 1.f / period;
    std::cout << MP_UNITS_STD_FMT::format("Setting period to {} (i.e. frequency to {})\n", period, m_frequency);
  }

  quantity<audio::sample_value, float> operator()()
  {
    auto out = angular::sin(m_phase.quantity_from_zero());
    m_phase += m_step;
    return out;
  }

private:
  using phase_t = quantity_point<angular::radian, mp_units::default_point_origin(angular::radian), float>;

  void update_step() { m_step = (m_frequency / m_context.sample_rate) * angular::revolution; }

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


  auto sin_gen = sine_wave_osc{context, 1 * Hz};

  // TODO set_frequency and set_period calls for demonstrating use of different units


  // auto buffer = std::vector<quantity<audio::sample_value, float>>(2 * audio::whole_note)
}
