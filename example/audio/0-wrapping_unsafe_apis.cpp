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
#endif

#include "third_party_audio_api.h"
#include "wrapped_third_party_audio_api.h"

int main()
{
  // Operating system or host applications will provide APIs with various musical context information such as tempo and
  // sample rate, but these APIs are not type safe using float or double values; e.g.:
  const auto unsafe_context = audio_third_party::get_musical_context();

  std::cout << MP_UNITS_STD_FMT::format("Musical context:\n\tTempo: {}\n\tSample Rate: {}\n\n", unsafe_context.tempo,
                                        unsafe_context.sample_rate);

  // These unsafe APIs can be wrapped a new API returning type-safe quantities for tempo and sample rate; e.g.:
  const auto safe_context = audio::get_musical_context();

  std::cout << MP_UNITS_STD_FMT::format("Musical context:\n\tTempo: {}\n\tSample Rate: {}\n", safe_context.tempo,
                                        safe_context.sample_rate);
}
