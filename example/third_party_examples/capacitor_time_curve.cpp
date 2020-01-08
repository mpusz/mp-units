
/*
 Copyright (c) 2003-2020 Andy Little.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see http://www.gnu.org/licenses./
*/

/*
    capacitor discharge curve using compile_time
    physical_quantities
*/

#include <units/physical/si/capacitance.h>
#include <units/physical/si/resistance.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/time.h>
#include <cmath>

namespace {
   namespace voltage {

       template <typename Rep = double>
       using V = units::si::voltage<units::si::volt,Rep>;

       template <typename Rep = double>
       using mV = units::si::voltage<units::si::millivolt,Rep>;

       template <typename Rep = double>
       using uV = units::si::voltage<units::si::microvolt,Rep>;

       template <typename Rep = double>
       using nV = units::si::voltage<units::si::nanovolt,Rep>;

       template <typename Rep = double>
       using pV = units::si::voltage<units::si::picovolt,Rep>;
   }
}

#include <iostream>

using namespace units::si::literals;
int main()
{
    std::cout << "mpusz/units capacitor time curve example...\n";
    std::cout.setf(std::ios_base::fixed,std::ios_base::floatfield);
    std::cout.precision(3);

    auto constexpr C = 0.47uF;
    auto constexpr V0 = 5.0V;
    auto constexpr R = 4.7kR;

    for ( auto t = 0ms ; t <= 50ms; ++t  ){

        auto const Vt = V0 * std::exp(-t / (R * C));

        std::cout << "at " << t << " voltage is " ;

        if     ( Vt >= 1V )    std::cout << Vt ;
        else if( Vt >= 1mV )   std::cout << voltage::mV<>{Vt};
        else if( Vt >= 1uV )   std::cout << voltage::uV<>{Vt};
        else if( Vt >= 1nV )   std::cout << voltage::nV<>{Vt};
        else                   std::cout << voltage::pV<>{Vt};
        std::cout << "\n";
    }
}
