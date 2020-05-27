

/*
   By using the in_quantity concept for quantity operator signatures rather tha type direct,
   we can make stdL::chrono duration a model of quantity
*/
#include <units/physical/si/speed.h>
#include <units/physical/si/time.h>

#include <chrono>

namespace units{ 

 // Here we  make chrono::duration fit the requirements for units::in_quantity conscept

   namespace detail{

      // make std::chrono::duration a model of quantity
      template< typename Rep, typename Period >
      inline constexpr bool is_quantity<std::chrono::duration<Rep, Period> > = true;
   }

   // N.B. These metafunctions are used to access the traits in models of quantity rather than poking the class template direct
   // In practise it would be best to add a get_count(q) function to requirements rather than use q.count();
   template <typename Rep>
   struct get_dimension<std::chrono::duration<Rep> >{
      using type = typename get_dimension<units::physical::si::time<units::physical::si::second> >::type; 
   };

   template <typename Rep>
   struct get_unit<std::chrono::duration<Rep> >{
      using type = typename get_unit<units::physical::si::time<units::physical::si::second> >::type; 
   };

}

#include <iostream>
namespace {
template <typename Rep>
std::ostream & operator << (std::ostream & os, std::chrono::duration<Rep> const & v)
{
      return os << v.count() << " s";
}
}

using namespace units::physical::si::literals;
using namespace std::literals::chrono_literals;

int main()
{
   std::cout << "Demonstration of superiority of concept operator function signatures.\n"
                "Here we make std::chrono::duration a model of units::in_quantity concept\n\n";
   auto v1 = 1000.0q_m_per_s;  // units_quantity
   auto v2 = 25.s;           // std::chrono::duration

   std::cout << "v1 = " << v1 <<'\n';

   std::cout << "v2 = " << v2 <<'\n';

   auto v3 = v1 * v2;       // <-- here we multiply a velocity by a chrono duration

   std::cout << v1 << " * " << v2 << " = " << v3 <<'\n';
   
}

