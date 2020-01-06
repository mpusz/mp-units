
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


#include <units/physical/si/length.h>

/*
    get conversion factor from one dimensionally-equivalent 
    quantity type to another
*/

namespace {

  
   template < 
      units::Quantity Target,
      units::Quantity Source
   >
   requires units::equivalent_dim<typename Source::dimension,typename Target::dimension>
   constexpr inline
   std::common_type_t<
      typename Target::rep,
      typename Source::rep
   >
   conversion_factor()
   {
      // get quantities looking like inputs but with Q::rep that doesnt have narrowing conversion
      typedef std::common_type_t<
         typename Target::rep,
         typename Source::rep
      > rep;
      typedef units::quantity<typename Source::dimension,typename Source::unit,rep> source;
      typedef units::quantity<typename Target::dimension,typename Target::unit,rep> target;
      return target{source{1}}.count();
   }

   auto units_str( units::Quantity const & q)
   {
       typedef std::remove_cvref_t<decltype(q)> qtype;
       return units::detail::unit_text<typename qtype::dimension, typename qtype::unit>();
   }

}

namespace {

    namespace length{
       using m = units::si::length<units::si::metre,double>;
       using mm = units::si::length<units::si::millimetre,double>;
   }

}

#include <iostream>

using namespace units::si::literals;
int main()
{
   length::m constexpr plankA = 2.0m;
   length::mm constexpr plankB = 1000.0mm;

   std::cout << "ratio  plankA / plankB = " << plankA / plankB << '\n'; 

   std::cout << "conversion factor to convert from vS in " << units_str(plankA) ;
   std::cout << " to vT in " << units_str(plankB) << " : vT = vS * ";
   std::cout << conversion_factor<length::mm,length::m >() << '\n';

   // can be evaluated at compile time
   static_assert(conversion_factor<length::mm,length::m>() == 1000,"error");

}
