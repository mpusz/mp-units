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

#include "audio.h"
#include "third_party_audio_api.h"

namespace audio {

//! Typesafe version of music application playback engine state
struct musical_context {
  mp_units::quantity<sample_rate[mp_units::si::hertz], float> current_sample_rate;
  mp_units::quantity<beats_per_minute, float> current_tempo;
};

//! Typesafe wrapper around API for host application musical context
musical_context get_musical_context()
{
  auto context = audio_third_party::get_musical_context();
  return musical_context{.current_sample_rate = context.current_sample_rate * mp_units::si::hertz,
                         .current_tempo = context.current_tempo * beats_per_minute};
}

}  // namespace audio
