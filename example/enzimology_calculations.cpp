// The MIT License (MIT)
//
// Copyright (c) 2023 Meskauskas Audrius
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

#include <mp-units/systems/enzimology/enzimology.h>
#include <mp-units/ostream.h>
#include <mp-units/quantity.h>
#include <mp-units/quantity_point.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::si;

int main()
 {
    // In the test tube with 2 ml of test solution, 10 micro-molles of the substrate have been processed in 5 minutes.
    auto time = 5. * min;
    auto processed_substrate = 10. * umol;
    auto volume = 2. * milli<litre>;

    // 1 unit (U) is the amount of enzyme that catalyses the reaction of 1 µmol of substrate per minute

    auto units = processed_substrate / time;
    quantity<enzimology::nanokatal> nanokatals = units; // conversion if we want, 1 U is about 16.67 nkat

    auto activity = units / volume;

    auto protein_concentration = (20.0 * mg) / (1.0 * milli<litre>);
    quantity<enzimology::specific_activity> specific_activity = activity / protein_concentration;

    std::cout << "Enzyme units " << units << " or  " << nanokatals <<
      ". Activity " << activity << ", specific activity " << specific_activity << ".\n";
}
