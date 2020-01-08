
/*
 Copyright (c) 2003-2019 Andy Little.

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

#include <units/physical/si/length.h>
#include <units/physical/us/length.h>
#include <units/physical/si/area.h>
#include <units/physical/si/volume.h>
#include <units/physical/si/time.h>

namespace {
   namespace length{

      template <typename Rep = double>
      using m =  units::si::length<units::si::metre,Rep>;

      template <typename Rep = double>
      using mm = units::si::length<units::si::millimetre,Rep>;

      template <typename Rep = double>
      using fm = units::si::length<units::si::femtometre,Rep>;

      template <typename Rep = double>
      using km = units::si::length<units::si::kilometre,Rep>;

      template <typename Rep = double>
      using AU = units::si::length<units::si::astronomical_unit,Rep>;

      template <typename Rep = double>
      using in = units::si::length<units::us::inch,Rep>;

      template <typename Rep = double>
      using angstrom  = units::si::length<units::si::angstrom,Rep>;

      template <typename Rep = double>
      using ch = units::si::length<units::si::chain,Rep>;

      template <typename Rep = double>
      using fathom = units::si::length<units::si::fathom,Rep>;

      template <typename Rep = double>
      using fathom_us = units::si::length<units::si::fathom_us,Rep>;

      template <typename Rep = double>
      using ft = units::si::length<units::us::foot,Rep>;

      template <typename Rep = double>
      using ft_us = units::si::length<units::si::foot_us,Rep>;

      template <typename Rep = double>
      using ly = units::si::length<units::si::light_year,Rep>;

      template <typename Rep = double>
      using mi = units::si::length<units::si::mile,Rep>;

      template <typename Rep = double>
      using mi_naut = units::si::length<units::si::nautical_mile,Rep>;

      template <typename Rep = double>
      using pc = units::si::length<units::si::parsec,Rep>;

      template <typename Rep = double>
      using pica_comp = units::si::length<units::si::pica_comp,Rep>;

      template <typename Rep = double>
      using pica_prn = units::si::length<units::si::pica_prn,Rep>;

      template <typename Rep = double>
      using point_comp = units::si::length<units::si::point_comp,Rep>;

      template <typename Rep = double>
      using point_prn = units::si::length<units::si::point_prn,Rep>;

      template <typename Rep = double>
      using rd = units::si::length<units::si::rod,Rep>;

      template <typename Rep = double>
      using yd = units::si::length<units::us::yard,Rep>;

   }

   namespace time{

      template <typename Rep = double>
      using s = units::si::time<units::si::second,Rep>;

      template <typename Rep = double>
      using min = units::si::time<units::si::minute,Rep>;

      template <typename Rep = double>
      using h = units::si::time<units::si::hour,Rep>;
   }

   namespace area{

      template <typename Rep = double>
      using m2 = units::si::area<units::si::square_metre,Rep>;

      template <typename Rep = double>
      using fm2 = units::si::area<units::si::square_femtometre,Rep>;
   }
}

#include <iostream>

using namespace units::si::literals;

void simple_quantities()
{
    using distance = length::m<>;
    using time = time::s<>;
    distance constexpr km = 1.0km;
    distance constexpr miles = 1q_mi;

    time constexpr sec = 1s;
    time constexpr min = 1min;
    time constexpr hr = 1h;

    std::cout << "A physical quantities library can choose the simple\n";
    std::cout << "option to provide output using a single type for each base unit:\n\n"; 
    std::cout << km << '\n';
    std::cout << miles << '\n';
    std::cout << sec << '\n';
    std::cout << min << '\n';
    std::cout << hr  <<  "\n\n";
}

void quantities_with_typed_units()
{
    length::km<> constexpr km = 1km;

    length::mi<> constexpr miles = 1q_mi;

    std::cout.precision(6);

    time::s<> constexpr  sec = 1s;
    time::min<> constexpr min = 1min;
    time::h<> constexpr  hr  = 1h;

    std::cout << "A more flexible option is to provide separate types for each unit,\n\n"; 
    std::cout << km << '\n';
    std::cout << miles << '\n';
    std::cout << sec << '\n';
    std::cout << min << '\n';
    std::cout << hr  << "\n\n";

    length::m<> constexpr meter = 1m;
    std::cout << "then a wide range of pre-defined units can be defined and converted,\n"
    " for consistency and repeatability across applications:\n\n";

    std::cout << meter << '\n' ;
   
    std::cout << " = " << length::AU<>(meter) << '\n'; 
    std::cout << " = " << length::angstrom<>(meter) << '\n'; 
    std::cout << " = " << length::ch<>(meter) << '\n'; 
    std::cout << " = " << length::fathom<>(meter) << '\n'; 
    std::cout << " = " << length::fathom_us<>(meter) << '\n';
    std::cout << " = " << length::ft<>(meter) << '\n'; 
    std::cout << " = " << length::ft_us<>(meter) << '\n';  
    std::cout << " = " << length::in<>(meter) << '\n'; 
    std::cout << " = " << length::ly<>(meter) << '\n';  
    std::cout << " = " << length::mi<>(meter) << '\n'; 
    std::cout << " = " << length::mi_naut<>(meter) << '\n'; 
    std::cout << " = " << length::pc<>(meter) << '\n'; 
    std::cout << " = " << length::pica_comp<>(meter) << '\n';  
    std::cout << " = " << length::pica_prn<>(meter) << '\n'; 
    std::cout << " = " << length::point_comp<>(meter) << '\n'; 
    std::cout << " = " << length::point_prn<>(meter) << '\n'; 
    std::cout << " = " << length::rd<>(meter) << '\n'; 
    std::cout << " = " << length::yd<>(meter) << '\n'; 
}

void calcs_comparison()
{
    std::cout.precision(20);
    std::cout << "\nA distinct unit for each type is efficient and accurate\n"
    "when adding two values of the same very big\n"
    "or very small type:\n\n";

    length::fm<float> L1A = 2fm; 

    length::fm<float> L2A = 3fm;

    length::fm<float> LrA = L1A + L2A;

    std::cout << L1A << " + " << L2A << " = " << LrA << "\n\n";

    std::cout << "The single unit method must convert large\n"
    "or small values in other units to the base unit.\n"
    "This is both inefficient and inaccurate\n\n";

    length::m<float> L1B = L1A;
    length::m<float> L2B = L2A;
    length::m<float> LrB = L1B + L2B;

    std::cout << L1B << " + " << L2B << " = " << LrB << "\n\n";

    std::cout << "In multiplication and division:\n\n";

    area::fm2<float> ArA = L1A * L2A ;
    std::cout << L1A << " * " << L2A << " = " << ArA << "\n\n";
    std::cout <<"similar problems arise\n\n";
    area::m2<float> ArB = L1B * L2B;
    std::cout << L1B << " * " << L2B << "\n = " << ArB << '\n';

}

int main()
{
   std::cout << "This demo was originally posted on com.lang.c++.moderated in 2006\n";
   std::cout << "http://compgroups.net/comp.lang.c++.moderated/dimensional-analysis-units/51712\n";
   std::cout << "Here converted to use mpusz/units library.\n\n";

   simple_quantities();
   quantities_with_typed_units();
   calcs_comparison();
}
